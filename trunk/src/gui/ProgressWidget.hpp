
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2014 Gerard Torrent
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

#ifndef _ProgressWidget_
#define _ProgressWidget_

#include <QTimer>
#include <QWidget>
#include <QMutex>
#include "ui_ProgressWidget.h"

namespace ccruncher_gui {

class ProgressWidget : public QWidget
{
    Q_OBJECT

  public:

    // interface widget
    Ui::ProgressWidget *ui;
    // fade timer
    QTimer timer;
    // fade alpha
    double opacity;
    // fade duration
    int duration;
    // fade direction
    bool isin;
    // mutex
    QMutex mutex;

  public:

    // constructor
    explicit ProgressWidget(QWidget *parent = 0);
    // destructor
    ~ProgressWidget();
    // fadein (0->1)
    void fadein(size_t millis=0);
    // fadeout (1->0)
    void fadeout(size_t millis=0);
    // set opacity
    void setOpacity(double);

  public slots:

    // change opacity
    void changeOpacity();

};

}

#endif
