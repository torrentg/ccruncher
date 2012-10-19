#ifndef _QDebugStream_
#define _QDebugStream_

#include <iostream>
#include <streambuf>
#include <string>
#include <QPlainTextEdit>

//---------------------------------------------------------------------------

/*
  class to redirect std:ccout and std::cerr to a TextEdit widget
  based on http://lists.trolltech.com/qt-interest/2005-06/thread00166-0.html
*/

class QDebugStream : public std::basic_streambuf<char>
{

  private:

    std::ostream &m_stream;
    std::streambuf *m_old_buf;
    std::string m_string;
    QPlainTextEdit* log_window;

  public:

    // constructor
    QDebugStream(std::ostream &stream, QPlainTextEdit* text_edit);
    // destructor
    ~QDebugStream();

  protected:

    // inherited from streambuf
    virtual int overflow(int v);
    // inherited from streambuf
    virtual std::streamsize xsputn(const char *p, std::streamsize n);

};

#endif

