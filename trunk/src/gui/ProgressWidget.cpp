
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2013 Gerard Torrent
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//===========================================================================

#include <QGraphicsOpacityEffect>
#include "gui/ProgressWidget.hpp"

//===========================================================================
// constructor
//===========================================================================
ProgressWidget::ProgressWidget(QWidget *parent) :
    QWidget(parent), ui(new Ui::ProgressWidget)
{
  ui->setupUi(this);
  ui->frame->setStyleSheet("background:"+palette().color(QPalette::Window).name());
  setGraphicsEffect(new QGraphicsOpacityEffect(this));
  setVisible(false);
  isin = true;
  ui->progress->setValue(0);
  setAttribute(Qt::WA_TransparentForMouseEvents);
  setOpacity(1.0);
  duration = 333;
  connect(&timer, SIGNAL(timeout()), this, SLOT(changeOpacity()));
}

//===========================================================================
// destructor
//===========================================================================
ProgressWidget::~ProgressWidget()
{
  delete ui;
}

//===========================================================================
// fadein (0->1)
//===========================================================================
void ProgressWidget::fadein(size_t millis)
{
  mutex.lock();

  if (timer.isActive()) timer.stop();

  if (isVisible() && opacity == 1.0) {
    mutex.unlock();
    return;
  }

  isin = true;
  setOpacity(0.0);
  setVisible(true);

  if (millis > 0) {
    duration = millis;
  }
  int num = duration/40;
  if (num > 255) {
    duration = 40*255;
  }
  if (num > 0) {
    timer.start(40);
  }
  else {
    setOpacity(1.0);
  }
  mutex.unlock();
}

//===========================================================================
// fadeout (1->0)
//===========================================================================
void ProgressWidget::fadeout(size_t millis)
{
  mutex.lock();

  if (timer.isActive()) timer.stop();

  if (!isVisible()) {
    mutex.unlock();
    return;
  }

  isin = false;

  if (millis > 0) {
    duration = millis;
  }
  int num = duration/40;
  if (num == 0) {
    setVisible(false);
    mutex.unlock();
    return;
  }
  if (num > 255) {
    duration = 40*255;
  }
  timer.start(40);
  mutex.unlock();
}

//===========================================================================
// changeOpacity
//===========================================================================
void ProgressWidget::changeOpacity()
{
  mutex.lock();
  opacity += (isin?+1:-1) * 40.0/(double)(duration);
  if (0.0 < opacity && opacity < 1.0) {
    setOpacity(opacity);
  }
  else if (opacity <= 0.0) {
    setOpacity(0.0);
    timer.stop();
    setVisible(false);
  }
  else {
    setOpacity(1.0);
    timer.stop();
  }
  mutex.unlock();
}

//===========================================================================
// setOpacity
//===========================================================================
void ProgressWidget::setOpacity(double val)
{
  opacity = val;
  QGraphicsOpacityEffect *effect = dynamic_cast<QGraphicsOpacityEffect*>(graphicsEffect());
  if (effect != NULL) {
    effect->setOpacity(val);
    update();
    ui->frame->update();
    ui->label->update();
    ui->progress->update();
  }
}

