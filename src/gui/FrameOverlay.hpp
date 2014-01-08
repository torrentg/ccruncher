
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

#ifndef _FrameOverlay_
#define _FrameOverlay_

#include <vector>
#include <QFrame>
#include <QWidget>
#include <QResizeEvent>

namespace ccruncher_gui {

class FrameOverlay : public QFrame
{

  private:

    // layers
    std::vector<QWidget*> layers;

  protected:

    // resize event
    void resizeEvent(QResizeEvent *event)
    {
      for(size_t i=0; i<layers.size(); i++) {
        if (layers[i] != NULL) layers[i]->resize(event->size());
      }
      event->accept();
    }

  public:

    // constructor
    explicit FrameOverlay(QWidget *parent = 0) : QFrame(parent) {}
    // add a layer
    void addLayer(QWidget *w) { layers.push_back(w); }

};

}

#endif

