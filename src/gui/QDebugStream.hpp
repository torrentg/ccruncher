#ifndef _QDebugStream_
#define _QDebugStream_

#include <iostream>
#include <streambuf>
#include <string>
#include <QObject>
#include <QString>

//---------------------------------------------------------------------------

/*
  class to redirect std:ccout and std::cerr to a TextEdit widget
  based on http://lists.trolltech.com/qt-interest/2005-06/thread00166-0.html
  caution: it is not performance optimal
  causes: string usage + string by value as argument
*/

class QDebugStream : public QObject, public std::basic_streambuf<char>
{
    Q_OBJECT

  private:

    std::ostream &m_stream;
    std::streambuf *m_old_buf;

  public:

    // constructor
    QDebugStream(std::ostream &stream, QObject *parent=NULL);
    // destructor
    ~QDebugStream();

  protected:

    // inherited from streambuf
    virtual int overflow(int v);
    // inherited from streambuf
    virtual std::streamsize xsputn(const char *p, std::streamsize n);

  signals:

    // print
    void print(const QString);

};

#endif

