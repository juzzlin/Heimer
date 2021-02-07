#include "double_slider.hpp"

#include <QtDebug>

DoubleSlider::DoubleSlider(QWidget *parent) : QStackedWidget(parent)
{
    _mouseMoving = false;
    _decimals = 2;
    _minimum = 0;
    _maximum = 100;
    _value = 0;

    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    _slider = new SimpleSlider(this);
    _slider->setMaximum(10000);

    _spinBox = new QDoubleSpinBox(this);
    _spinBox->setMinimum(0);
    _spinBox->setMaximum(100);

    addWidget(_slider);
    addWidget(_spinBox);

    connect(_spinBox,SIGNAL(editingFinished()),this,SLOT(setSliderMode()));
    connect(_spinBox,SIGNAL(editingFinished()),this,SLOT(setSliderMode()));
    connect(_spinBox,SIGNAL(valueChanged(double)),this,SLOT(setValue(double)));
    connect(_slider,SIGNAL(valueChanged(int)),this,SLOT(sliderValue(int)));
}

void DoubleSlider::setEditMode()
{
    setCurrentIndex(1);
}

void DoubleSlider::setSliderMode()
{
    setCurrentIndex(0);
}

double DoubleSlider::value() const
{
    return _value;
}

void DoubleSlider::setValue(double value)
{
    _slider->setValue(value * qPow(10,_decimals));
    _spinBox->setValue(value);
    _value = value;
    updateText();
    emit valueChanged(_value);
}

void DoubleSlider::setValue(int value)
{
    _slider->setValue(value);
    _spinBox->setValue(value);
    _value = value;
    updateText();
    emit valueChanged(_value);
}

void DoubleSlider::sliderValue(int value)
{
    setValue(value / qPow(10,_decimals));
}

int DoubleSlider::decimals() const
{
    return _decimals;
}

void DoubleSlider::setDecimals(int decimals)
{
    _spinBox->setDecimals(decimals);
    _decimals = decimals;
}

void DoubleSlider::updateText()
{
    _slider->setFormat(_prefix + QString::number(_value) + _suffix);
}

QString DoubleSlider::suffix() const
{
    return _suffix;
}

void DoubleSlider::setSuffix(const QString &suffix)
{
    _spinBox->setPrefix(suffix);
    _suffix = suffix;
    updateText();
}

QString DoubleSlider::prefix() const
{
    return _prefix;
}

void DoubleSlider::setPrefix(const QString &prefix)
{
    _spinBox->setPrefix(prefix);
    _prefix = prefix;
    updateText();
}

double DoubleSlider::minimum() const
{
    return _minimum;
}

void DoubleSlider::setMinimum(double minimum)
{
    _spinBox->setMinimum(minimum);
    _slider->setMinimum(minimum * qPow(10,_decimals));
    _minimum = minimum;
}

double DoubleSlider::maximum() const
{
    return _maximum;
}

void DoubleSlider::setMaximum(double maximum)
{
    _spinBox->setMaximum(maximum);
    _slider->setMaximum(maximum * qPow(10,_decimals));
    _maximum = maximum;
}

void DoubleSlider::mouseMoveEvent(QMouseEvent *event)
{
    _mouseMoving = true;
    event->ignore();
}

void DoubleSlider::mouseReleaseEvent(QMouseEvent *event)
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

QDoubleSpinBox *DoubleSlider::spinBox() const
{
    return _spinBox;
}

SimpleSlider *DoubleSlider::slider() const
{
    return _slider;
}
