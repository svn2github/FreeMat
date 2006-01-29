#ifndef __HIGHLIGHTER_HPP__
#define __HIGHLIGHTER_HPP__

#include <QSyntaxHighlighter>
#include <QHash>
#include <QTextCharFormat>

class QTextDocument;

class Highlighter : public QSyntaxHighlighter {
  Q_OBJECT

public:
  Highlighter(QTextDocument *parent = 0);
protected:
  void highlightBlock(const QString &text);
private:
  QHash<QString,QTextCharFormat> mappings;
  QTextCharFormat multiLineCommentFormat;
};

#endif
