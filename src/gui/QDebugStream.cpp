#include "QDebugStream.hpp"

//===========================================================================
// constructor
//===========================================================================
QDebugStream::QDebugStream(std::ostream &stream, QObject *parent) :
  QObject(parent), std::basic_streambuf<char>(), m_stream(stream)
{
  m_old_buf = stream.rdbuf();
  stream.rdbuf(this);
}

//===========================================================================
// destructor
//===========================================================================
QDebugStream::~QDebugStream()
{
  m_stream.rdbuf(m_old_buf);
}

//===========================================================================
// inherited from streambuf
//===========================================================================
int QDebugStream::overflow(int v)
{
  emit print(QString(QChar(v)));
  return v;
}

//===========================================================================
// inherited from streambuf
//===========================================================================
std::streamsize QDebugStream::xsputn(const char *p, std::streamsize n)
{
  emit print(QString::fromUtf8(p,n));
  return n;
}
