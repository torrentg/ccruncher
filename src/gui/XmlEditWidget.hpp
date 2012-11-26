#ifndef _XmlEditWidget_
#define _XmlEditWidget_

#include <QWidget>
#include <QString>
#include <QCloseEvent>
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
    // filename
    QString filename;

  private:

    // set filename
    void setCurrentFile(const QString &fileName);

  protected:

    // close event
    void closeEvent(QCloseEvent *event);

  public:

    // constructor
    explicit XmlEditWidget(const QString &filename, QWidget *parent=0);
    // destructor
    ~XmlEditWidget();
    // load file
    bool load(const QString &fileName);

  private slots:

    // set modified flag
    void documentWasModified();
    // save file
    bool save(const QString &fileName=QString());

};

#endif

