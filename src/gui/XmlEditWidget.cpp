
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
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

#include <cassert>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QMessageBox>
#include "gui/XmlEditWidget.hpp"
#include "utils/Exception.hpp"
#include "ui_XmlEditWidget.h"

using namespace ccruncher;

/**************************************************************************//**
 * @param[in] f Input filename.
 * @param[in] parent Widget parent.
 */
ccruncher_gui::XmlEditWidget::XmlEditWidget(const QString &f, QWidget *parent) :
    MdiChildWidget(parent), ui(new Ui::XmlEditWidget), highlighter(nullptr),
    toolbar(nullptr)
{
  ui->setupUi(this);

  // save action
  QKeySequence keys_save(QKeySequence::Save);
  actionSave = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
  actionSave->setStatusTip(tr("Save changes"));
  actionSave->setShortcut(keys_save);
  QObject::connect(actionSave, SIGNAL(triggered()), this, SLOT(save()));
  this->addAction(actionSave);

  // undo action
  actionUndo = new QAction(QIcon(":/images/undo.png"), tr("&Undo"), this);
  actionUndo->setStatusTip(tr("Undo changes"));
  connect(actionUndo, SIGNAL(triggered()), ui->editor, SLOT(undo()));

  // reload action
  actionReload = new QAction(QIcon(":/images/refresh.png"), tr("&Reload"), this);
  actionReload->setStatusTip(tr("Reload file"));
  connect(actionReload, SIGNAL(triggered()), this, SLOT(load()));

  // run action
  actionRun = new QAction(QIcon(":/images/exec.png"), tr("&Monte Carlo"), this);
  actionRun->setStatusTip(tr("Monte Carlo dialog"));
  connect(actionRun, SIGNAL(triggered()), this, SLOT(runFile()));

  // creating toolbar
  toolbar = new QToolBar(tr("Editor"), this);
  toolbar->addAction(actionRun);
  toolbar->addSeparator();
  toolbar->addAction(actionReload);
  toolbar->addAction(actionUndo);
  toolbar->addAction(actionSave);

  // creating highlighter
  highlighter = new XmlHighlighter(ui->editor);
  highlighter->setDocument(ui->editor->document());

  try {
    load(f);
  }
  catch(...) {
    if (toolbar != nullptr) delete toolbar;
    if (highlighter != nullptr) delete highlighter;
    throw;
  }
}

/**************************************************************************/
ccruncher_gui::XmlEditWidget::~XmlEditWidget()
{
  if (toolbar != nullptr) delete toolbar;
  if (highlighter != nullptr) delete highlighter;
  delete ui;
}

/**************************************************************************/
void ccruncher_gui::XmlEditWidget::documentWasModified()
{
  setWindowModified(ui->editor->document()->isModified());
  actionSave->setEnabled(ui->editor->document()->isModified());
  actionUndo->setEnabled(ui->editor->document()->availableUndoSteps()>0);
}

/**************************************************************************//**
 * @details Update info related to filename.
 * @param[in] fileName Input filename.
 */
void ccruncher_gui::XmlEditWidget::setCurrentFile(const QString &fileName)
{
  filename = QFileInfo(fileName).canonicalFilePath();
  ui->editor->document()->setModified(false);
  setWindowModified(false);
  actionSave->setEnabled(false);
  actionUndo->setEnabled(ui->editor->document()->availableUndoSteps()>0);
  QString name = QFileInfo(filename).fileName();
  setWindowTitle(name + " [*]");
}

/**************************************************************************//**
 * @param[in] str Input filename.
 * @return true = file loaded, false = otherwise.
 * @throw NonEditableException File too big or gzipped.
 * @throw InvalidFormatException It is not a CCruncher input file.
 * @throw OpenErrorException File not found.
 */
