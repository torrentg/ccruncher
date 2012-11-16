#ifndef _FrameOverlay_
#define _FrameOverlay_

#include <QFrame>
#include <QResizeEvent>

using namespace std;

namespace Ui {
class FrameOverlay;
}

class FrameOverlay : public QFrame
{

  private:

    // layers
    vector<QWidget*> layers;

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

#endif

