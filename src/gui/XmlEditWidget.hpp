
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

/**************************************************************************//**
 * @brief XML editor widget.
 */
class XmlEditWidget : public MdiChildWidget
{
    Q_OBJECT

  private:

    //! Interface
    Ui::XmlEditWidget *ui;
    //! Highlighter
    XmlHighlighter *highlighter;
    //! Filename
    QString filename;
    //! Toolbar
    QToolBar *toolbar;
    //! Action save
    QAction* actionSave;
    //! Action undo
    QAction *actionUndo;
    //! Action reload
    QAction *actionReload;
    //! Action run
    QAction *actionRun;

  private:

    //! Set filename
    void setCurrentFile(const QString &fileName);
    //! Returns xml encoding
    QString getEncoding(const QString &txt) const;

  protected:

    //! Close event
    void closeEvent(QCloseEvent *event);

  public:

    //! File non-editable (eg. gziped or size > 1MB)
    class NonEditableException : public std::exception { };
    //! File non-valid (eg. file.doc)
    class InvalidFormatException : public std::exception { };
    //! File not found or non-readable
    class OpenErrorException : public std::exception { };

  public:

    //! Constructor
    explicit XmlEditWidget(const QString &filename, QWidget *parent=0)
        throw(NonEditableException, InvalidFormatException, OpenErrorException);
    //! Destructor
    ~XmlEditWidget();
    //! Virtual method implementation
    QToolBar* getToolBar() { return toolbar; }

  public slots:

    //! Set modified flag
    void documentWasModified();
    //! Load file
    bool load(const QString &fileName=QString())
        throw(NonEditableException, InvalidFormatException, OpenErrorException);
    //! Save file
    bool save(const QString &fileName=QString());
    //! Simule current file
    void runFile();

  signals:

    //! Anchor clicked
    void anchorClicked(const QUrl &);

};

} // namespace

#endif

