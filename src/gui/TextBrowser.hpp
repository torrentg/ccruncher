#ifndef _TextBrowser_
#define _TextBrowser_

#include <QTextBrowser>

class TextBrowser : public QTextBrowser
{

  protected:

    // overrided method
    void setSource(const QUrl &) {}

  public:

    // constructor
    TextBrowser(QWidget *parent=0) : QTextBrowser(parent) {}

};

#endif

