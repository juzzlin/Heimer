#include "duqfslider.h"

DuQFSlider::DuQFSlider(QWidget *parent):
    QProgressBar(parent)
{
    setFormat("");
    _prefix = "";
    _suffix = "";
    _showValue = true;
    setValue(0);
}

QString DuQFSlider::prefix() const
{
    return _prefix;
}

void DuQFSlider::setPrefix(const QString &prefix)
{
    _prefix = prefix;
}

QString DuQFSlider::suffix() const
{
    return _suffix;
}

void DuQFSlider::setSuffix(const QString &suffix)
{
    _suffix = suffix;
}

QString DuQFSlider::text() const
{
    if (value() < minimum()) return "-";
    if (!_showValue) return _prefix + _suffix;
    return _prefix + QString::number(value()) + _suffix;
}

bool DuQFSlider::valueVisible() const
{
    return _showValue;
}

void DuQFSlider::showValue(bool showValue)
{
    _showValue = showValue;
}

void DuQFSlider::mouseMoveEvent(QMouseEvent *event)
{
    double newVal;
    if (orientation() == Qt::Vertical)
        newVal = minimum() + ((maximum()-minimum()) * (height()-event->y())) / height();
    else
       newVal = minimum() + ((maximum()-minimum()) * event->x()) / width();

    if (invertedAppearance() == true)
        newVal = maximum() - newVal;

    setValue(newVal);
    emit valueChanged(newVal);
    repaint();
    event->accept();

    QProgressBar::mouseMoveEvent(event);
}
