#ifndef _MdiChildWidget_
#define _MdiChildWidget_

#include <QWidget>
#include <QToolBar>
#include <QMainWindow>

class MdiChildWidget: public QWidget
{

  public:

    // constructor
    MdiChildWidget(QWidget *parent) : QWidget(parent, Qt::SubWindow) {}
    // destructor
/*    virtual ~MdiChildWidget() {
      QMainWindow *main = dynamic_cast<QMainWindow*>(parent);
      if (main != NULL && getToolBar() != NULL) {
        main->removeToolBar(getToolBar());
      }
    }*/
    // widget toolbar
    virtual QToolBar* getToolBar() { return NULL; }

};

#endif

