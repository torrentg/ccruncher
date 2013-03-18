
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

namespace ccruncher_gui {

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
    // returns xml encoding
    QString getEncoding(const QString &txt) const;

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

}

#endif

