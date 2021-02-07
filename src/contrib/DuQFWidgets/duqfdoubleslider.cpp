#include "duqfdoubleslider.h"

DuQFDoubleSlider::DuQFDoubleSlider(QWidget *parent):
    QProgressBar( parent )
{
    _locale = QLocale::system();
    setFormat("");
    setDecimals(2);
    setMaximum(100);
    setMinimum(0);
    setValue(0);
    _showValue = true;
}

void DuQFDoubleSlider::setValue(double v)
{
    _value = v;
    QProgressBar::setValue(v * _multiplicator);
    emit valueChanged(v);
    repaint();
}

double DuQFDoubleSlider::value() const
{
    return double(_value) / double(_multiplicator);
}

int DuQFDoubleSlider::decimals() const
{
    return _decimals;
}

void DuQFDoubleSlider::setDecimals(int d)
{
    _decimals = d;
    _multiplicator = qPow(10, d);
}

double DuQFDoubleSlider::maximum() const
{
    return double(QProgressBar::maximum()) / double(_multiplicator);
}

void DuQFDoubleSlider::setMaximum(int max)
{
    QProgressBar::setMaximum(max * _multiplicator);
}

void DuQFDoubleSlider::setMaximum(double m)
{
    QProgressBar::setMaximum(m * _multiplicator);
}

double DuQFDoubleSlider::minimum() const
{
    return double(QProgressBar::minimum()) / double(_multiplicator);
}

void DuQFDoubleSlider::setMinimum(double m)
{
    QProgressBar::setMinimum(m * _multiplicator);
}

QString DuQFDoubleSlider::text() const
{
    if (_value < minimum()) return "";
    if (!_showValue) return _prefix + _suffix;
    QString t = _locale.toString(_value,'f',_decimals);
    return _prefix + t + _suffix;
}

QString DuQFDoubleSlider::prefix() const
{
    return _prefix;
}

void DuQFDoubleSlider::setPrefix(const QString &prefix)
{
    _prefix = prefix;
}

QString DuQFDoubleSlider::suffix() const
{
    return _suffix;
}

void DuQFDoubleSlider::setSuffix(const QString &suffix)
{
    _suffix = suffix;
}

void DuQFDoubleSlider::showValue(bool show)
{
    _showValue = show;
}

bool DuQFDoubleSlider::valueVisible() const
{
    return _showValue;
}

void DuQFDoubleSlider::mouseMoveEvent(QMouseEvent *event)
{
    double newVal;
    if (orientation() == Qt::Vertical)
    {
        double h(height());
        double y(event->y());
        newVal = minimum() + ((maximum()-minimum()) * (h - y)) / h;
    }
    else
    {
        double w(width());
        double x(event->x());
        newVal = minimum() + ((maximum()-minimum()) * x) / w;
    }

    if (invertedAppearance() == true)
        newVal = maximum() - newVal;

    if (newVal > maximum()) newVal = maximum();
    else if (newVal < minimum()) newVal = minimum();

    setValue(newVal);

    event->accept();

    QProgressBar::mouseMoveEvent(event);
}