bool ccruncher_gui::XmlEditWidget::load(const QString &str)
{
  QString fileName = str;
  if (fileName.isEmpty()) {
    fileName = filename;
  }

  // check if file is compressed
  if (fileName.endsWith(".gz"))
  {
    QMessageBox::warning(this, tr("CCruncher"),
        tr("Edition of compressed files is not supported."));
    throw NonEditableException();
  }

  // save current content
  QCloseEvent event;
  closeEvent(&event);
  if (!event.isAccepted()) return false;

  QFile file(fileName);

  // opening file
  if (!file.open(QFile::ReadOnly | QFile::Text))
  {
    QMessageBox::warning(this, tr("CCruncher"),
        tr("Cannot open file '%1':\n%2.")
        .arg(fileName)
        .arg(file.errorString()));
    throw OpenErrorException();
  }

  // reject file if bigger than 1 MB
  if (file.size() > 1*1024*1024)
  {
    QMessageBox::warning(this, tr("CCruncher"),
        tr("File '%1' too big to be edited.\nTry to open it using an external editor.")
        .arg(fileName));
    throw NonEditableException();
  }

  QApplication::setOverrideCursor(Qt::WaitCursor);
  QTextStream in(&file);
  in.setCodec("UTF-8");
  QString content = in.readAll();

  QString codec = getEncoding(content);
  if (codec.toUpper() != "UTF-8") {
    in.seek(0);
    in.setCodec(codec.toStdString().c_str());
    content = in.readAll();
  }

  // reject non-ccruncher input files
  if (!content.contains("<ccruncher") && !content.contains("<portfolio"))
  {
    QApplication::restoreOverrideCursor();
    QMessageBox::warning(this, tr("CCruncher"),
        tr("File '%1'\nis not a valid input file.")
        .arg(fileName));
    throw InvalidFormatException();
  }

  ui->editor->setPlainText(content);
  QApplication::restoreOverrideCursor();

  setCurrentFile(fileName);
  connect(ui->editor->document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));
  return true;
}

/**************************************************************************//**
 * @details Save current content to file.
 * @param[in] str Filename.
 */
bool ccruncher_gui::XmlEditWidget::save(const QString &str)
{
  QString fileName = str;
  if (fileName.isEmpty()) {
    fileName = filename;
  }

  QFile file(fileName);
  if (!file.open(QFile::WriteOnly | QFile::Text))
  {
     QMessageBox::warning(this, tr("CCruncher"),
                          tr("Cannot save file '%1':\n%2.")
                          .arg(fileName)
                          .arg(file.errorString()));
     return false;
  }

  QApplication::setOverrideCursor(Qt::WaitCursor);
  QString txt = ui->editor->toPlainText();
  QString codec = getEncoding(txt);
  QTextStream out(&file);
  out.setCodec(codec.toStdString().c_str());
  out << txt;
  QApplication::restoreOverrideCursor();
  setWindowModified(false);
  ui->editor->document()->setModified(false);
  actionSave->setEnabled(false);
  actionUndo->setEnabled(ui->editor->document()->availableUndoSteps()>0);
  return true;
}

/**************************************************************************//**
 * @param[in] event Event info.
 */
void ccruncher_gui::XmlEditWidget::closeEvent(QCloseEvent *event)
{
  if (ui->editor->document()->isModified())
  {
    QString name = QFileInfo(filename).fileName();
    QMessageBox::StandardButton rc = QMessageBox::warning(this, tr("CCruncher"),
            tr("'%1' has been modified.\nDo you want to save your changes?")
            .arg(name), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (rc == QMessageBox::Save) {
      if (save()) event->accept();
      else event->ignore();
    }
    else if (rc == QMessageBox::Cancel) {
      event->ignore();
    }
    else if (rc == QMessageBox::Discard) {
      event->accept();
    }
    else {
      event->ignore();
    }
  }
  else
  {
    event->accept();
  }
}

/**************************************************************************//**
 * @details Switch to Execute dialog.
 */
void ccruncher_gui::XmlEditWidget::runFile()
{
  QUrl url = QUrl::fromLocalFile(filename);
  url.setPath(url.toLocalFile());
  url.setScheme("exec");
  emit anchorClicked(url);
}

/**************************************************************************//**
 * @details Returns encoding set in the XML.
 * @param[in] txt XML content.
 * @return XML encoding (default = UTF-8).
 */
QString ccruncher_gui::XmlEditWidget::getEncoding(const QString &txt) const
{
  QRegExp regexp("encoding\\s*=\\s*[\"']([^\"']+)[\"']");
  QString codec = "UTF-8";
  if (regexp.indexIn(txt) >= 0) {
    codec = regexp.cap(1);
  }
  return codec;
}

