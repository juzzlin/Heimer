#include "duqfspinbox.h"

DuQFSpinBox::DuQFSpinBox(QWidget *parent) :
    QStackedWidget(parent)
{
    setupUi();
    connectEvents();

    _prefix = "";
    _suffix = "";

    setMaximum(100);
    setMinimum(0);
    setValue(0);

    this->setCurrentIndex(1);
}

void DuQFSpinBox::setupUi()
{
    QWidget *spinBoxPage = new QWidget();

    QHBoxLayout *horizontalLayout = new QHBoxLayout(spinBoxPage);
    horizontalLayout->setSpacing(0);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    spinBox = new QSpinBox(spinBoxPage);
    spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    horizontalLayout->addWidget(spinBox);

    this->addWidget(spinBoxPage);

    QWidget *sliderPage = new QWidget();

    QHBoxLayout *sliderLayout = new QHBoxLayout(sliderPage);
    sliderLayout->setSpacing(0);
    sliderLayout->setContentsMargins(0, 0, 0, 0);

    slider = new DuQFSlider();
    sliderLayout->addWidget(slider);

    this->addWidget(sliderPage);

    spinBox->installEventFilter(this);
}

void DuQFSpinBox::connectEvents()
{
    connect(slider,SIGNAL(valueChanged(int)), this, SLOT(slider_valueChanged(int)));
    connect(spinBox,SIGNAL(editingFinished()), this, SLOT(spinBox_editingFinished()));
    connect(spinBox,SIGNAL(valueChanged(int)), this, SLOT(spinBox_valueChanged(int)));
}

void DuQFSpinBox::spinBox_editingFinished()
{
    this->setCurrentIndex(1);
}

void DuQFSpinBox::spinBox_valueChanged(int arg1)
{
    slider->setValue(arg1);
    emit valueChanged(arg1);
}

void DuQFSpinBox::slider_valueChanged(int arg1)
{
    spinBox->setValue(arg1);
}

bool DuQFSpinBox::valueVisible() const
{
    return slider->valueVisible();
}

void DuQFSpinBox::showValue(bool showValue)
{
    slider->showValue(showValue);
}

int DuQFSpinBox::value() const
{
    return spinBox->value();
}

void DuQFSpinBox::setValue(int value)
{
    QSignalBlocker b1(spinBox);
    QSignalBlocker b2(slider);
    spinBox->setValue(value);
    slider->setValue(value);
}

int DuQFSpinBox::maximum() const
{
    return spinBox->maximum();
}

void DuQFSpinBox::setMaximum(int max)
{
    spinBox->setMaximum(max);
    slider->setMaximum(max);
}

int DuQFSpinBox::minimum() const
{
    return spinBox->minimum();
}

void DuQFSpinBox::setMinimum(int min)
{
    spinBox->setMinimum(min);
    slider->setMinimum(min);
}

QString DuQFSpinBox::prefix() const
{
    return slider->prefix();
}

void DuQFSpinBox::setPrefix(const QString &prefix)
{
    slider->setPrefix(prefix);
}

QString DuQFSpinBox::suffix() const
{
    return slider->suffix();
}

void DuQFSpinBox::setSuffix(const QString &suffix)
{
    slider->setSuffix(suffix);
}

void DuQFSpinBox::mouseReleaseEvent(QMouseEvent *event)
{
    if (this->currentIndex() == 0)
    {
        event->ignore();
        return;
    }
    if (event->button() != Qt::LeftButton)
    {
        event->ignore();
        return;
    }
    if (_originalMousePos != event->pos())
    {
        event->ignore();
        return;
    }
    this->setCurrentIndex(0);
    spinBox->selectAll();
    spinBox->setFocus();
    event->accept();
}

void DuQFSpinBox::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        _originalMousePos = event->pos();
    }

    if (event->y() < 0 || event->y() > height() || event->x() < 0 || event->x() > width())
    {
        this->setCurrentIndex(1);
        event->accept();
        return;
    }

    event->ignore();
}

void DuQFSpinBox::focusOutEvent(QFocusEvent *event)
{
    this->setCurrentIndex(1);
    event->accept();
}

bool DuQFSpinBox::eventFilter(QObject *obj, QEvent *event)
{
  if (event->type() == QEvent::FocusOut)
  {
      this->setCurrentIndex(1);
      return true;
  }
  else
  {
      // standard event processing
      return QObject::eventFilter(obj, event);
  }
}
