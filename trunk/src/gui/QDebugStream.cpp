#include "QDebugStream.hpp"

//===========================================================================
// constructor
//===========================================================================
QDebugStream::QDebugStream(std::ostream &stream, QPlainTextEdit* text_edit) : m_stream(stream)
{
  log_window = text_edit;
  m_old_buf = stream.rdbuf();
  stream.rdbuf(this);
}

//===========================================================================
// destructor
//===========================================================================
QDebugStream::~QDebugStream()
{
  // output anything that is left
  if (!m_string.empty())
  log_window->appendPlainText(m_string.c_str());
  m_stream.rdbuf(m_old_buf);
}

//===========================================================================
// inherited from streambuf
//===========================================================================
int QDebugStream::overflow(int v)
{
  if (v == '\n')
  {
    log_window->appendPlainText(m_string.c_str());
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
      log_window->appendPlainText(tmp.c_str());
      m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
    }
  }

  return n;
}
