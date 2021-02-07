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

    _spinBox = new QSpinBox(spinBoxPage);
    //_spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    horizontalLayout->addWidget(_spinBox);

    this->addWidget(spinBoxPage);

    QWidget *sliderPage = new QWidget();

    QHBoxLayout *sliderLayout = new QHBoxLayout(sliderPage);
    sliderLayout->setSpacing(0);
    sliderLayout->setContentsMargins(0, 0, 0, 0);

    _slider = new DuQFSlider();
    sliderLayout->addWidget(_slider);

    this->addWidget(sliderPage);

    _spinBox->installEventFilter(this);
}

void DuQFSpinBox::connectEvents()
{
    connect(_slider,SIGNAL(valueChanged(int)), this, SLOT(slider_valueChanged(int)));
    connect(_spinBox,SIGNAL(editingFinished()), this, SLOT(spinBox_editingFinished()));
    connect(_spinBox,SIGNAL(valueChanged(int)), this, SLOT(spinBox_valueChanged(int)));
}

QSpinBox *DuQFSpinBox::spinBox() const
{
    return _spinBox;
}

DuQFSlider *DuQFSpinBox::slider() const
{
    return _slider;
}

void DuQFSpinBox::spinBox_editingFinished()
{
    this->setCurrentIndex(1);
}

void DuQFSpinBox::spinBox_valueChanged(int arg1)
{
    _slider->setValue(arg1);
    emit valueChanged(arg1);
}

void DuQFSpinBox::slider_valueChanged(int arg1)
{
    _spinBox->setValue(arg1);
}

bool DuQFSpinBox::valueVisible() const
{
    return _slider->valueVisible();
}

void DuQFSpinBox::showValue(bool showValue)
{
    _slider->showValue(showValue);
}

int DuQFSpinBox::value() const
{
    return _spinBox->value();
}

void DuQFSpinBox::setValue(int value)
{
    QSignalBlocker b1(_spinBox);
    QSignalBlocker b2(_slider);
    _spinBox->setValue(value);
    _slider->setValue(value);
}

int DuQFSpinBox::maximum() const
{
    return _spinBox->maximum();
}

void DuQFSpinBox::setMaximum(int max)
{
    _spinBox->setMaximum(max);
    _slider->setMaximum(max);
}

int DuQFSpinBox::minimum() const
{
    return _spinBox->minimum();
}

void DuQFSpinBox::setMinimum(int min)
{
    _spinBox->setMinimum(min);
    _slider->setMinimum(min);
}

QString DuQFSpinBox::prefix() const
{
    return _slider->prefix();
}

void DuQFSpinBox::setPrefix(const QString &prefix)
{
    _slider->setPrefix(prefix);
}

QString DuQFSpinBox::suffix() const
{
    return _slider->suffix();
}

void DuQFSpinBox::setSuffix(const QString &suffix)
{
    _slider->setSuffix(suffix);
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
    _spinBox->selectAll();
    _spinBox->setFocus();
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
