// This file is part of Heimer.
// Copyright (C) 2026 Jussi Lind <jussi.lind@iki.fi>
//
// Heimer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Heimer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Heimer. If not, see <http://www.gnu.org/licenses/>.

#include "themes_tab.hpp"

#include "../../application/application_service.hpp"
#include "../../application/service_container.hpp"
#include "../../application/theme_service.hpp"
#include "../../common/constants.hpp"
#include "../../domain/mind_map_data.hpp"
#include "../../domain/theme.hpp"
#include "../theme_preview.hpp"
#include "theme_editor_dialog.hpp"

#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

namespace Dialogs {

static const QSize iconSize { 96, 48 };

ThemesTab::ThemesTab(QString name, ThemeServiceS themeService, QWidget * parent)
  : SettingsTabBase { name, parent }
  , m_themeService(themeService)
  , m_themeList(new QListWidget { this })
  , m_applyButton(new QPushButton { tr("Apply to current mind map"), this })
  , m_setDefaultButton(new QPushButton { tr("Set as default for new mind maps"), this })
  , m_newButton(new QPushButton { tr("New from current mind map") + Constants::Misc::threeDots(), this })
  , m_duplicateButton(new QPushButton { tr("Duplicate") + Constants::Misc::threeDots(), this })
  , m_editButton(new QPushButton { tr("Edit") + Constants::Misc::threeDots(), this })
  , m_renameButton(new QPushButton { tr("Rename") + Constants::Misc::threeDots(), this })
  , m_deleteButton(new QPushButton { tr("Delete"), this })
{
    initWidgets();

    refresh(m_themeService->defaultTheme().name);
}

void ThemesTab::initWidgets()
{
    m_themeList->setIconSize(iconSize);
    m_themeList->setSpacing(2);
    connect(m_themeList, &QListWidget::currentItemChanged, this, &ThemesTab::updateButtons);
    connect(m_themeList, &QListWidget::itemDoubleClicked, this, &ThemesTab::applySelectedTheme);

    connect(m_applyButton, &QPushButton::clicked, this, &ThemesTab::applySelectedTheme);
    connect(m_setDefaultButton, &QPushButton::clicked, this, &ThemesTab::setSelectedThemeAsDefault);
    connect(m_newButton, &QPushButton::clicked, this, &ThemesTab::createThemeFromCurrentMindMap);
    connect(m_duplicateButton, &QPushButton::clicked, this, &ThemesTab::duplicateSelectedTheme);
    connect(m_editButton, &QPushButton::clicked, this, &ThemesTab::editSelectedTheme);
    connect(m_renameButton, &QPushButton::clicked, this, &ThemesTab::renameSelectedTheme);
    connect(m_deleteButton, &QPushButton::clicked, this, &ThemesTab::deleteSelectedTheme);

    const auto buttonLayout = new QVBoxLayout;
    for (auto && button : { m_applyButton, m_setDefaultButton }) {
        buttonLayout->addWidget(button);
    }
    buttonLayout->addSpacing(12);
    for (auto && button : { m_newButton, m_duplicateButton, m_editButton, m_renameButton, m_deleteButton }) {
        buttonLayout->addWidget(button);
    }
    buttonLayout->addStretch();

    const auto listLayout = new QHBoxLayout;
    listLayout->addWidget(m_themeList, 1);
    listLayout->addLayout(buttonLayout);

    const auto help = new QLabel { tr("A theme defines the colors, fonts and effects of a mind map. Built-in themes can't be modified, but they can be duplicated."), this };
    help->setWordWrap(true);

    const auto mainLayout = new QVBoxLayout;
    mainLayout->addWidget(help);
    mainLayout->addLayout(listLayout);
    setLayout(mainLayout);
}

void ThemesTab::refresh(QString selectedName)
{
    m_themeList->clear();

    const auto defaultThemeName = m_themeService->defaultTheme().name;
    for (auto && theme : m_themeService->themes()) {
        auto text = theme.name;
        if (m_themeService->isFactoryTheme(theme.name)) {
            text += " " + tr("(built-in)");
        }
        if (theme.name == defaultThemeName) {
            text += " - " + tr("default for new mind maps");
        }

        const auto item = new QListWidgetItem { QIcon { ThemePreview::render(theme, iconSize) }, text, m_themeList };
        item->setData(Qt::UserRole, theme.name);
        if (theme.name == defaultThemeName) {
            auto font = item->font();
            font.setBold(true);
            item->setFont(font);
        }
    }

    selectTheme(selectedName);
    updateButtons();
}

QString ThemesTab::selectedThemeName() const
{
    const auto item = m_themeList->currentItem();
    return item ? item->data(Qt::UserRole).toString() : QString {};
}

void ThemesTab::selectTheme(const QString & name)
{
    for (int i = 0; i < m_themeList->count(); i++) {
        if (m_themeList->item(i)->data(Qt::UserRole).toString() == name) {
            m_themeList->setCurrentRow(i);
            return;
        }
    }
    m_themeList->setCurrentRow(0);
}

void ThemesTab::updateButtons()
{
    const auto name = selectedThemeName();
    const auto isSelected = !name.isEmpty();
    const auto isUserTheme = m_themeService->isUserTheme(name);

    m_applyButton->setEnabled(isSelected);
    m_setDefaultButton->setEnabled(isSelected);
    m_duplicateButton->setEnabled(isSelected);
    m_editButton->setEnabled(isUserTheme);
    m_renameButton->setEnabled(isUserTheme);
    m_deleteButton->setEnabled(isUserTheme);
}

std::optional<QString> ThemesTab::askNewThemeName(QString title, QString initialName)
{
    auto name = initialName;
    while (true) {
        bool ok = false;
        name = QInputDialog::getText(this, title, tr("Theme name:"), QLineEdit::Normal, name, &ok).trimmed();
        if (!ok) {
            return {};
        }
        if (name.isEmpty()) {
            continue;
        }
        if (m_themeService->theme(name)) {
            QMessageBox::warning(this, title, tr("A theme named '%1' already exists.").arg(name));
            continue;
        }
        return name;
    }
}

void ThemesTab::applySelectedTheme()
{
    if (const auto theme = m_themeService->theme(selectedThemeName()); theme) {
        applyTheme(*theme, this);
    }
}

void ThemesTab::applyTheme(const Theme & theme, QWidget * dialogParent)
{
    const auto applicationService = SC::instance().applicationService();
    bool applyNodeColors = true;
    if (applicationService->hasIndividualNodeColors()) {
        QMessageBox messageBox { dialogParent };
        messageBox.setWindowTitle(tr("Apply Theme"));
        messageBox.setIcon(QMessageBox::Question);
        messageBox.setText(tr("Some nodes have individual colors."));
        messageBox.setInformativeText(tr("Do you want to apply the node colors of the theme also to these nodes?"));
        const auto applyToAllButton = messageBox.addButton(tr("Apply to all nodes"), QMessageBox::YesRole);
        const auto keepColorsButton = messageBox.addButton(tr("Keep individual colors"), QMessageBox::NoRole);
        messageBox.addButton(QMessageBox::Cancel);
        messageBox.setDefaultButton(keepColorsButton);
        messageBox.exec();
        if (messageBox.clickedButton() == applyToAllButton) {
            applyNodeColors = true;
        } else if (messageBox.clickedButton() == keepColorsButton) {
            applyNodeColors = false;
        } else {
            return;
        }
    }

    applicationService->applyTheme(theme, applyNodeColors);
}

void ThemesTab::setSelectedThemeAsDefault()
{
    const auto name = selectedThemeName();
    if (m_themeService->setDefaultThemeName(name)) {
        refresh(name);
    }
}

void ThemesTab::createThemeFromCurrentMindMap()
{
    if (const auto name = askNewThemeName(tr("New Theme"), tr("My theme")); name) {
        auto theme = SC::instance().applicationService()->mindMapData()->theme();
        theme.name = *name;
        if (m_themeService->saveUserTheme(theme)) {
            refresh(*name);
        }
    }
}

void ThemesTab::duplicateSelectedTheme()
{
    auto theme = m_themeService->theme(selectedThemeName());
    if (!theme) {
        return;
    }

    if (const auto name = askNewThemeName(tr("Duplicate Theme"), theme->name + " " + tr("(copy)")); name) {
        theme->name = *name;
        if (m_themeService->saveUserTheme(*theme)) {
            refresh(*name);
        }
    }
}

void ThemesTab::editSelectedTheme()
{
    const auto name = selectedThemeName();
    if (const auto theme = m_themeService->theme(name); theme && m_themeService->isUserTheme(name)) {
        ThemeEditorDialog editor { *theme, this };
        connect(&editor, &ThemeEditorDialog::applied, this, [this, &editor, name](const Theme & editedTheme) {
            if (m_themeService->saveUserTheme(editedTheme)) {
                refresh(name);
                applyTheme(editedTheme, &editor);
            }
        });
        if (editor.exec() == QDialog::Accepted && m_themeService->saveUserTheme(editor.theme())) {
            refresh(name);
        }
    }
}

void ThemesTab::renameSelectedTheme()
{
    const auto name = selectedThemeName();
    if (!m_themeService->isUserTheme(name)) {
        return;
    }

    if (const auto newName = askNewThemeName(tr("Rename Theme"), name); newName && m_themeService->renameUserTheme(name, *newName)) {
        refresh(*newName);
    }
}

void ThemesTab::deleteSelectedTheme()
{
    const auto name = selectedThemeName();
    if (!m_themeService->isUserTheme(name)) {
        return;
    }

    if (QMessageBox::question(this, tr("Delete Theme"), tr("Delete the theme '%1'?").arg(name)) == QMessageBox::Yes) {
        m_themeService->removeUserTheme(name);
        refresh({});
    }
}

} // namespace Dialogs
