#include "QStreamBuf.hpp"

//===========================================================================
// constructor
//===========================================================================
QStreamBuf::QStreamBuf(QObject *parent) : QObject(parent), std::basic_streambuf<char>()
{
  // nothing to do
}

//===========================================================================
// inherited from streambuf
//===========================================================================
int QStreamBuf::overflow(int v)
{
  emit print(QString(QChar(v)));
  return v;
}

//===========================================================================
// inherited from streambuf
//===========================================================================
std::streamsize QStreamBuf::xsputn(const char *p, std::streamsize n)
{
  emit print(QString::fromUtf8(p,n));
  return n;
}

