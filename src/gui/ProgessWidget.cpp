#include "gui/ProgressWidget.hpp"

//===========================================================================
// constructor
//===========================================================================
ProgressWidget::ProgressWidget(QWidget *parent) :
    QWidget(parent), ui(new Ui::ProgressWidget)
{
  ui->setupUi(this);
  ui->progress->setValue(0);
  //setAttribute(Qt::WA_TransparentForMouseEvents);
  setOpacity(1.0);
  duration = 333;
  connect(&timer, SIGNAL(timeout()), this, SLOT(decrease()));
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
  if (!isVisible() || timer.isActive()) return;
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
  setOpacity(1.0);
  timer.start(40);
}

//===========================================================================
// decrease
//===========================================================================
void ProgressWidget::decrease()
{
  opacity -= 40.0/(double)(duration);
  if (opacity > 0.0) {
    setOpacity(opacity);
  }
  else {
    setOpacity(0.0);
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
}

