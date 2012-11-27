#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QMessageBox>
#include "gui/XmlEditWidget.hpp"
#include "utils/Exception.hpp"
#include "ui_XmlEditWidget.h"
#include <cassert>

using namespace ccruncher;

//===========================================================================
// constructor
//===========================================================================
XmlEditWidget::XmlEditWidget(const QString &f, QWidget *parent) :
    MdiChildWidget(parent), ui(new Ui::XmlEditWidget), highlighter(NULL),
    toolbar(NULL)
{
  ui->setupUi(this);
  highlighter = new XmlHighlighter(ui->editor);

  if(!load(f)) {
    throw Exception("cannot read file " + filename.toStdString());
  }

  // save action
  QKeySequence keys_save(QKeySequence::Save);
  QAction* actionSave = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
  actionSave->setStatusTip(tr("Save changes"));
  actionSave->setShortcut(keys_save);
  QObject::connect(actionSave, SIGNAL(triggered()), this, SLOT(save()));
  this->addAction(actionSave);

  // undo action
  QAction *actionUndo = new QAction(QIcon(":/images/undo.png"), tr("&Undo"), this);
  actionUndo->setStatusTip(tr("Undo changes"));
  connect(actionUndo, SIGNAL(triggered()), ui->editor, SLOT(undo()));

  // reload action
  QAction *actionReload = new QAction(QIcon(":/images/refresh.png"), tr("&Reload"), this);
  actionReload->setStatusTip(tr("Reload file"));
  connect(actionReload, SIGNAL(triggered()), this, SLOT(load()));

  // run action
  QAction *actionRun = new QAction(QIcon(":/images/gear.png"), tr("&Monte Carlo"), this);
  actionRun->setStatusTip(tr("Monte Carlo dialog"));
  connect(actionRun, SIGNAL(triggered()), this, SLOT(runFile()));

  // creating toolbar
  toolbar = new QToolBar(tr("Editor"), this);
  toolbar->addAction(actionRun);
  toolbar->addAction(actionReload);
  toolbar->addAction(actionUndo);
  toolbar->addAction(actionSave);
}

//===========================================================================
// destructor
//===========================================================================
XmlEditWidget::~XmlEditWidget()
{
  if (toolbar != NULL) delete toolbar;
  if (highlighter != NULL) delete highlighter;
  delete ui;
}

//===========================================================================
// documentWasModified
//===========================================================================
void XmlEditWidget::documentWasModified()
{
  setWindowModified(ui->editor->document()->isModified());
}

//===========================================================================
// setCurrentFile
//===========================================================================
void XmlEditWidget::setCurrentFile(const QString &fileName)
{
  filename = QFileInfo(fileName).canonicalFilePath();
  ui->editor->document()->setModified(false);
  setWindowModified(false);
  QString name = QFileInfo(filename).fileName();
  setWindowTitle(name + " [*]");
}

//===========================================================================
// load file
//===========================================================================
bool XmlEditWidget::load(const QString &str)
{
  QString fileName = str;
  if (fileName.isEmpty()) {
    fileName = filename;
  }

  QCloseEvent event;
  closeEvent(&event);
  if (!event.isAccepted()) return false;

  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text))
  {
    QMessageBox::warning(this, tr("CCruncher"),
                         tr("Cannot read file %1:\n%2.")
                         .arg(fileName)
                         .arg(file.errorString()));
    return false;
  }

  if (file.size() > 1*1024*1024) // reject file if bigger than 1 MB
  {
     QMessageBox::warning(this, tr("CCruncher"),
                         tr("File %1 too big.\nTry to open it using an external editor.")
                         .arg(fileName));
     return false;
  }

  //TODO: check that is a text file

  QTextStream in(&file);
  QApplication::setOverrideCursor(Qt::WaitCursor);
  ui->editor->setPlainText(in.readAll());
  QApplication::restoreOverrideCursor();

  setCurrentFile(fileName);
  connect(ui->editor->document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));
  return true;
}

//===========================================================================
// save current content to file
//===========================================================================
bool XmlEditWidget::save(const QString &str)
{
  QString fileName = str;
  if (fileName.isEmpty()) {
    fileName = filename;
  }

  QFile file(fileName);
  if (!file.open(QFile::WriteOnly | QFile::Text))
  {
     QMessageBox::warning(this, tr("CCruncher"),
                          tr("Cannot save file %1:\n%2.")
                          .arg(fileName)
                          .arg(file.errorString()));
     return false;
  }

  QTextStream out(&file);
  QApplication::setOverrideCursor(Qt::WaitCursor);
  out << ui->editor->toPlainText();
  QApplication::restoreOverrideCursor();
  setWindowModified(false);
  ui->editor->document()->setModified(false);
  return true;
}

//===========================================================================
// close
//===========================================================================
void XmlEditWidget::closeEvent(QCloseEvent *event)
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

//===========================================================================
// run current file
//===========================================================================
void XmlEditWidget::runFile()
{
  QUrl url = QUrl::fromLocalFile(filename);
  url.setPath(url.toLocalFile());
  url.setScheme("exec");
  emit anchorClicked(url);
}

