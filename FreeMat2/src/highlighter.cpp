#include <QtGui>
#include "highlighter.hpp"

  Highlighter::Highlighter(QTextDocument *parent)
        : QSyntaxHighlighter(parent)
    {
        QTextCharFormat keywordFormat;
        keywordFormat.setForeground(Qt::darkBlue);
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

        QTextCharFormat singleLineCommentFormat;
        singleLineCommentFormat.setForeground(Qt::red);
        mappings["\\%[^\n]*"] = singleLineCommentFormat;
    }

    void Highlighter::highlightBlock(const QString &text)
    {
      if (text.isEmpty()) return;
        foreach (QString pattern, mappings.keys()) {
            QRegExp expression(pattern);
            int index = text.indexOf(expression);
            while (index >= 0) {
                int length = expression.matchedLength();
                setFormat(index, length, mappings[pattern]);
                index = text.indexOf(expression, index + length);
            }
        }
    }

