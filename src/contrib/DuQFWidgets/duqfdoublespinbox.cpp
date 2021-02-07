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

    _spinBox = new QDoubleSpinBox(spinBoxPage);
    //_spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    horizontalLayout->addWidget(_spinBox);

    this->addWidget(spinBoxPage);

    QWidget *sliderPage = new QWidget();

    QHBoxLayout *sliderLayout = new QHBoxLayout(sliderPage);
    sliderLayout->setSpacing(0);
    sliderLayout->setContentsMargins(0, 0, 0, 0);

    _slider = new DuQFDoubleSlider();
    sliderLayout->addWidget(_slider);

    this->addWidget(sliderPage);

    _spinBox->installEventFilter(this);
}

void DuQFDoubleSpinBox::connectEvents()
{
    connect(_slider,SIGNAL(valueChanged(double)), this, SLOT(slider_valueChanged(double)));
    connect(_spinBox,SIGNAL(editingFinished()), this, SLOT(spinBox_editingFinished()));
    connect(_spinBox,SIGNAL(valueChanged(double)), this, SLOT(spinBox_valueChanged(double)));
}

DuQFDoubleSlider *DuQFDoubleSpinBox::slider() const
{
    return _slider;
}

QDoubleSpinBox *DuQFDoubleSpinBox::spinBox() const
{
    return _spinBox;
}

QString DuQFDoubleSpinBox::prefix() const
{
    return _slider->prefix();
}

void DuQFDoubleSpinBox::setPrefix(const QString &prefix)
{
    _slider->setPrefix(prefix);
}

QString DuQFDoubleSpinBox::suffix() const
{
    return _slider->suffix();
}

void DuQFDoubleSpinBox::setSuffix(const QString &suffix)
{
    _slider->setSuffix(suffix);
}

int DuQFDoubleSpinBox::decimals() const
{
    return _spinBox->decimals();
}

void DuQFDoubleSpinBox::setDecimals(int d)
{
    _spinBox->setDecimals(d);
    _slider->setDecimals(d);
}

int DuQFDoubleSpinBox::maximum() const
{
    return _spinBox->maximum();
}

void DuQFDoubleSpinBox::setMaximum(int max)
{
    _spinBox->setMaximum(max);
    _slider->setMaximum(max);
}

int DuQFDoubleSpinBox::minimum() const
{
    return _spinBox->minimum();
}

void DuQFDoubleSpinBox::setMinimum(int min)
{
    _spinBox->setMinimum(min);
    _slider->setMinimum(min);
}

bool DuQFDoubleSpinBox::valueVisible() const
{
    return _slider->valueVisible();
}

void DuQFDoubleSpinBox::showValue(bool showValue)
{
    _slider->showValue(showValue);
}

double DuQFDoubleSpinBox::value() const
{
    return _spinBox->value();
}

void DuQFDoubleSpinBox::setValue(double value)
{
    QSignalBlocker b1(_spinBox);
    QSignalBlocker b2(_slider);
    _spinBox->setValue(value);
    _slider->setValue(value);
}

void DuQFDoubleSpinBox::spinBox_editingFinished()
{
     this->setCurrentIndex(1);
}

void DuQFDoubleSpinBox::spinBox_valueChanged(double arg1)
{
    QSignalBlocker b1(_slider);
    _slider->setValue( arg1 );
    emit valueChanged(arg1);
}

void DuQFDoubleSpinBox::slider_valueChanged(double arg1)
{
    QSignalBlocker b1(_spinBox);
    _spinBox->setValue( arg1 );
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
    _spinBox->selectAll();
    _spinBox->setFocus();
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
