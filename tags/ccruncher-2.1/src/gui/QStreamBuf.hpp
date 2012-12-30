#ifndef _QStreamBuf_
#define _QStreamBuf_

#include <iostream>
#include <streambuf>
#include <string>
#include <QObject>
#include <QString>

//---------------------------------------------------------------------------

/*
  convert streambuf call to signals
  based on http://lists.trolltech.com/qt-interest/2005-06/thread00166-0.html
  caution: it is not performance optimal
  causes: string usage + string by value as argument
*/

class QStreamBuf : public QObject, public std::basic_streambuf<char>
{
    Q_OBJECT

  public:

    // constructor
    QStreamBuf(QObject *parent=NULL);

  protected:

    // inherited from streambuf
    virtual int overflow(int v);
    // inherited from streambuf
    virtual std::streamsize xsputn(const char *p, std::streamsize n);

  signals:

    // print
    void print(const QString &);

};

#endif

