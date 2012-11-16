#include "gui/ProgressWidget.hpp"

//===========================================================================
// constructor
//===========================================================================
ProgressWidget::ProgressWidget(QWidget *parent) :
    QWidget(parent), ui(new Ui::ProgressWidget)
{
  ui->setupUi(this);
  ui->progress->setValue(0);
  setAttribute(Qt::WA_TransparentForMouseEvents);
  opacity = 1.0;
  duration = 333;
  connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
}

//===========================================================================
// destructor
//===========================================================================
ProgressWidget::~ProgressWidget()
{
  delete ui;
}

//===========================================================================
// fade
//===========================================================================
void ProgressWidget::show()
{
  timer.stop();
  setOpacity(1.0);
  QWidget::show();
}

//===========================================================================
// fade
//===========================================================================
void ProgressWidget::fade(int millis)
{
  if (!isVisible()) return;
  if (millis > 0) {
    duration = millis;
  }
  int num = duration/40;
  if (num == 0) {
    setVisible(false);
    return;
  }
  if (num > 255) {
    duration = 40*255;
  }
  opacity = 1.0;
  timer.start(40);
  show();
}

//===========================================================================
// paintEvent
//===========================================================================
void ProgressWidget::paintEvent(QPaintEvent * /* event */)
{
  opacity -= 40.0/(double)(duration);
  if (opacity > 0.0) {
    setOpacity(opacity);
  }
  else {
    timer.stop();
    setVisible(false);
  }
}

//===========================================================================
// setOpacity
//===========================================================================
void ProgressWidget::setOpacity(double val)
{
  opacity = val;
  setWindowOpacity(val);
  /*
  ui->frame->setWindowOpacity(val);
  ui->label->setWindowOpacity(val);
  ui->progress->setWindowOpacity(val);
  */
}

