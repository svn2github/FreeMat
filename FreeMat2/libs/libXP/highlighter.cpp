/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include <QtGui>
#include "highlighter.hpp"

Highlighter::Highlighter(QTextDocument *parent)
  : QSyntaxHighlighter(parent)
{
  QSettings settings("FreeMat","FreeMat");

  keywordColor = settings.value("editor/syntax_colors/keyword",Qt::darkBlue).value<QColor>();
  commentColor = settings.value("editor/syntax_colors/comments",Qt::darkRed).value<QColor>();
  stringColor = settings.value("editor/syntax_colors/strings",Qt::darkGreen).value<QColor>();
  untermStringColor = settings.value("editor/syntax_colors/untermstrings",Qt::darkRed).value<QColor>();


  QTextCharFormat keywordFormat;
  keywordFormat.setForeground(keywordColor);
  keywordFormat.setFontWeight(QFont::Bold);
  QStringList keywordPatterns;
  keywordPatterns << "\\bbreak\\b" <<
    "\\bcase\\b" <<
    "\\bcatch\\b" <<
    "\\bcontinue\\b" <<
    "\\belse\\b" <<
    "\\belseif\\b" <<
    "\\bend\\b" <<
    "\\bfor\\b" <<
    "\\bfunction\\b" <<
    "\\bglobal\\b" <<
    "\\bif\\b" <<
    "\\bkeyboard\\b" <<
    "\\botherwise\\b" <<
    "\\bpersistent\\b" <<
    "\\bquit\\b" <<
    "\\bretall\\b" <<
    "\\breturn\\b" <<
    "\\bswitch\\b" <<
    "\\btry\\b" <<
    "\\bwhile\\b";
  foreach (QString pattern, keywordPatterns)
    mappings[pattern] = keywordFormat;
/*
  QTextCharFormat singleLineCommentFormat;
  singleLineCommentFormat.setForeground(commentColor);
  mappings["\\%[^\n]*"] = singleLineCommentFormat;
*/
  stringFormat.setForeground(stringColor);
  untermStringFormat.setForeground(untermStringColor);

  highlightingEnabled = settings.value("editor/syntax_enable",true).toBool();

  //  mappings["[^'\\]\\)\\}A-Za-z0-9]'[^']*'"] = stringFormat;
}

void Highlighter::highlightBlock(const QString &text)
{
  if (text.isEmpty() || (!highlightingEnabled)) return;
  foreach (QString pattern, mappings.keys()) {
    QRegExp expression(pattern);
    int index = text.indexOf(expression);
    while (index >= 0) {
      int length = expression.matchedLength();
      setFormat(index, length, mappings[pattern]);
      index = text.indexOf(expression, index + length);
    }
  }

  QRegExp sutest("[^'\\]\\)\\}A-Za-z0-9]'[^']*");
  QRegExp droptest("[\\[\\(\\{A-Za-z0-9]");
  int index = text.indexOf(sutest);
  while (index >=0) {
    int length = sutest.matchedLength();
    if (length>0) {
      QString first(text[index]); 
      int q = first.indexOf(droptest);
      if (q >= 0) {
	index++;
	length--;
      }
    }
    setFormat(index, length, untermStringFormat);
    index = text.indexOf(sutest, index + length);
  }


  QRegExp sttest("[^'\\]\\)\\}A-Za-z0-9]'[^']*'");
  index = text.indexOf(sttest);
  while (index >=0) {
    int length = sttest.matchedLength();
    if (length>0) {
      QString first(text[index]); 
      int q = first.indexOf(droptest);
      if (q >= 0) {
	index++;
	length--;
      }
    }
    setFormat(index, length, stringFormat);
    index = text.indexOf(sttest, index + length);
  }

  QTextCharFormat singleLineCommentFormat;
  singleLineCommentFormat.setForeground(commentColor);
  QRegExp comment("\\%[^\n|\\%]*");
  int index1 = text.indexOf(comment);
  QRegExp notcomment("[^'\\]\\)\\}A-Za-z0-9]'[^']*'");
  while (index1 >= 0) {
    int length1 = comment.matchedLength();
    bool isComment = true;
    int index2 = text.indexOf(notcomment);
    while (index2 >= 0) {
      int length2 = notcomment.matchedLength();
      if (index1 > index2 && index1 < index2+length2) {// '%' is inside a string
        isComment = false;
        break;
      }
      index2 = text.indexOf(notcomment, index2 + length2);
    }
    if (isComment)
      setFormat(index1, length1, singleLineCommentFormat);
    index1 = text.indexOf(comment, index1 + length1);
  }

}

