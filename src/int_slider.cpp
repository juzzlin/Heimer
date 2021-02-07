#include "int_slider.hpp"

#include <QtDebug>

IntSlider::IntSlider(QWidget *parent)
    : QStackedWidget(parent)
{
    _mouseMoving = false;
    _minimum = 0;
    _maximum = 100;
    _value = 0;

    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    _slider = new SimpleSlider(this);

    _spinBox = new QSpinBox(this);
    _spinBox->setMinimum(0);
    _spinBox->setMaximum(100);

    addWidget(_slider);
    addWidget(_spinBox);

    connect(_spinBox,SIGNAL(editingFinished()),this,SLOT(setSliderMode()));
    connect(_spinBox,SIGNAL(valueChanged(int)),this,SLOT(setValue(int)));
    connect(_slider,SIGNAL(valueChanged(int)),this,SLOT(setValue(int)));
}

void IntSlider::setEditMode()
{
    setCurrentIndex(1);
}

void IntSlider::setSliderMode()
{
    setCurrentIndex(0);
}

int IntSlider::value() const
{
    return _value;
}

void IntSlider::setValue(int value)
{
    _spinBox->setValue(value);
    _slider->setValue(value);
    _value = value;
    emit valueChanged(_value);
}

QSpinBox *IntSlider::spinBox() const
{
    return _spinBox;
}

SimpleSlider *IntSlider::slider() const
{
    return _slider;
}

QString IntSlider::suffix() const
{
    return _suffix;
}

void IntSlider::setSuffix(const QString &suffix)
{
    _spinBox->setPrefix(suffix);
    _slider->setFormat(_prefix + "%v" + suffix);
    _suffix = suffix;
}

QString IntSlider::prefix() const
{
    return _prefix;
}

void IntSlider::setPrefix(const QString &prefix)
{
    _spinBox->setPrefix(prefix);
    _slider->setFormat(prefix + "%v" + _suffix);
    _prefix = prefix;
}

int IntSlider::maximum() const
{
    return _maximum;
}

void IntSlider::setMaximum(int maximum)
{
    _spinBox->setMaximum(maximum);
    _slider->setMaximum(maximum);
    _maximum = maximum;
}

int IntSlider::minimum() const
{
    return _minimum;
}

void IntSlider::setMinimum(int minimum)
{
    _spinBox->setMinimum(minimum);
    _slider->setMinimum(minimum);
    _minimum = minimum;
}

void IntSlider::mouseMoveEvent(QMouseEvent *event)
{
    _mouseMoving = true;
    event->ignore();
}

void IntSlider::mouseReleaseEvent(QMouseEvent *event)
{
    if (!_mouseMoving)
    {
        setEditMode();
        event->accept();
    }
    else
    {
        _mouseMoving = false;
        event->ignore();
    }
}
