#ifndef INTSLIDER_H
#define INTSLIDER_H

#include <QStackedWidget>
#include "simple_slider.hpp"
#include <QSpinBox>

class IntSlider : public QStackedWidget
{
    Q_OBJECT
public:
    explicit IntSlider(QWidget *parent = nullptr);

    int minimum() const;
    void setMinimum(int minimum);

    int maximum() const;
    void setMaximum(int maximum);

    QString prefix() const;
    void setPrefix(const QString &prefix);

    QString suffix() const;
    void setSuffix(const QString &suffix);

    int value() const;

    SimpleSlider *slider() const;

    QSpinBox *spinBox() const;

signals:
    void valueChanged(int);

public slots:
    void setEditMode();
    void setSliderMode();
    void setValue(int value);

private:
    SimpleSlider *_slider;
    QSpinBox *_spinBox;
    bool _mouseMoving;
    int _value;
    int _minimum;
    int _maximum;
    QString _prefix;
    QString _suffix;

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

};

#endif // INTSLIDER_H
