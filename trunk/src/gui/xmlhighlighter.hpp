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
#ifndef XMLHIGHLIGHTER_H
#define XMLHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextEdit>
#include <QRegExp>

class XmlHighlighter : public QSyntaxHighlighter
{

  private:

    // parser state
    enum ParsingState
    {
      NoState = -1,
      ExpectElementNameOrSlash,
      ExpectElementName,
      ExpectAttributeOrEndOfElement,
      ExpectEqual,
      ExpectAttributeValue,
      InComment,
      InCdata,
      InElement
    };

  private:

    // formatters
    QTextCharFormat fmtSyntaxChar;
    QTextCharFormat fmtElementName;
    QTextCharFormat fmtComment;
    QTextCharFormat fmtAttributeName;
    QTextCharFormat fmtAttributeValue;
    QTextCharFormat fmtError;
    QTextCharFormat fmtOther;
    // regular expressions
    QRegExp regexp_comment;
    QRegExp regexp_comment_begin;
    QRegExp regexp_comment_end;
    QRegExp regexp_cdata;
    QRegExp regexp_cdata_begin;
    QRegExp regexp_cdata_end;
    QRegExp regexp_attrib;
    QRegExp regexp_name;

  private:

    // initialization
    void init();

  protected:

    // overrided method
    virtual void highlightBlock(const QString& rstrText);
    // overrided method
    int processDefaultText(int i, const QString& rstrText, ParsingState &state);

  public:

    // constructor
    XmlHighlighter(QObject* parent);
    // constructor
    XmlHighlighter(QTextDocument* parent);
    // constructor
    XmlHighlighter(QTextEdit* parent);

};

#endif

