#include "duqfdoublespinbox.h"

DuQFDoubleSpinBox::DuQFDoubleSpinBox(QWidget *parent):
    QStackedWidget( parent )
{
    setupUi();
    connectEvents();

    setDecimals(2);
    setMinimum(0);
    setMaximum(100);
    setValue(0);

    this->setCurrentIndex(1);
}

void DuQFDoubleSpinBox::setupUi()
{
    QWidget *spinBoxPage = new QWidget();

    QHBoxLayout *horizontalLayout = new QHBoxLayout(spinBoxPage);
    horizontalLayout->setSpacing(0);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    spinBox = new QDoubleSpinBox(spinBoxPage);
    spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    horizontalLayout->addWidget(spinBox);

    this->addWidget(spinBoxPage);

    QWidget *sliderPage = new QWidget();

    QHBoxLayout *sliderLayout = new QHBoxLayout(sliderPage);
    sliderLayout->setSpacing(0);
    sliderLayout->setContentsMargins(0, 0, 0, 0);

    slider = new DuQFDoubleSlider();
    sliderLayout->addWidget(slider);

    this->addWidget(sliderPage);

    spinBox->installEventFilter(this);
}

void DuQFDoubleSpinBox::connectEvents()
{
    connect(slider,SIGNAL(valueChanged(double)), this, SLOT(slider_valueChanged(double)));
    connect(spinBox,SIGNAL(editingFinished()), this, SLOT(spinBox_editingFinished()));
    connect(spinBox,SIGNAL(valueChanged(double)), this, SLOT(spinBox_valueChanged(double)));
}

QString DuQFDoubleSpinBox::prefix() const
{
    return slider->prefix();
}

void DuQFDoubleSpinBox::setPrefix(const QString &prefix)
{
    slider->setPrefix(prefix);
}

QString DuQFDoubleSpinBox::suffix() const
{
    return slider->suffix();
}

void DuQFDoubleSpinBox::setSuffix(const QString &suffix)
{
    slider->setSuffix(suffix);
}

int DuQFDoubleSpinBox::decimals() const
{
    return spinBox->decimals();
}

void DuQFDoubleSpinBox::setDecimals(int d)
{
    spinBox->setDecimals(d);
    slider->setDecimals(d);
}

int DuQFDoubleSpinBox::maximum() const
{
    return spinBox->maximum();
}

void DuQFDoubleSpinBox::setMaximum(int max)
{
    spinBox->setMaximum(max);
    slider->setMaximum(max);
}

int DuQFDoubleSpinBox::minimum() const
{
    return spinBox->minimum();
}

void DuQFDoubleSpinBox::setMinimum(int min)
{
    spinBox->setMinimum(min);
    slider->setMinimum(min);
}

bool DuQFDoubleSpinBox::valueVisible() const
{
    return slider->valueVisible();
}

void DuQFDoubleSpinBox::showValue(bool showValue)
{
    slider->showValue(showValue);
}

double DuQFDoubleSpinBox::value() const
{
    return spinBox->value();
}

void DuQFDoubleSpinBox::setValue(double value)
{
    QSignalBlocker b1(spinBox);
    QSignalBlocker b2(slider);
    spinBox->setValue(value);
    slider->setValue(value);
}

void DuQFDoubleSpinBox::spinBox_editingFinished()
{
     this->setCurrentIndex(1);
}

void DuQFDoubleSpinBox::spinBox_valueChanged(double arg1)
{
    QSignalBlocker b1(slider);
    slider->setValue( arg1 );
    emit valueChanged(arg1);
}

void DuQFDoubleSpinBox::slider_valueChanged(double arg1)
{
    QSignalBlocker b1(spinBox);
    spinBox->setValue( arg1 );
    emit valueChanged(arg1);
}

void DuQFDoubleSpinBox::mouseReleaseEvent(QMouseEvent *event)
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
    spinBox->selectAll();
    spinBox->setFocus();
    event->accept();
}

void DuQFDoubleSpinBox::mousePressEvent(QMouseEvent *event)
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

void DuQFDoubleSpinBox::focusOutEvent(QFocusEvent *event)
{
    this->setCurrentIndex(1);
    event->accept();
}

bool DuQFDoubleSpinBox::eventFilter(QObject *obj, QEvent *event)
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
