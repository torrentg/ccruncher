#include <QFile>
#include <QTextStream>
#include "gui/XmlEditWidget.hpp"
#include "utils/Exception.hpp"
#include "ui_XmlEditWidget.h"
#include <cassert>

using namespace ccruncher;

//===========================================================================
// constructor
//===========================================================================
XmlEditWidget::XmlEditWidget(const QString &filename, QWidget *parent) :
    QWidget(parent), ui(new Ui::XmlEditWidget), highlighter(NULL)
{
  ui->setupUi(this);
  highlighter = new XmlHighlighter(ui->editor);
  QFile file(filename);
  if(!file.open(QFile::ReadOnly | QFile::Text)) {
    throw Exception("can't open file " + filename.toStdString());
  }
  QTextStream in(&file);
  ui->editor->setPlainText(in.readAll());
}

//===========================================================================
// destructor
//===========================================================================
XmlEditWidget::~XmlEditWidget()
{
  delete ui;
  if (highlighter != NULL) {
    delete highlighter;
  }
}

