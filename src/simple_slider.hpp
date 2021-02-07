#ifndef SIMPLESLIDER_H
#define SIMPLESLIDER_H

#include <QProgressBar>
#include <QMouseEvent>

class SimpleSlider : public QProgressBar
{
    Q_OBJECT
public:
    SimpleSlider(QWidget *parent = nullptr);

protected:
    void mouseMoveEvent(QMouseEvent *event);

};

#endif // SIMPLESLIDER_H
