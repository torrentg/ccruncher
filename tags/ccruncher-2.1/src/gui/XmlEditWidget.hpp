#ifndef _XmlEditWidget_
#define _XmlEditWidget_

#include <QWidget>
#include <QString>
#include <QCloseEvent>
#include <QToolBar>
#include <QUrl>
#include <QAction>
#include "gui/MdiChildWidget.hpp"
#include "gui/xmlhighlighter.hpp"

namespace Ui {
class XmlEditWidget;
}

class XmlEditWidget : public MdiChildWidget
{
    Q_OBJECT

  private:

    // interface
    Ui::XmlEditWidget *ui;
    // highlisghter
    XmlHighlighter *highlighter;
    // filename
    QString filename;
    // toolbar
    QToolBar *toolbar;
    // actions
    QAction* actionSave;
    QAction *actionUndo;
    QAction *actionReload;
    QAction *actionRun;

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
    // virtual method implementation
    QToolBar* getToolBar() { return toolbar; }

  public slots:

    // set modified flag
    void documentWasModified();
    // load file
    bool load(const QString &fileName=QString());
    // save file
    bool save(const QString &fileName=QString());
    // simule current file
    void runFile();

  signals:

    // anchor clicked
    void anchorClicked(const QUrl &);

};

#endif

