#include "simpleslider.h"

SimpleSlider::SimpleSlider(QWidget *parent) :
    QProgressBar(parent)
{
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    setFormat("%v");
    setValue(0);
}

void SimpleSlider::mouseMoveEvent(QMouseEvent *event)
{
    int newVal;
    if (orientation() == Qt::Vertical)
       newVal = minimum() + ((maximum()-minimum()) * (height()-event->y())) / height();
    else
       newVal = minimum() + ((maximum()-minimum()) * event->x()) / width();

    if (invertedAppearance() == true)
        setValue( maximum() - newVal );
    else
        setValue(newVal);

    event->accept();

    QProgressBar::mouseMoveEvent(event);
}
