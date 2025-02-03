
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

#include <qwt_plot_canvas.h>
#include <QMouseEvent>

/**************************************************************************//**
 * @brief Artifact to track mouse on a Qwt plot.
 */
class QwtPlotCanvasExt : public QwtPlotCanvas
{
  Q_OBJECT

  public:

    //! Constructor
    QwtPlotCanvasExt(QwtPlot *p=nullptr) : QwtPlotCanvas(p) {
      setMouseTracking(true);
    }
    //! Destructor
    virtual ~QwtPlotCanvasExt() override {}
    //! Mouse moved
    virtual void mouseMoveEvent(QMouseEvent* event) override {
      emit mouseMoved(event->pos());
    }

  signals:

    //! Status updated
    void mouseMoved(QPoint);

};
