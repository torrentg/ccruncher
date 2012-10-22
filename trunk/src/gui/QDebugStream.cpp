#include "QDebugStream.hpp"

//===========================================================================
// constructor
//===========================================================================
QDebugStream::QDebugStream(std::ostream &stream, QObject *parent) :
  QObject(parent), basic_streambuf<char>(), m_stream(stream)
{
  m_old_buf = stream.rdbuf();
  stream.rdbuf(this);
}

//===========================================================================
// destructor
//===========================================================================
QDebugStream::~QDebugStream()
{
  // output anything that is left
  if (!m_string.empty()) emit print(m_string.c_str());
  m_stream.rdbuf(m_old_buf);
}

//===========================================================================
// inherited from streambuf
//===========================================================================
int QDebugStream::overflow(int v)
{
/*
  m_string += v;
  emit print(m_string.c_str());
  m_string.erase(m_string.begin(), m_string.end());
*/
  if (v == '\n')
  {
    emit print(m_string.c_str());
    m_string.erase(m_string.begin(), m_string.end());
  }
  else
    m_string += v;

  return v;
}

//===========================================================================
// inherited from streambuf
//===========================================================================
std::streamsize QDebugStream::xsputn(const char *p, std::streamsize n)
{
  m_string.append(p, p + n);

  size_t pos = 0;
  while (pos != std::string::npos)
  {
    pos = m_string.find('\n');
    if (pos != std::string::npos)
    {
      std::string tmp(m_string.begin(), m_string.begin() + pos);
      emit print(tmp.c_str());
      m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
    }
  }

  return n;
}
