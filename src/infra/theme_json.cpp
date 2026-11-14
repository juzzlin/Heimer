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

#include "theme_json.hpp"

#include <QJsonArray>
#include <QJsonDocument>

namespace ThemeJson {

namespace Keys {

const auto FORMAT_VERSION = "formatVersion";
const auto THEMES = "themes";

const auto NAME = "name";
const auto BACKGROUND_COLOR = "backgroundColor";
const auto GRID_COLOR = "gridColor";
const auto NODE_COLOR = "nodeColor";
const auto NODE_TEXT_COLOR = "nodeTextColor";
const auto CORNER_RADIUS = "cornerRadius";
const auto FONT = "font";
const auto TEXT_SIZE = "textSize";
const auto EDGE_COLOR = "edgeColor";
const auto EDGE_WIDTH = "edgeWidth";
const auto ARROW_SIZE = "arrowSize";
const auto EDGE_LABEL_COLOR = "edgeLabelColor";
const auto EDGE_LABEL_TEXT_COLOR = "edgeLabelTextColor";

const auto SHADOW = "shadow";
const auto SHADOW_OFFSET = "offset";
const auto SHADOW_BLUR_RADIUS = "blurRadius";
const auto SHADOW_SELECTED_ITEM_BLUR_RADIUS = "selectedItemBlurRadius";
const auto SHADOW_COLOR = "color";
const auto SHADOW_SELECTED_ITEM_COLOR = "selectedItemColor";

} // namespace Keys

static const int formatVersion = 1;

static QString colorToString(const QColor & color)
{
    return color.name(QColor::HexArgb);
}

static void readColor(const QJsonObject & json, const char * key, QColor & color)
{
    if (const auto value = json.value(key); value.isString()) {
        if (const QColor parsed { value.toString() }; parsed.isValid()) {
            color = parsed;
        }
    }
}

static void readInt(const QJsonObject & json, const char * key, int & value)
{
    if (const auto jsonValue = json.value(key); jsonValue.isDouble()) {
        value = jsonValue.toInt();
    }
}

static void readDouble(const QJsonObject & json, const char * key, double & value)
{
    if (const auto jsonValue = json.value(key); jsonValue.isDouble()) {
        value = jsonValue.toDouble();
    }
}

QJsonObject toJson(const Theme & theme)
{
    QJsonObject shadow;
    shadow[Keys::SHADOW_OFFSET] = theme.shadowEffect.offset();
    shadow[Keys::SHADOW_BLUR_RADIUS] = theme.shadowEffect.blurRadius();
    shadow[Keys::SHADOW_SELECTED_ITEM_BLUR_RADIUS] = theme.shadowEffect.selectedItemBlurRadius();
    shadow[Keys::SHADOW_COLOR] = colorToString(theme.shadowEffect.shadowColor());
    shadow[Keys::SHADOW_SELECTED_ITEM_COLOR] = colorToString(theme.shadowEffect.selectedItemShadowColor());

    QJsonObject json;
    json[Keys::NAME] = theme.name;
    json[Keys::BACKGROUND_COLOR] = colorToString(theme.backgroundColor);
    json[Keys::GRID_COLOR] = colorToString(theme.gridColor);
    json[Keys::NODE_COLOR] = colorToString(theme.nodeColor);
    json[Keys::NODE_TEXT_COLOR] = colorToString(theme.nodeTextColor);
    json[Keys::CORNER_RADIUS] = theme.cornerRadius;
    json[Keys::FONT] = theme.font.toString();
    json[Keys::TEXT_SIZE] = theme.textSize;
    json[Keys::EDGE_COLOR] = colorToString(theme.edgeColor);
    json[Keys::EDGE_WIDTH] = theme.edgeWidth;
    json[Keys::ARROW_SIZE] = theme.arrowSize;
    json[Keys::EDGE_LABEL_COLOR] = colorToString(theme.edgeLabelColor);
    json[Keys::EDGE_LABEL_TEXT_COLOR] = colorToString(theme.edgeLabelTextColor);
    json[Keys::SHADOW] = shadow;
    return json;
}

Theme fromJson(const QJsonObject & json)
{
    Theme theme;
    theme.name = json.value(Keys::NAME).toString();
    readColor(json, Keys::BACKGROUND_COLOR, theme.backgroundColor);
    readColor(json, Keys::GRID_COLOR, theme.gridColor);
    readColor(json, Keys::NODE_COLOR, theme.nodeColor);
    readColor(json, Keys::NODE_TEXT_COLOR, theme.nodeTextColor);
    readInt(json, Keys::CORNER_RADIUS, theme.cornerRadius);
    if (const auto font = json.value(Keys::FONT); font.isString()) {
        theme.font.fromString(font.toString());
    }
    readInt(json, Keys::TEXT_SIZE, theme.textSize);
    readColor(json, Keys::EDGE_COLOR, theme.edgeColor);
    readDouble(json, Keys::EDGE_WIDTH, theme.edgeWidth);
    readDouble(json, Keys::ARROW_SIZE, theme.arrowSize);
    readColor(json, Keys::EDGE_LABEL_COLOR, theme.edgeLabelColor);
    readColor(json, Keys::EDGE_LABEL_TEXT_COLOR, theme.edgeLabelTextColor);

    if (const auto shadowValue = json.value(Keys::SHADOW); shadowValue.isObject()) {
        const auto shadow = shadowValue.toObject();
        int offset = theme.shadowEffect.offset();
        int blurRadius = theme.shadowEffect.blurRadius();
        int selectedItemBlurRadius = theme.shadowEffect.selectedItemBlurRadius();
        QColor shadowColor = theme.shadowEffect.shadowColor();
        QColor selectedItemShadowColor = theme.shadowEffect.selectedItemShadowColor();
        readInt(shadow, Keys::SHADOW_OFFSET, offset);
        readInt(shadow, Keys::SHADOW_BLUR_RADIUS, blurRadius);
        readInt(shadow, Keys::SHADOW_SELECTED_ITEM_BLUR_RADIUS, selectedItemBlurRadius);
        readColor(shadow, Keys::SHADOW_COLOR, shadowColor);
        readColor(shadow, Keys::SHADOW_SELECTED_ITEM_COLOR, selectedItemShadowColor);
        theme.shadowEffect = { offset, blurRadius, selectedItemBlurRadius, shadowColor, selectedItemShadowColor };
    }

    return theme;
}

QByteArray toDocument(const std::vector<Theme> & themes)
{
    QJsonArray themeArray;
    for (auto && theme : themes) {
        themeArray.append(toJson(theme));
    }

    QJsonObject root;
    root[Keys::FORMAT_VERSION] = formatVersion;
    root[Keys::THEMES] = themeArray;
    return QJsonDocument { root }.toJson(QJsonDocument::Indented);
}

std::optional<std::vector<Theme>> fromDocument(const QByteArray & document)
{
    QJsonParseError error;
    const auto jsonDocument = QJsonDocument::fromJson(document, &error);
    if (error.error != QJsonParseError::NoError || !jsonDocument.isObject()) {
        return {};
    }

    const auto themesValue = jsonDocument.object().value(Keys::THEMES);
    if (!themesValue.isArray()) {
        return {};
    }

    std::vector<Theme> themes;
    for (auto && themeValue : themesValue.toArray()) {
        if (themeValue.isObject()) {
            if (auto theme = fromJson(themeValue.toObject()); !theme.name.isEmpty()) {
                themes.push_back(theme);
            }
        }
    }
    return themes;
}

} // namespace ThemeJson
