#ifndef DUQFSLIDER_H
#define DUQFSLIDER_H

#include <QProgressBar>
#include <QMouseEvent>

class DuQFSlider : public QProgressBar
{
        Q_OBJECT
public:
    DuQFSlider(QWidget *parent = nullptr);

    QString prefix() const;
    void setPrefix(const QString &prefix);

    QString suffix() const;
    void setSuffix(const QString &suffix);

    virtual QString text() const Q_DECL_OVERRIDE;

    bool valueVisible() const;
    void showValue(bool showValue);

signals:
    void valueChanged(int);
private:
    QString _prefix;
    QString _suffix;
    bool _showValue;
protected:
    void mouseMoveEvent ( QMouseEvent * event ) Q_DECL_OVERRIDE;
};
#endif // DUQFSLIDER_H
