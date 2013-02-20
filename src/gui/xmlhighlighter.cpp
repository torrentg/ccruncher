/****************************************************************************
**
** Copyright (C) 2006 J-P Nurmi. All rights reserved.
**
** The used XML syntax highlighting principles have been adapted from 
** KXESyntaxHighlighter, which is part of KXML Editor 1.1.4,
** (C) 2003 by The KXMLEditor Team (http://kxmleditor.sourceforge.net).
** 
** This file may be used under the terms of the GPL Version 2, June 1991.
** For details, see http://www.gnu.org/licenses/gpl.html
** 
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#include <QColor>
#include "xmlhighlighter.hpp"

//TODO: colorize errors (errorFormat=background red, review cases)

static const QColor DEFAULT_SYNTAX_CHAR     = Qt::blue;
static const QColor DEFAULT_ELEMENT_NAME    = Qt::darkRed;
static const QColor DEFAULT_COMMENT         = Qt::darkGray; //darkGreen;
static const QColor DEFAULT_ATTRIBUTE_NAME  = Qt::darkMagenta;
static const QColor DEFAULT_ATTRIBUTE_VALUE = Qt::black;
static const QColor DEFAULT_ERROR           = Qt::red;
static const QColor DEFAULT_OTHER           = Qt::black;

// Regular expressions for parsing XML borrowed from:
// http://www.cs.sfu.ca/~cameron/REX.html
static const QString EXPR_COMMENT           = "<!--[^-]*-([^-][^-]*-)*->";
static const QString EXPR_COMMENT_BEGIN     = "<!--";
static const QString EXPR_COMMENT_END       = "-->";
static const QString EXPR_CDATA             = "<!\\[CDATA\\[.*\\]\\]>";
static const QString EXPR_CDATA_BEGIN       = "<!\\[CDATA\\[";
static const QString EXPR_CDATA_END         = "\\]\\]>";
static const QString EXPR_ATTRIBUTE_VALUE   = "\"[^<\"]*\"|'[^<']*'";
static const QString EXPR_NAME              = "([A-Za-z_:]|[^\\x00-\\x7F])([A-Za-z0-9_:.-]|[^\\x00-\\x7F])*";

//===========================================================================
// constructors
//===========================================================================
XmlHighlighter::XmlHighlighter(QObject* parent)
: QSyntaxHighlighter(parent)
{
  init();
}

XmlHighlighter::XmlHighlighter(QTextDocument* parent)
: QSyntaxHighlighter(parent)
{
  init();
}

XmlHighlighter::XmlHighlighter(QTextEdit* parent)
: QSyntaxHighlighter(parent)
{
  init();
}

//===========================================================================
// init
//===========================================================================
void XmlHighlighter::init()
{
  fmtSyntaxChar.setForeground(DEFAULT_SYNTAX_CHAR);
  fmtElementName.setForeground(DEFAULT_ELEMENT_NAME);
  fmtComment.setForeground(DEFAULT_COMMENT);
  fmtAttributeName.setForeground(DEFAULT_ATTRIBUTE_NAME);
  fmtAttributeValue.setForeground(DEFAULT_ATTRIBUTE_VALUE);
  fmtError.setForeground(DEFAULT_ERROR);
  fmtOther.setForeground(DEFAULT_OTHER);

  regexp_comment = QRegExp(EXPR_COMMENT);
  regexp_comment_begin = QRegExp(EXPR_COMMENT_BEGIN);
  regexp_comment_end = QRegExp(EXPR_COMMENT_END);
  regexp_cdata = QRegExp(EXPR_CDATA);
  regexp_cdata_begin = QRegExp(EXPR_CDATA_BEGIN);
  regexp_cdata_end = QRegExp(EXPR_CDATA_END);
  regexp_attrib = QRegExp(EXPR_ATTRIBUTE_VALUE);
  regexp_name = QRegExp(EXPR_NAME);
}

//===========================================================================
// highlightBlock
//===========================================================================
void XmlHighlighter::highlightBlock(const QString& text)
{
  int i = 0;
  int pos = 0;
  int iLength = 0;
  int brackets = 0;
  ParsingState state = NoState;

  if (previousBlockState() == InComment)
  {
    if ((pos=regexp_comment_end.indexIn(text,i)) >= 0)
    {
      setFormat(i, pos-i, fmtComment);
      setFormat(pos, 3, fmtSyntaxChar);
      i = pos+3;
      setCurrentBlockState(InElement);
      state = NoState; // setting default state
    }
    else
    {
      setFormat(i, text.length(), fmtComment);
      setCurrentBlockState(InComment);
      return;
    }
  }
  else if (previousBlockState() == InCdata)
  {
    if ((pos=regexp_cdata_end.indexIn(text,i)) >= 0)
    {
      setFormat(i, pos-i, fmtOther);
      setFormat(pos, 3, fmtSyntaxChar);
      i = pos+3;
      setCurrentBlockState(InElement);
      state = NoState;
    }
    else
    {
      setFormat(i, text.length(), fmtOther);
      setCurrentBlockState(InCdata);
      return;
    }
  }
  else
  {
    state = (ParsingState) previousBlockState();
  }

  for (; i < text.length(); i++)
  {
    switch (text.at(i).toAscii())
    {
      case '<':
        brackets++;
        if (brackets == 1) {
          setFormat(i, 1, fmtSyntaxChar);
          state = ExpectElementNameOrSlash;
        }
        else {
          setFormat(i, 1, fmtError);
        }
        break;

      case '>':
        brackets--;
        if (brackets == 0) {
          setFormat(i, 1, fmtSyntaxChar);
        }
        else {
          setFormat(i, 1, fmtError);
        }
        state = NoState;
        break;

      case '/':
        if (state == ExpectElementNameOrSlash)
        {
          state = ExpectElementName;
          setFormat(i, 1, fmtSyntaxChar);
        }
        else if (state == ExpectAttributeOrEndOfElement) {
          setFormat(i, 1, fmtSyntaxChar);
        }
        else {
          setFormat(i, 1, fmtOther);
        }
        break;

      case '=':
        if (state == ExpectEqual)
        {
          state = ExpectAttributeValue;
          setFormat(i, 1, fmtOther);
        }
        else
        {
          setFormat(i, 1, fmtOther);
        }
        break;

      case '\'':
      case '\"':
        if (state == ExpectAttributeValue)
        {
          if ((pos=regexp_attrib.indexIn(text,i)) == i)
          {
            iLength = regexp_attrib.matchedLength();
            setFormat(i, 1, fmtOther);
            setFormat(i + 1, iLength - 2, fmtAttributeValue);
            setFormat(i + iLength - 1, 1, fmtOther);
            i += iLength - 1;
            state = ExpectAttributeOrEndOfElement;
          }
          else
          {
            setFormat(i, 1, fmtOther);
          }
        }
        else
        {
          setFormat(i, 1, fmtOther);
        }
        break;

      case '!':
        if (state == ExpectElementNameOrSlash)
        {
          // search 1-line comment
          if ((pos=regexp_comment.indexIn(text,i-1)) == i-1)
          {
            iLength = regexp_comment.matchedLength();
            setFormat(pos, 4, fmtSyntaxChar);
            setFormat(pos + 4, iLength - 7, fmtComment);
            setFormat(pos + iLength - 3, 3, fmtSyntaxChar);
            i += iLength - 2;
            state = NoState;
            setCurrentBlockState(InElement);
            brackets--;
          }
          // search multi-line comment
          else if ((pos=regexp_comment_begin.indexIn(text,i-1)) == i-1)
          {
            setFormat(pos, 4, fmtSyntaxChar);
            setFormat(pos + 4, text.length() - pos - 4, fmtComment);
            setCurrentBlockState(InComment);
            return;
          }
          // search 1-line cdata
          else if ((pos=regexp_cdata.indexIn(text,i-1)) == i-1)
          {
            iLength = regexp_cdata.matchedLength();
            setFormat(pos, 9, fmtSyntaxChar);
            setFormat(pos + 9, iLength - 12, fmtOther);
            setFormat(pos + iLength - 3, 3, fmtSyntaxChar);
            i += iLength - 2;
            state = NoState;
            setCurrentBlockState(InElement);
            brackets--;
          }
          // search multi-line cdata
          else if ((pos=regexp_cdata_begin.indexIn(text,i-1)) == i-1)
          {
            setFormat(pos, 9, fmtSyntaxChar);
            setFormat(pos + 9, text.length() - pos - 9, fmtOther);
            setCurrentBlockState(InCdata);
            return;
          }
          else
          {
            setFormat(i, 1, fmtOther);
          }
        }
        else
        {
          setFormat(i, 1, fmtOther);
        }
        break;

      default:
      {
        iLength = processDefaultText(i, text, state);
        if (iLength > 0) i += iLength - 1;
        break;
      }
    }
  }

  setCurrentBlockState(state);
}

//===========================================================================
// processDefaultText
//===========================================================================
int XmlHighlighter::processDefaultText(int i, const QString& text, ParsingState &state)
{
  int pos = 0;
  int iLength = 0;

  switch(state)
  {
    case ExpectElementNameOrSlash:
    case ExpectElementName:
    {
      if ((pos=regexp_name.indexIn(text,i)) == i)
      {
        iLength = regexp_name.matchedLength();
        setFormat(pos, iLength, fmtElementName);
        state = ExpectAttributeOrEndOfElement;
      }
      else
      {
        setFormat(i, 1, fmtOther);
      }
      break;
    }

    case ExpectAttributeOrEndOfElement:
    {
      if ((pos=regexp_name.indexIn(text,i)) == i)
      {
        iLength = regexp_name.matchedLength();
        setFormat(pos, iLength, fmtAttributeName);
        state = ExpectEqual;
      }
      else
      {
        setFormat(i, 1, fmtOther);
      }
      break;
    }

    default:
      setFormat(i, 1, fmtOther);
      break;
  }

  return iLength;
}  

