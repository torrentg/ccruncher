
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2025 Gerard Torrent
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

#pragma once

#include <QTimer>
#include <QWidget>
#include <QMutex>
#include "ui_ProgressWidget.h"

namespace ccruncher_gui {

/**************************************************************************//**
 * @brief Progress widget with fading.
 */
class ProgressWidget : public QWidget
{
    Q_OBJECT

  public:

    //! Interface widget
    Ui::ProgressWidget *ui;
    //! Fade timer
    QTimer timer;
    //! Fade alpha
    double opacity;
    //! Fade duration
    int duration;
    //! Fade direction
    bool isin;
    //! Mutex
    QMutex mutex;

  public:

    //! Constructor
    ProgressWidget(QWidget *parent = nullptr);
    //! Destructor
    virtual ~ProgressWidget() override;
    //! Fadein (0->1)
    void fadein(size_t millis=0);
    //! Fadeout (1->0)
    void fadeout(size_t millis=0);
    //! Set opacity
    void setOpacity(double);

  public slots:

    //! Change opacity
    void changeOpacity();

};

} // namespace
