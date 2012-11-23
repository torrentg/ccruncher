#ifndef _XmlEditWidget_
#define _XmlEditWidget_

#include <QWidget>
#include <QString>
#include "gui/xmlhighlighter.hpp"

namespace Ui {
class XmlEditWidget;
}

class XmlEditWidget : public QWidget
{
    Q_OBJECT

  private:

    // interface
    Ui::XmlEditWidget *ui;
    // highlisghter
    XmlHighlighter *highlighter;

  public:

    // constructor
    explicit XmlEditWidget(const QString &filename, QWidget *parent=0);
    // destructor
    ~XmlEditWidget();
    
};

#endif

