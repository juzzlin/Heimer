#ifndef DOUBLESLIDER_H
#define DOUBLESLIDER_H

#include <QStackedWidget>
#include "simple_slider.hpp"
#include <QDoubleSpinBox>
#include <QtMath>

class DoubleSlider : public QStackedWidget
{
    Q_OBJECT
public:
    explicit DoubleSlider(QWidget *parent = nullptr);

    double minimum() const;
    void setMinimum(double minimum);

    double maximum() const;
    void setMaximum(double maximum);

    QString prefix() const;
    void setPrefix(const QString &prefix);

    QString suffix() const;
    void setSuffix(const QString &suffix);

    int decimals() const;
    void setDecimals(int decimals);

    double value() const;

    QDoubleSpinBox *spinBox() const;

    SimpleSlider *slider() const;

signals:
    void valueChanged(double);

public slots:
    void setEditMode();
    void setSliderMode();
    void setValue(double value);
    void setValue(int value);

private:
    SimpleSlider *_slider;
    QDoubleSpinBox *_spinBox;
    bool _mouseMoving;
    double _value;
    double _minimum;
    double _maximum;
    QString _prefix;
    QString _suffix;
    int _decimals;

private slots:
    void sliderValue(int value);
    void updateText();

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

};

#endif // DOUBLESLIDER_H
