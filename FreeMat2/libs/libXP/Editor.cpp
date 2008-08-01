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
#include "Editor.hpp"
#include "Interpreter.hpp"
#include "highlighter.hpp"
#include <QtGui>
#include "Scope.hpp"
#include "Array.hpp"
#include "Print.hpp"

FMFindDialog::FMFindDialog(QWidget *parent) : QDialog(parent) {
  ui.setupUi(this);
  connect(ui.btFind,SIGNAL(clicked()),this,SLOT(find()));
  connect(ui.btClose,SIGNAL(clicked()),this,SLOT(hide()));
  setWindowIcon(QIcon(QString::fromUtf8(":/images/freemat_small_mod_64.png")));
  setWindowTitle("Find - " + QString::fromStdString(Interpreter::getVersionString()));
  ui.btFind->setIcon(QIcon(QString::fromUtf8(":/images/find.png")));
  ui.btClose->setIcon(QIcon(QString::fromUtf8(":/images/close.png")));
}

void FMFindDialog::setFindText(QString text) {
  int index = ui.cmFindText->findText(text);
  // remove the same text if found to avoid duplicating text
  if (index >= 0)
    ui.cmFindText->removeItem(index);
  ui.cmFindText->insertItem(0, text);
  ui.cmFindText->setCurrentIndex(0);
}

void FMFindDialog::find() {
  emit doFind(ui.cmFindText->currentText(), ui.cbBackwards->checkState() == Qt::Checked,
 	      ui.cbSensitive->checkState() == Qt::Checked);
}

void FMFindDialog::found() {
  ui.lbStatus->setText("");
}

void FMFindDialog::notfound() {
  if (ui.cbBackwards->checkState())
    ui.lbStatus->setText("Search reached start of document");
  else
    ui.lbStatus->setText("Search reached end of document");
}

FMReplaceDialog::FMReplaceDialog(QWidget *parent) : QDialog(parent) {
  ui.setupUi(this);
  connect(ui.btFind,SIGNAL(clicked()),this,SLOT(find()));
  connect(ui.btClose,SIGNAL(clicked()),this,SLOT(hide()));
  connect(ui.btReplace,SIGNAL(clicked()),this,SLOT(replace()));
  connect(ui.btReplaceAll,SIGNAL(clicked()),this,SLOT(replaceAll()));
  setWindowIcon(QIcon(QString::fromUtf8(":/images/freemat_small_mod_64.png")));
  setWindowTitle("Find - " + QString::fromStdString(Interpreter::getVersionString()));
  ui.btFind->setIcon(QIcon(QString::fromUtf8(":/images/find.png")));
  ui.btClose->setIcon(QIcon(QString::fromUtf8(":/images/close.png")));
}

void FMReplaceDialog::setReplaceText(QString text) {
  int index = ui.cmFindText->findText(text);
  // remove the same text if found to avoid duplicating text
  if (index >= 0)
    ui.cmFindText->removeItem(index);
  ui.cmFindText->insertItem(0, text);
  ui.cmFindText->setCurrentIndex(0);
}

void FMReplaceDialog::find() {
  emit doFind(ui.cmFindText->currentText(), 
	      ui.cbBackwards->checkState() == Qt::Checked,
	      ui.cbSensitive->checkState() == Qt::Checked);
}

void FMReplaceDialog::replace() {
  emit doReplace(ui.cmFindText->currentText(),
		 ui.cmReplaceText->currentText(),
		 ui.cbBackwards->checkState() == Qt::Checked,
		 ui.cbSensitive->checkState() == Qt::Checked);
}

void FMReplaceDialog::replaceAll() {
  emit doReplaceAll(ui.cmFindText->currentText(),
		    ui.cmReplaceText->currentText(),
		    ui.cbBackwards->checkState() == Qt::Checked,
		    ui.cbSensitive->checkState() == Qt::Checked);
}

void FMReplaceDialog::found() {
  ui.lbStatus->setText("");
}

void FMReplaceDialog::notfound() {
  if (ui.cbBackwards->checkState())
    ui.lbStatus->setText("Search reached start of document");
  else
    ui.lbStatus->setText("Search reached end of document");
}

void FMReplaceDialog::showrepcount(int cnt) {
  QString p;
  if (cnt == 0)
    p = QString("Found no instances of the search text");
  else if (cnt == 1)
    p = QString("Replaced one occurance");
  else
    p = QString("Replaced %1 occurances").arg(cnt);
  ui.lbStatus->setText(p);
}

FMTextEdit::FMTextEdit() : QTextEdit() {
  QSettings settings("FreeMat","FreeMat");
  indentSize = settings.value("editor/tab_size",4).toInt();
  indentActive = settings.value("editor/indent_enable",true).toBool();
  matchActive = settings.value("editor/match_enable",true).toBool();
  matchingColor = settings.value("editor/matching_color",Qt::black).value<QColor>();
  matchingBegin = -1;
  matchingEnd = -1;
  setLineWrapMode(QTextEdit::NoWrap);
  connect( this, SIGNAL( cursorPositionChanged() ), this, SLOT( slotCursorPositionChanged()));
}

FMTextEdit::~FMTextEdit() {
}

bool FMTextEdit::replace(QString text, QString reptext, QTextDocument::FindFlags flag) {
  if (textCursor().selectedText() == text) {
    QTextCursor cursor(textCursor());
    cursor.insertText(reptext);
    cursor.movePosition(QTextCursor::Left,QTextCursor::KeepAnchor,reptext.size());
    setTextCursor(cursor);
    return true;
  }
  if (find(text,flag)) {
    QTextCursor cursor(textCursor());
    cursor.insertText(reptext);
    cursor.movePosition(QTextCursor::Left,QTextCursor::KeepAnchor,reptext.size());
    setTextCursor(cursor);
    return true;
  } else
    return false;
}

int FMTextEdit::replaceAll(QString text, QString reptext, QTextDocument::FindFlags flag) {
  textCursor().beginEditBlock();
  int repcount = 0;
  while (replace(text,reptext,flag))
    repcount++;
  textCursor().endEditBlock();
  return repcount;
}

void FMTextEdit::comment() {
  QTextCursor cursor(textCursor());
  QTextCursor line1(cursor);
  QTextCursor line2(cursor);
  if (cursor.position() < cursor.anchor()) {
    line2.setPosition(cursor.anchor());
  } else {
    line1.setPosition(cursor.anchor());
  }
  line1.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
  QTextCursor line2Copy(line2);
  line2.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
  if (line2.position() == line2Copy.position()) //at beginning of line, ignore this line
    line2.movePosition(QTextCursor::Up,QTextCursor::MoveAnchor);
  QTextCursor pos(line1);
  pos.beginEditBlock();
  while (pos.position() < line2.position()) { 
    pos.insertText("% "); 
    pos.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor);
  }
  pos.insertText("% "); 
  pos.endEditBlock();
}

void FMTextEdit::uncomment() {
  QTextCursor cursor(textCursor());
  QTextCursor line1(cursor);
  QTextCursor line2(cursor);
  if (cursor.position() < cursor.anchor()) {
    line2.setPosition(cursor.anchor());
  } else {
    line1.setPosition(cursor.anchor());
  }
  line1.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
  QTextCursor line2Copy(line2);
  line2.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
  if (line2.position() == line2Copy.position()) //at beginning of line, ignore this line
    line2.movePosition(QTextCursor::Up,QTextCursor::MoveAnchor);
  QTextCursor pos(line1);
  pos.beginEditBlock();
  while (pos.position() < line2.position()) { 
    pos.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor);
    if (pos.selectedText() == "%") {
      pos.deleteChar();
      pos.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor);
      if (pos.selectedText() == " ")  
        pos.deleteChar();
    }
    pos.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor);
    pos.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
  }

  pos.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor);
  if (pos.selectedText() == "%") {
    pos.deleteChar();
    pos.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor);
    if (pos.selectedText() == " ")  
      pos.deleteChar();
  }
  pos.endEditBlock();
}

void FMTextEdit::increaseIndent() {
  QString Blanks;
  Blanks.fill(' ',indentSize);
  QTextCursor cursor(textCursor());
  QTextCursor line1(cursor);
  QTextCursor line2(cursor);
  if (cursor.position() == cursor.anchor()) { //add blanks to align text
    QTextCursor pos(cursor);
    pos.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
    QString NewBlanks(Blanks);
    NewBlanks.chop((cursor.position()-pos.position()+indentSize)%indentSize);
    cursor.insertText(NewBlanks);
    return;
  }
  
  if (cursor.position() < cursor.anchor()) {
    line2.setPosition(cursor.anchor());
  } else {
    line1.setPosition(cursor.anchor());
  }
  line1.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
  QTextCursor line2Copy(line2);
  line2.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
  if (line2.position() == line2Copy.position()) //at beginning of line, ignore this line
    line2.movePosition(QTextCursor::Up,QTextCursor::MoveAnchor);
 
  QTextCursor pos(line1);
  pos.beginEditBlock();
  while (pos.position() < line2.position()) { 
    pos.insertText(Blanks);
    pos.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor);
  }
  pos.insertText(Blanks);
  pos.endEditBlock();
}

void FMTextEdit::decreaseIndent() {
  QTextCursor cursor(textCursor());
  QTextCursor line1(cursor);
  QTextCursor line2(cursor);
  if (cursor.position() < cursor.anchor()) {
    line2.setPosition(cursor.anchor());
  } else {
    line1.setPosition(cursor.anchor());
  }
  line1.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
  QTextCursor line2Copy(line2);
  line2.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
  if (line2.position() == line2Copy.position()) //at beginning of line, ignore this line
    line2.movePosition(QTextCursor::Up,QTextCursor::MoveAnchor);
  QTextCursor pos(line1);
  pos.beginEditBlock();
  while (pos.position() < line2.position()) { 
    for (int i=0; i<indentSize; i++) { //remove at most "indentSize" of blank characters
      pos.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor);
      if (pos.selectedText() == " ") {
        pos.deleteChar();
      }
      else
      	break;
    }
    pos.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor);
    pos.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
  }

  for (int i=0; i<indentSize; i++) { //remove at most "indentSize" of  blank characters
    pos.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor);
    if (pos.selectedText() == " ") {
      pos.deleteChar();
    }
    else
      break;
  }
  pos.endEditBlock();
}

void FMTextEdit::keyPressEvent(QKeyEvent*e) {
  bool tab = false;
  int keycode = e->key();
  bool delayedIndent = false;
  if (keycode) {
    QByteArray p(e->text().toAscii());
    char key;
    if (!e->text().isEmpty())
      key = p[0];
    else
      key = 0;
    if (key == 0x09) {
      tab = true;
      if (indentActive)
	emit indent();
    }
    if (key == 13) {
      delayedIndent = true;
      if (indentActive)
	emit indent();
    }
  }
  if (!tab || !indentActive)
    QTextEdit::keyPressEvent(e);
  else
    e->accept();
  if (indentActive)
    if (delayedIndent) 
      emit indent();
}

bool FMTextEdit::event(QEvent *event){ 
  if (event->type() == QEvent::ToolTip) {
    QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
    QTextCursor Cursor = cursorForPosition(helpEvent->pos());
    Cursor.select(QTextCursor::WordUnderCursor);
    QString textSelected = Cursor.selectedText();
    emit showDataTips(helpEvent->globalPos(), textSelected);
  }
  return QTextEdit::event(event);
}

void FMTextEdit::slotCursorPositionChanged()
{
    if ( matchActive ) {
      plainText = toPlainText();
      bool matchbefore = (matchingBegin !=-1 && matchingEnd != -1);
      bool matchnow = findmatch();
      if ( matchnow || (!matchnow && matchbefore) )
        viewport()->update();
   }
}

void FMTextEdit::setMatchBracket(bool flag)
{
    matchActive  = flag;
    viewport()->update();
}

QString simplifiedMCode(const QString Str)
{
    enum
    {
        NORMAL = -1,
        NORMAL_WITH_VAR,
        NORMAL_IN_PAR,
        COMMENT,
        STRING
    };
   int state = NORMAL;
   QString SimplifiedString = Str;
   for (int i = 0; i < SimplifiedString.count(); i++)
    {
        QChar ch = SimplifiedString[i];
        int nPar = 0;
        switch (state)
        {
            case NORMAL:
                if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || 
                    (ch >= '0' && ch <= '9') || (ch == ']'))
                    state = NORMAL_WITH_VAR;                    
                else if ((ch == '(') ) {
                    state = NORMAL_IN_PAR;
                    nPar++;
                }
                else if (ch == '%') // remove comments
                {
                    state = COMMENT;
                    SimplifiedString[i] = ' ';
                }
                else if (ch == '\'' )
                {
                    state = STRING;
                }
                break;
            case NORMAL_WITH_VAR:
                if ((ch == ' ') || (ch == ',') || (ch == '[') ) {
                    if (nPar <= 0)
                        state = NORMAL;
                    else
                        state = NORMAL_IN_PAR;
                }                   
                else if ((ch == '(') ) {
                    state = NORMAL_IN_PAR;
                    nPar++;
                }
                else if (ch == '%') // remove comments
                {
                    state = COMMENT;
                    SimplifiedString[i] = ' ';
                }
                break;
            case NORMAL_IN_PAR:
                if (ch == ')') {
                    nPar--;
                    if (nPar <= 0)
                        state = NORMAL_WITH_VAR;
                }
                else if (SimplifiedString.mid(i, 3) == "end") //remove "end" as index of array
                {
                    SimplifiedString[i]   = ' ';
                    SimplifiedString[i+1] = ' ';
                    SimplifiedString[i+2] = ' ';
                }
                else if (ch == '%') // remove comments
                {
                    state = COMMENT;
                    SimplifiedString[i] = ' ';
                }
                break;
            case COMMENT:
                if (ch == '\n')
                {
                    if (nPar <= 0)
                        state = NORMAL;
                    else
                        state = NORMAL_IN_PAR;
                }
                else // remove comments
                	SimplifiedString[i] = ' ';
                break;
            case STRING:
                if (ch == '\'')
                {
                   if (nPar <= 0)
                        state = NORMAL;
                    else
                        state = NORMAL_IN_PAR;
                }
                else // remove string
	                SimplifiedString[i] = ' ';
                break;
        }
    }
    return SimplifiedString;
}

bool FMTextEdit::findmatch()
{
	QString simpleCodes =  simplifiedMCode( plainText );
    QTextCursor cursor = textCursor();
    int pos = cursor.position();
   	matchingBegin = -1;
   	matchingEnd = -1;
    Key = "";
    matchKey = "";
    if ( pos==-1  || cursor.atEnd() || simpleCodes.at(pos) == '_')
    	return false;
    
    QChar ch = simpleCodes.at(pos);
    if (QString("({[]})").contains(simpleCodes.at(pos)))
       ch = simpleCodes.at(pos);
    else if (!cursor.atStart() && QString("({[]})").contains(simpleCodes.at(pos-1))) {
       pos--;
       ch = simpleCodes.at(pos);
    }
    else {
        cursor.select(QTextCursor::WordUnderCursor);
        Key = cursor.selectedText();
        if (Key.isEmpty() || 
           !QString("if elseif else switch case otherwise for while try catch end").contains(Key))
            return false;
        else
            pos = cursor.selectionStart(); //readjust to the position of first letter
    }
    
    QChar matchCh;
    int inc = 1;
    if (ch == '(')
    	matchCh = ')';
    else if (ch == '{')
    	matchCh = '}';
    else if (ch == '[')
    	matchCh = ']';
    else if (ch == ')')
    {
    	matchCh = '(';
    	inc = -1;
   	}
    else if(ch == '}')
    {
    	matchCh = '{';
    	inc = -1;
   	}
    else if (ch == ']')
    {
    	matchCh = '[';
    	inc = -1;
   	}
    else if (ch == ']')
    {
    	matchCh = '[';
    	inc = -1;
   	}
    else if (Key.isEmpty())
    	return false;
    	
    matchingBegin = pos;
    int nb = 0;
    if (Key.isEmpty()) {
        do
        {
        	if (simpleCodes.at(pos) == ch)
        		nb++;
        	else if (simpleCodes.at(pos) == matchCh)
        	{
        		nb--;
        		if (nb == 0)
        		{
        			matchingEnd = pos;
        			break;
       			}
       		}
       		pos += inc;
       	}
        while(pos >= 0 && pos < simpleCodes.length());
        
        if(matchingEnd !=-1) { //found match, save brackets
            Key = ch;
            matchKey = matchCh;
        }
    } 
    else {
        if (Key == "if" || Key == "switch" || Key == "for" || 
                 Key == "while" || Key == "try" ) { 
            // search forward
            nb = 1;
            inc = 1;
            pos = cursor.selectionEnd()+inc; 
        }
        else if (Key == "end" || Key == "else" || Key == "elseif" || 
            Key == "case" || Key == "otherwise" || Key == "catch") { 
            // search backward
            nb = -1;
            inc = -1;
            pos = cursor.selectionStart()+inc; 
        }
        else
            return false;
        do {
            QChar chr = simpleCodes.at(pos);
            if (chr < 'a' || chr > 'z' ) { //ignore if not a small alphabet character
                pos += inc;
            }
            else {
                int old_pos = pos;
                cursor.clearSelection();
        		cursor.setPosition(pos);
                cursor.select(QTextCursor::WordUnderCursor);
                matchKey = cursor.selectedText();
                if (matchKey.isEmpty()) {
                    pos += inc;
                }
                else {
                	if (matchKey == "if" || matchKey == "switch" || matchKey == "for" || 
                	    matchKey == "while" || matchKey == "try" )
                		nb++;
                	else if (matchKey == "end")
                		nb--;
                         
            		if (nb == 0)
            		{
            			matchingEnd = cursor.selectionStart();
            			break;
           			}
           			if (inc > 0) {
                        pos = cursor.selectionEnd()+inc;
                        // some characters cause selection to move backward
                        // if so force it move forward
                        if (pos <= old_pos) 
                            pos = old_pos + inc;                      
                    }
                    else {
                        pos = cursor.selectionStart()+inc;
                        // some characters may cause selection to move forward
                        // if so force it move backward
                        if (pos >= old_pos) 
                            pos = old_pos + inc;                      
                    }
                }
            }
       	} while(pos >= 0 && pos < simpleCodes.length());
    }

    if(matchingEnd !=-1) //found match
      return true;
    else
      return false;
}

void FMTextEdit::paintEvent(QPaintEvent *event) {
  QPainter painter( viewport() );
  if (matchActive && matchingBegin != -1 && matchingEnd != -1) {
	const int contentsY = verticalScrollBar()->value();
	const qreal pageBottom = contentsY + viewport()->height();
	
	for ( QTextBlock block = document()->begin(); block.isValid(); block = block.next() )
	{
		QTextLayout* layout = block.layout();
		const QRectF boundingRect = layout->boundingRect();
		QPointF position = layout->position();
		
		if ( position.y() +boundingRect.height() < contentsY )
			continue;
		if ( position.y() > pageBottom )
			break;
		
		const QString txt = block.text();
		const int len = txt.length();
		
		for ( int i=0; i<len; i++)
		{
			if( block.position() + i == matchingBegin)
			{
				QTextCursor cursor = textCursor();
				cursor.setPosition( block.position() + i, QTextCursor::MoveAnchor);
				QRect r1 = cursorRect( cursor );
				cursor.movePosition( QTextCursor::Right, QTextCursor::MoveAnchor, Key.size());
				QRect r2 = cursorRect( cursor );
				painter.setPen( matchingColor );
                painter.drawLine( r1.x()+r1.width()/2, r1.y()+r1.height()-2, r2.x()+r2.width()/2, r2.y()+r2.height()-2);
			}
			else if(block.position() + i == matchingEnd)
			{
				QTextCursor cursor = textCursor();
				cursor.setPosition( block.position() + i, QTextCursor::MoveAnchor);
				QRect r1 = cursorRect( cursor );
				cursor.movePosition( QTextCursor::Right, QTextCursor::MoveAnchor, matchKey.size());
				QRect r2 = cursorRect( cursor );
				painter.setPen( matchingColor );
                painter.drawLine( r1.x()+r1.width()/2, r1.y()+r1.height()-2, r2.x()+r2.width()/2, r2.y()+r2.height()-2);
			}
		}
	}
  }
  QTextEdit::paintEvent( event );
}

void FMTextEdit::contextMenuEvent(QContextMenuEvent* e) {
  e->ignore();
}

void FMTextEdit::fontUpdate() {
  QFontMetrics fm(font());
  QFontInfo fi(font());
  if (!fi.fixedPitch()) 
    QMessageBox::warning(this,"FreeMat",
			 "You have selected a font that is not a fixed pitch.\nThe editor really requires a fixed pitch font to work.");
  setTabStopWidth(fm.width(' ')*indentSize);
}

FMIndent::FMIndent() {
  QSettings settings("FreeMat","FreeMat");
  indentSize = settings.value("editor/tab_size",3).toInt();
}

FMIndent::~FMIndent() {
}

void FMIndent::setDocument(FMTextEdit *te) {
  m_te = te; 
}

FMTextEdit* FMIndent::document() const {
  return m_te;
}

void removeMatch(QString &a, QRegExp &pattern) {
  int k = 0;
  while ((k = a.indexOf(pattern,k)) != -1) {
    for ( int i = 0; i < pattern.matchedLength(); i++ )
      a.replace(k+i,1,'X');
    k += pattern.matchedLength();
  }
}

int countMatches(QString a, QRegExp &pattern) {
  int matchCount = 0;
  int k = 0;
  while ((k = a.indexOf(pattern,k)) != -1) {
    matchCount++;
    k += pattern.matchedLength();
  }
  return matchCount;
}

QString setIndentSpace(QString toIndent, int leading_space) {
  QRegExp whitespace("^\\s*");
  int k;
  if ((k = toIndent.indexOf(whitespace,0)) != -1)
    toIndent.remove(0,whitespace.matchedLength());
  toIndent = QString(leading_space,' ') + toIndent;
  return toIndent;
}

bool allWhiteSpace(QString a) {
  QRegExp whitespace("^\\s*");
  int k;
  if ((k = a.indexOf(whitespace,0)) != -1)
    a.remove(0,whitespace.matchedLength());
  return (a.isEmpty());
}

QString stripLine(QString a) {
  // 2.  Replace string literals in <a> with dummy characters to avoid confusion,
  //     i.e., 'blahbla' --> xxxxxxxx
  QRegExp literal("\'([^\']*)\'");
  removeMatch(a,literal);
  // 3.  Replace 'end' keyword as used as a dimension token with dummy characters
  //     to avoid confusion,
  //     i.e., A(end,:) --> A(xxx,:)
  //     We do this by replacing \([^\)]*(\bend\b)[^\)]*\) --> xxx
  QRegExp endparen("\\([^\\)]*(\\bend\\b)[^\\)]*\\)");
  removeMatch(a,endparen);
  QRegExp endbracket("\\{[^\\}]*(\\bend\\b)[^\\}]*\\}");
  removeMatch(a,endbracket);
  // 4.  Strip comments
  //     i.e., % foo --> %
  QRegExp comment("%.*");
  removeMatch(a,comment);
  return a;
}

int computeIndexIncrement(QString a) {
  // 5.  Compute the incremental index level - this is the number of
  //     matches to: if, else, elseif, for, function, try, catch, while, switch
  //     minus the number of matches to 'end'.
  QRegExp keyword_in("\\b(if|for|function|try|while|switch)\\b");
  QRegExp keyword_out("\\bend\\b");
  int indent_in_count = countMatches(a,keyword_in);
  int indent_out_count = countMatches(a,keyword_out);
  int indent_increment = indent_in_count - indent_out_count;
  return indent_increment;
}

QString indentLine(QString toIndent, QStringList priorText, int indentSize) {
  // Two observations:
  //   1.  If the _current_ line contains contains 'end' in excess of 'for', etc., then
  //       the indentation for the current line should be 1 less.
  //   2.  The cursor should not move relative to the text when things are tabbed
  QString a;
  // 1.  Let <a> be the line of non-blank text prior to the current one.  If
  //     no such line exists, then we are done (no change).
  while (!priorText.empty() && allWhiteSpace(priorText.last())) 
    priorText.removeLast();
  if (priorText.empty())
    return setIndentSpace(toIndent,0);
  a = priorText.last();
  // Strip the prior line of confusing constructs...
  a = stripLine(a);
  int indent_increment = computeIndexIncrement(a);
  QString b = stripLine(toIndent);
  // Some lines require an adjustment: "catch,elseif,else,end"
  QRegExp keyword_adjust("^\\s*\\b(end|else|elseif|catch)\\b");
  if (b.indexOf(keyword_adjust) >= 0) {
    indent_increment--;
  }
  if (a.indexOf(keyword_adjust) >= 0) {
    indent_increment++;
  }
  QRegExp function_det("^\\s*\\b(function)\\b");
  if (b.indexOf(function_det) >= 0) {
    return setIndentSpace(toIndent,0);
  }
  // 6.  Find the start of non-white text in <a>, add incremental index*tab size - this 
  //     is the amount of white space at the beginning of the current line.
  QRegExp whitespace("^\\s*");
  int leading_space = 0;
  int k = 0;
  if ((k = a.indexOf(whitespace,0)) != -1)
    leading_space = whitespace.matchedLength();
  leading_space += indent_increment*indentSize;
  leading_space = qMax(leading_space,0);
  // Indenting is simpler for us than for QSA or for C++.  We simply
  // apply the following rules:
  // 7.  Adjust the current line.
  //    if ((b.indexOf(else_only) >= 0)) 
  //     indent_increment++;
  return setIndentSpace(toIndent,leading_space);
}

void FMIndent::update() {
  QTextCursor cursor(m_te->textCursor());
  QTextCursor save(cursor);
  QTextCursor final(cursor);
  // Get the current cursor position relative to the start of the line
  final.movePosition(QTextCursor::StartOfLine,QTextCursor::KeepAnchor);
  int curpos = final.selectedText().length();
  cursor.movePosition(QTextCursor::StartOfLine);
  cursor.movePosition(QTextCursor::EndOfLine,QTextCursor::KeepAnchor);
  QString toIndent(cursor.selectedText());
  cursor.movePosition(QTextCursor::StartOfLine);
  cursor.movePosition(QTextCursor::Start,QTextCursor::KeepAnchor);
  QStringList priorlines(cursor.selection().toPlainText().split("\n"));
  QString indented(indentLine(toIndent,priorlines,indentSize));
  save.movePosition(QTextCursor::StartOfLine);
  save.movePosition(QTextCursor::EndOfLine,QTextCursor::KeepAnchor);
  save.insertText(indented);
  // Move the cursor to where it was relative to the original text
  // The number of characters inserted 
  int orig_length = toIndent.length();
  int new_length = indented.length();
  int new_pos = qMax(curpos + new_length-orig_length,0);
  final.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
  final.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,new_pos);
  m_te->setTextCursor(final);
}

Interpreter* FMEditPane::getInterpreter() {
  return m_eval;
}

FMEditPane::FMEditPane(Interpreter* eval) : QWidget() {
  m_eval = eval;
  tEditor = new FMTextEdit;
  LineNumber *tLN = new LineNumber(tEditor);
  BreakPointIndicator *tBP = new BreakPointIndicator(tEditor,this);
  QHBoxLayout *layout = new QHBoxLayout;
  layout->addWidget(tLN);
  layout->addWidget(tBP);
  layout->addWidget(tEditor);
  setLayout(layout);
  FMIndent *ind = new FMIndent;

  connect(tEditor,SIGNAL(indent()),ind,SLOT(update()));
  Highlighter *highlight = new Highlighter(tEditor->document());
  ind->setDocument(tEditor);
}


FMTextEdit* FMEditPane::getEditor() {
  return tEditor;
}

void FMEditPane::setFileName(QString filename) {
  curFile = filename;
}

QString FMEditPane::getFileName() {
  return curFile;
}

int FMEditPane::getLineNumber() {
  QTextCursor tc(tEditor->textCursor());
  tc.movePosition(QTextCursor::StartOfLine);
  int linenumber = 1;
  while (!tc.atStart()) {
    tc.movePosition(QTextCursor::Up);
    linenumber++;
  }
  return linenumber;
}

FMEditor::FMEditor(Interpreter* eval) : QMainWindow() {
  m_eval = eval;
  setWindowIcon(QPixmap(":/images/freemat_small_mod_64.png"));
  prevEdit = NULL;
  tab = new QTabWidget(this);
  setCentralWidget(tab);
  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  readSettings();
  connect(tab,SIGNAL(currentChanged(int)),this,SLOT(tabChanged(int)));
  addTab();
  tabChanged(0);
  m_find = new FMFindDialog;
  connect(m_find,SIGNAL(doFind(QString,bool,bool)),
 	  this,SLOT(doFind(QString,bool,bool)));
  m_replace = new FMReplaceDialog;
  connect(m_replace,SIGNAL(doFind(QString,bool,bool)),
 	  this,SLOT(doFind(QString,bool,bool)));
  connect(m_replace,SIGNAL(doReplace(QString,QString,bool,bool)),
 	  this,SLOT(doReplace(QString,QString,bool,bool)));
  connect(m_replace,SIGNAL(doReplaceAll(QString,QString,bool,bool)),
 	  this,SLOT(doReplaceAll(QString,QString,bool,bool)));
  std::string mpath = m_eval->getTotalPath();
  QString path = QString::fromStdString(mpath);
#ifdef WIN32
#define PATHSEP ";"
#else
#define PATHSEP ":"
#endif
  pathList = path.split(PATHSEP,QString::SkipEmptyParts);
}

void FMEditor::doFind(QString text, bool backwards, bool sensitive) {
  QTextDocument::FindFlags flags;
  if (backwards) flags = QTextDocument::FindBackward;
  if (sensitive) flags = flags | QTextDocument::FindCaseSensitively;
  if (!currentEditor()->find(text,flags)) {
    m_find->notfound();
    m_replace->notfound();
  }  else {
    m_find->found();
    m_replace->found();
  }
}

void FMEditor::doReplace(QString text, QString replace, 
			 bool backwards, bool sensitive) {
  QTextDocument::FindFlags flags;
  if (backwards) flags = QTextDocument::FindBackward;
  if (sensitive) flags = flags | QTextDocument::FindCaseSensitively;
  if (!currentEditor()->replace(text,replace,flags)) 
    m_replace->notfound();
  else
    m_replace->found();
}

void FMEditor::doReplaceAll(QString text, QString reptxt, 
			    bool backwards, bool sensitive) {
  QTextDocument::FindFlags flags;
  if (backwards) flags = QTextDocument::FindBackward;
  if (sensitive) flags = flags | QTextDocument::FindCaseSensitively;
  int repcount = currentEditor()->replaceAll(text,reptxt,flags);
  m_replace->showrepcount(repcount);
}

void FMEditor::readSettings() {
  QSettings settings("FreeMat", "FreeMat");
  QPoint pos = settings.value("editor/pos", QPoint(200, 200)).toPoint();
  QSize size = settings.value("editor/size", QSize(400, 400)).toSize();
  resize(size);
  move(pos);
  QString font = settings.value("editor/font").toString();
  if (!font.isNull()) {
    QFont new_font;
    new_font.fromString(font);
    m_font = new_font;
  }
  updateFont();
  isShowToolTip = settings.value("editor/tooltip_enable", true).toBool();
  dataTipConfigAct->setChecked(isShowToolTip); 
  isMatchBracket = settings.value("editor/match_enable", true).toBool();
  bracketMatchConfigAct->setChecked(isMatchBracket); 
}

void FMEditor::updateFont() {
  for (int i=0;i<tab->count();i++) {
    QWidget *p = tab->widget(i);
    FMEditPane *te = qobject_cast<FMEditPane*>(p);
    te->setFont(m_font);
    te->getEditor()->fontUpdate();
  }
}

void FMEditor::writeSettings() {
  QSettings settings("FreeMat", "FreeMat");
  settings.setValue("editor/pos", pos());
  settings.setValue("editor/size", size());
  settings.setValue("editor/font", m_font.toString());
  settings.setValue("editor/tooltip_enable", isShowToolTip);
  settings.setValue("editor/match_enable", isMatchBracket);
  settings.sync();
}

QString FMEditor::currentFilename() {
  QWidget *p = tab->currentWidget();
  FMEditPane *te = qobject_cast<FMEditPane*>(p);
  if (!te) {
    addTab();
    p = tab->currentWidget();
    te = qobject_cast<FMEditPane*>(p);
  }
  return te->getFileName();
}

void FMEditor::setCurrentFilename(QString filename) {
  QWidget *p = tab->currentWidget();
  FMEditPane *te = qobject_cast<FMEditPane*>(p);
  if (!te) {
    addTab();
    p = tab->currentWidget();
    te = qobject_cast<FMEditPane*>(p);
  }
  te->setFileName(filename);
}

FMTextEdit* FMEditor::currentEditor() {
  QWidget *p = tab->currentWidget();
  FMEditPane *te = qobject_cast<FMEditPane*>(p);
  if (!te) {
    addTab();
    p = tab->currentWidget();
    te = qobject_cast<FMEditPane*>(p);
  }
  return te->getEditor();
}

void FMEditor::addTab() {
  tab->addTab(new FMEditPane(m_eval),"untitled.m");
  tab->setCurrentIndex(tab->count()-1);
  updateFont();
}

FMEditor::~FMEditor() {
}

QString FMEditor::shownName() {
  QString sName;
  if (currentFilename().isEmpty())
    sName = "untitled.m";
  else
    sName = strippedName(currentFilename());
  return sName;
}

void FMEditor::updateTitles() {
  tab->setTabText(tab->currentIndex(),shownName());
  setWindowTitle(QString("%1[*]").arg(shownName()) + " - " + QString::fromStdString(Interpreter::getVersionString()) + " Editor");
  documentWasModified();
}


void FMEditor::tabChanged(int newslot) {
  disconnect(cutAct,SIGNAL(triggered()),0,0);
  disconnect(copyAct,SIGNAL(triggered()),0,0);
  disconnect(pasteAct,SIGNAL(triggered()),0,0);
  connect(cutAct,SIGNAL(triggered()),currentEditor(),SLOT(cut()));
  connect(copyAct,SIGNAL(triggered()),currentEditor(),SLOT(copy()));
  connect(pasteAct,SIGNAL(triggered()),currentEditor(),SLOT(paste()));
  connect(this,SIGNAL(setMatchBracket(bool)),
          currentEditor(),SLOT(setMatchBracket(bool)));
  // Disconnect each of the contents changed signals
  if (prevEdit) {
    disconnect(prevEdit->document(),SIGNAL(contentsChanged()),0,0);
    disconnect(prevEdit,SIGNAL(showDataTips(QPoint, QString)),0,0);
  }
  // NEED TO DISCONNECT...
  connect(currentEditor()->document(),SIGNAL(contentsChanged()),this,SLOT(documentWasModified()));
  connect(currentEditor(),SIGNAL(showDataTips(QPoint, QString)),
          this,SLOT(showDataTips(QPoint, QString)));
  updateTitles();
  prevEdit = currentEditor();
}

void FMEditor::documentWasModified() {
  setWindowModified(currentEditor()->document()->isModified());
  if (currentEditor()->document()->isModified()) 
    tab->setTabText(tab->currentIndex(),shownName()+"*");
  else
    tab->setTabText(tab->currentIndex(),shownName());
}

void FMEditor::createActions() {
  newAct = new QAction(QIcon(":/images/new.png"),"&New Tab",this);
  newAct->setShortcut(Qt::Key_N | Qt::CTRL);
  connect(newAct,SIGNAL(triggered()),this,SLOT(addTab()));
  openAct = new QAction(QIcon(":/images/open.png"),"&Open",this);
  openAct->setShortcut(Qt::Key_O | Qt::CTRL);
  connect(openAct,SIGNAL(triggered()),this,SLOT(open()));
  saveAct = new QAction(QIcon(":/images/save.png"),"&Save",this);
  saveAct->setShortcut(Qt::Key_S | Qt::CTRL);
  connect(saveAct,SIGNAL(triggered()),this,SLOT(save()));
  saveAsAct = new QAction("Save &As",this);
  
  for (int i = 0; i < MaxRecentFiles; ++i) {
    recentFileActs[i] = new QAction(this);
	recentFileActs[i]->setVisible(false);
	connect(recentFileActs[i], SIGNAL(triggered()),
		     this, SLOT(openRecentFile()));
  }

  connect(saveAsAct,SIGNAL(triggered()),this,SLOT(saveAs()));
  quitAct = new QAction(QIcon(":/images/quit.png"),"&Quit Editor",this);
  quitAct->setShortcut(Qt::Key_Q | Qt::CTRL);
  connect(quitAct,SIGNAL(triggered()),this,SLOT(close()));
  closeAct = new QAction(QIcon(":/images/close.png"),"&Close Tab",this);
  closeAct->setShortcut(Qt::Key_W | Qt::CTRL);
  connect(closeAct,SIGNAL(triggered()),this,SLOT(closeTab()));
  copyAct = new QAction(QIcon(":/images/copy.png"),"&Copy",this);
  copyAct->setShortcut(Qt::Key_C | Qt::CTRL);
  cutAct = new QAction(QIcon(":/images/cut.png"),"Cu&t",this);
  cutAct->setShortcut(Qt::Key_X | Qt::CTRL);
  pasteAct = new QAction(QIcon(":/images/paste.png"),"&Paste",this);
  pasteAct->setShortcut(Qt::Key_V | Qt::CTRL);
  fontAct = new QAction("&Font",this);
  connect(fontAct,SIGNAL(triggered()),this,SLOT(font()));
  findAct = new QAction(QIcon(":/images/find.png"),"&Find",this);
  findAct->setShortcut(Qt::Key_F | Qt::CTRL);
  connect(findAct,SIGNAL(triggered()),this,SLOT(find()));
  commentAct = new QAction("Comment Region",this);
  commentAct->setShortcut(Qt::Key_R | Qt::CTRL); 
  connect(commentAct,SIGNAL(triggered()),this,SLOT(comment()));
  uncommentAct = new QAction("Uncomment Region",this);
  uncommentAct->setShortcut(Qt::Key_T | Qt::CTRL); 
  connect(uncommentAct,SIGNAL(triggered()),this,SLOT(uncomment()));
  increaseIndentAct = new QAction("Increase Indent",this);
  increaseIndentAct->setShortcut(Qt::Key_Tab); 
  connect(increaseIndentAct,SIGNAL(triggered()),this,SLOT(increaseIndent()));
  decreaseIndentAct = new QAction("Decrease Indent",this);
  decreaseIndentAct->setShortcut(Qt::Key_Tab | Qt::SHIFT); 
  connect(decreaseIndentAct,SIGNAL(triggered()),this,SLOT(decreaseIndent()));
  undoAct = new QAction("Undo Edits",this);
  undoAct->setShortcut(Qt::Key_Z | Qt::CTRL);
  connect(undoAct,SIGNAL(triggered()),this,SLOT(undo()));
  redoAct = new QAction("Redo Edits",this);
  redoAct->setShortcut(Qt::Key_Y | Qt::CTRL);
  connect(redoAct,SIGNAL(triggered()),this,SLOT(redo()));
  replaceAct = new QAction("Find and Replace",this);
  replaceAct->setShortcut(Qt::Key_H | Qt::CTRL); 
  connect(replaceAct,SIGNAL(triggered()),this,SLOT(replace()));
  helpWinAct = new QAction("Online &Manual",this);
  helpWinAct->setShortcut(Qt::Key_F1); 
  connect(helpWinAct,SIGNAL(triggered()),this,SLOT(helpWin()));
  helpOnSelectionAct = new QAction("Help on Selection",this);
  helpOnSelectionAct->setShortcut(Qt::Key_F2);
  connect(helpOnSelectionAct,SIGNAL(triggered()),this,SLOT(helpOnSelection()));
  openSelectionAct = new QAction("Open Selection",this);
  connect(openSelectionAct,SIGNAL(triggered()),this,SLOT(openSelection()));
  dbStepAct = new QAction(QIcon(":/images/dbgnext.png"),"&Step Over",this);
  dbStepAct->setShortcut(Qt::Key_F10); 
  connect(dbStepAct,SIGNAL(triggered()),this,SLOT(dbstep()));
  dbTraceAct = new QAction(QIcon(":/images/dbgstep.png"),"&Step Into",this);
  dbTraceAct->setShortcut(Qt::Key_F11); 
  connect(dbTraceAct,SIGNAL(triggered()),this,SLOT(dbtrace()));
  dbContinueAct = new QAction(QIcon(":/images/dbgrun.png"),"&Continue",this);
  dbContinueAct->setShortcut(Qt::Key_Down | Qt::CTRL); 
  connect(dbContinueAct,SIGNAL(triggered()),this,SLOT(dbcontinue()));
  dbSetClearBPAct = new QAction(QIcon(":/images/stop.png"),"Set/Clear Breakpoint",this);
  dbSetClearBPAct->setShortcut(Qt::Key_F12); 
  connect(dbSetClearBPAct,SIGNAL(triggered()),this,SLOT(dbsetclearbp()));
  dbStopAct = new QAction(QIcon(":/images/player_stop.png"),"Stop Debugging",this);
  dbStopAct->setShortcut(Qt::Key_Escape | Qt::CTRL); 
  connect(dbStopAct,SIGNAL(triggered()),this,SLOT(dbstop()));
  colorConfigAct = new QAction("Text Highlighting",this);
  connect(colorConfigAct,SIGNAL(triggered()),this,SLOT(configcolors()));
  indentConfigAct = new QAction("Indenting",this);
  connect(indentConfigAct,SIGNAL(triggered()),this,SLOT(configindent()));
  dataTipConfigAct = new QAction("Show datatips",this);
  dataTipConfigAct->setCheckable(true);
  dataTipConfigAct->setShortcut(Qt::Key_F1 | Qt::SHIFT);
  connect(dataTipConfigAct,SIGNAL(triggered()),this,SLOT(configDataTip()));
  bracketMatchConfigAct = new QAction("Match brackets/keywords",this);
  bracketMatchConfigAct->setCheckable(true);
  bracketMatchConfigAct->setShortcut(Qt::Key_F2 | Qt::SHIFT);
  connect(bracketMatchConfigAct,SIGNAL(triggered()),this,SLOT(configBracketMatch()));
  executeSelectedAct = new QAction(QIcon(":/images/player_playselection.png"),"Execute Selected Text",this);
  executeSelectedAct->setShortcut(Qt::Key_F9); 
  connect(executeSelectedAct,SIGNAL(triggered()),this,SLOT(execSelected()));
  executeCurrentAct = new QAction(QIcon(":/images/player_play.png"),"Execute Current Buffer",this);
  executeCurrentAct->setShortcut(Qt::Key_F5); 
  connect(executeCurrentAct,SIGNAL(triggered()),this,SLOT(execCurrent()));
}

void FMEditor::execSelected() {
  emit EvaluateText(currentEditor()->textCursor().selectedText() + "\n");
}

void FMEditor::execCurrent() {
  if (currentFilename().isEmpty())
    QMessageBox::information(this,"Cannot execute unnamed buffer","You must save the current buffer into a file before it can be executed.");
  else {
    if (!maybeSave())
      return;
    emit EvaluateText("source '" + currentFilename() + "'\n");
  }
}

void FMEditor::undo() {
  currentEditor()->undo();
}

void FMEditor::redo() {
  currentEditor()->redo();
}

void FMEditor::comment() {
  currentEditor()->comment();
}

void FMEditor::uncomment() {
  currentEditor()->uncomment();
}

void FMEditor::increaseIndent() {
  currentEditor()->increaseIndent();
}

void FMEditor::decreaseIndent() {
  currentEditor()->decreaseIndent();
}

void FMEditor::find() {
  m_find->setFindText(currentEditor()->textCursor().selectedText());
  m_find->show();
  m_find->raise();
}

void FMEditor::replace() {
  m_replace->setReplaceText(currentEditor()->textCursor().selectedText());
  m_replace->show();
  m_replace->raise();
}

void FMEditor::helpWin() {
  emit EvaluateText("helpwin\n");
}

void FMEditor::helpOnSelection() {
  emit EvaluateText("helpwin " + currentEditor()->textCursor().selectedText() + "\n");
}

void FMEditor::openSelection() {
  loadFile(currentEditor()->textCursor().selectedText());
}

void FMEditor::createMenus() {
  fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction(newAct);
  fileMenu->addAction(openAct);
  fileMenu->addAction(saveAct);
  fileMenu->addAction(saveAsAct);
  fileMenu->addAction(closeAct);

  separatorAct = fileMenu->addSeparator();
  for (int i = 0; i < MaxRecentFiles; ++i)
    fileMenu->addAction(recentFileActs[i]);
  fileMenu->addSeparator();
  updateRecentFileActions();

  fileMenu->addAction(quitAct);
  editMenu = menuBar()->addMenu("&Edit");
  editMenu->addAction(undoAct);
  editMenu->addAction(redoAct);
  editMenu->addAction(copyAct);
  editMenu->addAction(cutAct);
  editMenu->addAction(pasteAct);
  QMenu* configMenu = editMenu->addMenu("&Preferences");
  configMenu->addAction(fontAct);
  configMenu->addAction(colorConfigAct);
  configMenu->addAction(indentConfigAct);
  configMenu->addAction(dataTipConfigAct);
  configMenu->addAction(bracketMatchConfigAct);
  toolsMenu = menuBar()->addMenu("&Tools");
  toolsMenu->addAction(findAct);
  toolsMenu->addAction(replaceAct);
  toolsMenu->addAction(commentAct);
  toolsMenu->addAction(uncommentAct);
  toolsMenu->addAction(increaseIndentAct);
  toolsMenu->addAction(decreaseIndentAct);
  debugMenu = menuBar()->addMenu("&Debug");
  debugMenu->addAction(executeCurrentAct);
  debugMenu->addAction(executeSelectedAct);
  debugMenu->addSeparator();
  debugMenu->addAction(dbStepAct);
  debugMenu->addAction(dbTraceAct);
  debugMenu->addAction(dbContinueAct);
  debugMenu->addAction(dbSetClearBPAct);
  debugMenu->addAction(dbStopAct);
  helpMenu = menuBar()->addMenu("&Help");
  helpMenu->addAction(helpWinAct);
  helpMenu->addAction(helpOnSelectionAct);
  m_popup = new QMenu;
  m_popup->addAction(openSelectionAct);
  m_popup->addAction(helpOnSelectionAct);
  m_popup->addAction(copyAct);
  m_popup->addAction(cutAct);
  m_popup->addAction(pasteAct);
  m_popup->addSeparator();
  m_popup->addAction(undoAct);
  m_popup->addAction(redoAct);
  m_popup->addSeparator();
  m_popup->addAction(findAct);
  m_popup->addAction(replaceAct);
  m_popup->addSeparator();
  m_popup->addAction(commentAct);
  m_popup->addAction(uncommentAct);
  m_popup->addSeparator();
  m_popup->addAction(executeCurrentAct);
  m_popup->addAction(executeSelectedAct);
  m_popup->addSeparator();
  m_popup->addAction(dbStepAct);
  m_popup->addAction(dbTraceAct);
  m_popup->addAction(dbContinueAct);
  m_popup->addAction(dbSetClearBPAct);
  m_popup->addAction(dbStopAct);
}

void FMEditor::contextMenuEvent(QContextMenuEvent *e) {
  m_popup->exec(e->globalPos());
}

void FMEditor::createToolBars() {
  fileToolBar = addToolBar("File");
  fileToolBar->addAction(newAct);
  fileToolBar->addAction(openAct);
  fileToolBar->addAction(saveAct);
  fileToolBar->addAction(closeAct);
  editToolBar = addToolBar("Edit");
  editToolBar->addAction(copyAct);
  editToolBar->addAction(cutAct);
  editToolBar->addAction(pasteAct);
  debugToolBar = addToolBar("Debug");
  debugToolBar->addAction(executeCurrentAct);
  debugToolBar->addAction(executeSelectedAct);
  debugToolBar->addSeparator();
  debugToolBar->addAction(dbStepAct);
  debugToolBar->addAction(dbTraceAct);
  debugToolBar->addAction(dbContinueAct);
  debugToolBar->addAction(dbSetClearBPAct);
  debugToolBar->addAction(dbStopAct);
}

void FMEditor::configcolors() {
  FMSynLightConf t(this);
  t.exec();
}

void FMEditor::configindent() {
  FMIndentConf t(this);
  t.exec();
}
 
void FMEditor::configDataTip() {
  if (dataTipConfigAct->isChecked()) {
     isShowToolTip = true;
     statusBar()->showMessage("Datatips on", 2000);
  }
  else {
     isShowToolTip = false;
     statusBar()->showMessage("Datatips off", 2000);
  }
}

void FMEditor::configBracketMatch() {
  if (bracketMatchConfigAct->isChecked()) {
     isMatchBracket = true;
     statusBar()->showMessage("Brackets & keywords matching on", 2000);
  }
  else {
     isMatchBracket = false;
     statusBar()->showMessage("Brackets & keywords matching off", 2000);
  }
  emit setMatchBracket(isMatchBracket);
}

void FMEditor::dbstep() {
  m_eval->ExecuteLine("dbstep\n");  
}

void FMEditor::dbtrace() {
  m_eval->ExecuteLine("dbtrace\n");  
}

void FMEditor::dbcontinue() {
  m_eval->ExecuteLine("return\n");
}

void FMEditor::dbsetclearbp() {
  QWidget *p = tab->currentWidget();
  FMEditPane *te = qobject_cast<FMEditPane*>(p);
//   // DEMO
//   QList<QTextEdit::ExtraSelection> selections;
//   QTextCursor cursor(te->getEditor()->textCursor());
//   cursor.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
//   cursor.movePosition(QTextCursor::Down,QTextCursor::KeepAnchor);
//   QTextCharFormat format(te->getEditor()->currentCharFormat());
//   format.setBackground(QBrush(Qt::red));
//   QTextEdit::ExtraSelection sel;
//   sel.format = format;
//   sel.cursor = cursor;
//   selections.push_back(sel);
//   te->getEditor()->dsetExtraSelections(selections);

  m_eval->toggleBP(te->getFileName(),te->getLineNumber());
}

void FMEditor::dbstop() {
  m_eval->ExecuteLine("retall\n");
}

void FMEditor::createStatusBar() {
  statusBar()->showMessage("Ready");
}

static QString lastfile;
static bool lastfile_set = false;

static QStringList GetOpenFileNames(QWidget *w, const QString &filePath = QString()) { 
  QStringList retfiles;
  if (!filePath.isEmpty())
    retfiles = QFileDialog::getOpenFileNames(w,"Open File in Editor",filePath,
					   "M files (*.m);;Text files (*.txt);;All files (*)");  
  else if (lastfile_set)
    retfiles = QFileDialog::getOpenFileNames(w,"Open File in Editor",lastfile,
					   "M files (*.m);;Text files (*.txt);;All files (*)");
  else
    retfiles = QFileDialog::getOpenFileNames(w,"Open File in Editor",QString(),
					   "M files (*.m);;Text files (*.txt);;All files (*)");
  return retfiles;
}

static QString GetSaveFileName(QWidget *w, const QString &filePath = QString()) { 
  QString retfile;
  if (!filePath.isEmpty())
    retfile = QFileDialog::getSaveFileName(w,"Save File",filePath,
					   "M files (*.m);;Text files (*.txt);;All files (*)");
  else if (lastfile_set)
    retfile = QFileDialog::getSaveFileName(w,"Save File",lastfile,
					   "M files (*.m);;Text files (*.txt);;All files (*)");
  else
    retfile = QFileDialog::getSaveFileName(w,"Save File",QString(),
					   "M files (*.m);;Text files (*.txt);;All files (*)");
  return retfile;  
}

bool FMEditor::isFileOpened(const QString &fileName) 
{
  // Check for one of the editors that might be editing this file already
  // if opened, set it as current
  for (int i=0;i<tab->count();i++) {
	QWidget *w = tab->widget(i);
	FMEditPane *te = qobject_cast<FMEditPane*>(w);
	if (te) {
	  if (te->getFileName() == fileName) {
	tab->setCurrentIndex(i);
	return true;
	  }
	}
  }
  return false;
}

void FMEditor::open() {
  QFileInfo fileInfo(currentFilename()); 
  QString filePath = fileInfo.absolutePath(); 
  QStringList fileNames = GetOpenFileNames(this, filePath);
  QStringList::Iterator it = fileNames.begin();
  while(it != fileNames.end()) {
    QString fileName = *it;
    loadFile(fileName);
    ++it;
  }
}

void FMEditor::openRecentFile() 
{
  QAction *action = qobject_cast<QAction *>(sender());
  QString fileName = action->data().toString();
  loadFile(fileName);
}


bool FMEditor::save() {
  if (currentFilename().isEmpty()) {
    return saveAs();
  } else {
    return saveFile(currentFilename());
  }
}

bool FMEditor::saveAs() {
  //QString fileName = GetSaveFileName(this);
  QString fileName;
  if (currentFilename().isEmpty()) 
    fileName = GetSaveFileName(this);
  else { //initial save path name are the same as current
    fileName = GetSaveFileName(this, currentFilename()); 
  }
  if (fileName.isEmpty())
    return false;
  // Check for a conflict
  for (int i=0;i<tab->count();i++) {
    QWidget *w = tab->widget(i);
    FMEditPane *te = qobject_cast<FMEditPane*>(w);
    if (te) {
      if ((te->getFileName() == fileName) 
	  && (i != tab->currentIndex())) {
	QMessageBox::critical(this,"FreeMat","Cannot save to filename\n " + fileName + "\n as this file is open in another tab.\n  Please close the other tab and\n then repeat the save operation.");
	tab->setCurrentIndex(i);
	return false;
      }
    }
  }
  return saveFile(fileName);
}

void FMEditor::RefreshBPLists() {
  update();
}

void FMEditor::IllegalLineOrCurrentPath(string name, int line) {
  QString fullname = QString::fromStdString(name);
  QFileInfo fileInfo(fullname);
  QString filePath = QFileInfo(fullname).absolutePath();
  QString currentPath = QDir::currentPath();
  if (filePath !=currentPath) {
     int ret = QMessageBox::warning(this, tr("FreeMat"),
				   "File " + fullname + " is not on the current path."
				   " To set breakpoint, do you want to change current "
				   " path to the path of this file?",
				   QMessageBox::Yes | QMessageBox::Default,
				   QMessageBox::No,
				   QMessageBox::Cancel | QMessageBox::Escape);
	if (ret == QMessageBox::Yes) {
	  emit EvaluateText("cd " + filePath + "\n");
	  // leave some time to finish the above command
#ifndef WIN32
	  sleep(1);
#else
	  Sleep(1);
#endif
	  // make sure the current path is the file path
	  // before execute toggleBP() 
	  currentPath = QDir::currentPath();
	  if (filePath == currentPath)
	     m_eval->toggleBP(fullname, line);
	   else
	     statusBar()->showMessage("Try again", 2000);
	}
  }
  else 
     statusBar()->showMessage("Illegal line number", 2000);
}

void FMEditor::ShowActiveLine() {
  // Find the tab with this matching filename
  QString tname(QString::fromStdString(m_eval->getInstructionPointerFileName()));
  if (tname == "") return;
  // Check for one of the editors that might be editing this file already
  for (int i=0;i<tab->count();i++) {
    QWidget *w = tab->widget(i);
    FMEditPane *te = qobject_cast<FMEditPane*>(w);
    if (te) {
      if (te->getFileName() == tname) {
	tab->setCurrentIndex(i);
	update();
	return;
      }
    }
  }
  if (currentEditor()->document()->isModified() ||
      (tab->tabText(tab->currentIndex()) != "untitled.m")) {
    tab->addTab(new FMEditPane(m_eval),"untitled.m");
    tab->setCurrentIndex(tab->count()-1);
    updateFont();
  }
  loadFile(tname);
  update();
}

void FMEditor::refreshContext() {
  if (!context) return;
  
  //Reset all the values
  varNameList = QStringList();
  varTypeList = QStringList();
  varFlagsList = QStringList();
  varSizeList = QStringList();
  varValueList = QStringList();

  StringVector varnames = StringVector(context->listAllVariables());
  std::sort(varnames.begin(),varnames.end());
  for (int i=0;i<varnames.size();i++) {
    QString name(QString::fromStdString(varnames[i]));
    QString type;
    QString flags;
    QString size;
    QString value;
    Array lookup;
    ArrayReference ptr;
    ptr = context->lookupVariable(varnames[i]);
    if (!ptr.valid()) {
      type = "undefined";
    } else {
      lookup = *ptr;
      Class t = lookup.dataClass();
      switch(t) {
      case FM_CELL_ARRAY:
	type = "cell";
	break;
      case FM_STRUCT_ARRAY:
	if (lookup.isUserClass())
	  type = QString::fromStdString(lookup.className().back());
	else
	  type = "struct";
	break;
      case FM_LOGICAL:
	type = "logical";
	break;
      case FM_UINT8:
	type = "uint8";
	break;
      case FM_INT8:
	type = "int8";
	break;
      case FM_UINT16:
	type = "uint16";
	break;
      case FM_INT16:
	type = "int16";
	break;
      case FM_UINT32:
	type = "uint32";
	break;
      case FM_INT32:
	type = "int32";
	break;
      case FM_UINT64:
	type = "uint64";
	break;
      case FM_INT64:
	type = "int64";
	break;
      case FM_FLOAT:
	type = "float";
	break;
      case FM_DOUBLE:
	type = "double";
	break;
      case FM_COMPLEX:
	type = "complex";
	break;
      case FM_DCOMPLEX:
	type = "dcomplex";
	break;
      case FM_STRING:
	type = "string";
	break;
      case FM_FUNCPTR_ARRAY:
	type = "func ptr";
	break;
      }
      if (lookup.sparse())
	flags = "Sparse ";
      if (context->isVariableGlobal(varnames[i])) {
	flags += "Global ";
      } else if (context->isVariablePersistent(varnames[i])) {
	flags += "Persistent ";
      }
      size = QString::fromStdString(lookup.dimensions().asString());
      try {
	value = QString::fromStdString(ArrayToPrintableString(lookup));
      } catch (Exception& e) {
      }
    }
    varNameList << name;
	varTypeList << type;
	varFlagsList << flags;
	varSizeList << size;
	varValueList << value;	    
  }
}

void FMEditor::setContext(Context *watch) {
  context = watch;
  refreshContext();
}

void FMEditor::showDataTips(QPoint pos, QString textSelected) {

  if (!isShowToolTip)
     return;
     
  bool foundTip = false;
  if (!textSelected.isEmpty()) {
    //split selected text into smaller parts and match with existing variable names
 	QStringList list = textSelected.split(QRegExp("\\W+"), QString::SkipEmptyParts);
	 for (int j = 0; j < list.size(); j++)
		for (int i = 0; i < varNameList.size(); i++)
		  if (list.at(j) == varNameList.at(i)) {
			foundTip = true;
			if (varValueList.at(i).isEmpty())
			  QToolTip::showText(pos, varNameList.at(i) + ": " + 
				                    varSizeList.at(i) + " " + 
				                    varTypeList.at(i));
			else
			  QToolTip::showText(pos, varNameList.at(i) + ": " + 
				                    varSizeList.at(i) + " " + 
				                    varTypeList.at(i) + " =\n    " + 
				                    varValueList.at(i) );
			break;
	      }
    if (!foundTip)
      QToolTip::hideText();
  }
  else
    QToolTip::hideText();
}

void FMEditor::closeTab() {
  if (maybeSave()) {
    QWidget *p = tab->currentWidget();
    tab->removeTab(tab->currentIndex());
    prevEdit = NULL;
    delete p;
  }
}

bool FMEditor::maybeSave() {
  if (currentEditor()->document()->isModified()) {
    int ret = QMessageBox::warning(this, tr("FreeMat"),
				   "The document " + shownName() + " has been modified.\n"
				   "Do you want to save your changes?",
				   QMessageBox::Yes | QMessageBox::Default,
				   QMessageBox::No,
				   QMessageBox::Cancel | QMessageBox::Escape);
    if (ret == QMessageBox::Yes)
      return save();
    else if (ret == QMessageBox::Cancel)
      return false;
  }
  return true;  
}

bool FMEditor::saveFile(const QString &fileName)
{
  QFile file(fileName);
  if (!file.open(QFile::WriteOnly | QFile::Text)) {
    QMessageBox::warning(this, tr("FreeMat"),
			 tr("Cannot write file %1:\n%2.")
			 .arg(fileName)
			 .arg(file.errorString()));
    return false;
  }
  
  QTextStream out(&file);
  QApplication::setOverrideCursor(Qt::WaitCursor);
  out << currentEditor()->toPlainText();
  QApplication::restoreOverrideCursor();
  
  setCurrentFile(fileName);
  statusBar()->showMessage(tr("File saved"), 2000);
  return true;
}

void FMEditor::closeEvent(QCloseEvent *event) {
  while (tab->count() > 0) {
    if (!maybeSave()) {
      event->ignore();
      return;
    } else {
      QWidget *p = tab->currentWidget();
      tab->removeTab(tab->currentIndex());
      prevEdit = NULL;
      delete p;
    }
  }
  writeSettings();
  event->accept();
}

QString FMEditor::getFullFileName(QString fname)
{
  QFileInfo info(fname);
  std::string fn = fname.toStdString();
  FuncPtr val;
  bool isFun = m_eval->getContext()->lookupFunction(fn,val);
  if (isFun && (val->type() == FM_M_FUNCTION)) {
    //if file is a matlab file get the file name from the interpreter
    MFunctionDef* mfun = (MFunctionDef*)val;
    if( mfun ) {
      fn = mfun->fileName;
      return QString::fromStdString(fn);
    }
    else {
	  return QString();
    }
  }
  else {
    bool isFound = false;
    for (int i=0;i<pathList.size();i++) {
      QDir pdir(pathList[i]);
      if (pdir.exists(fname)) {
        return pdir.absoluteFilePath(fname);
      }
    }
    if (!isFound) {
	  return QString();
	}
  }
}

void FMEditor::loadFile(const QString &fileName)
{
  // ignore if empty filename
  if (fileName.isEmpty())
    return;
    
  // check if filename contains line number
  QString fname = fileName;
/*    
  int lineNum = 0;
  int posPlusSign = fname.indexOf('+');
  if (posPlusSign > 0) {
    QString lineNumeSt = fname.mid(posPlusSign);
    lineNum = lineNumeSt.toInt();
    fname.remove(posPlusSign, fname.size()-posPlusSign);
  }
*/

  QString fn = getFullFileName(fname);
  if (fn.isEmpty()) {
       QMessageBox::warning(this, tr("FreeMat"),
	        tr("Cannot read file %1. No such file found on path list.")
	        .arg(fname));
    return;
  }
  else
    fname = fn;
    
  // Check for one of the editors that might be editing this file already
  // if found, show its tab
  for (int i=0;i<tab->count();i++) {
    QWidget *w = tab->widget(i);
    FMEditPane *te = qobject_cast<FMEditPane*>(w);
    if (te) {
      if (te->getFileName() == fname) {
	tab->setCurrentIndex(i);
	return;
      }
    }
  }

  // check if there's already an unmodified "untitled.m" tab
  // if not create a new tab
  if (tab->tabText(tab->currentIndex()) != "untitled.m") { 
    tab->addTab(new FMEditPane(m_eval),"untitled.m");
    tab->setCurrentIndex(tab->count()-1);
    updateFont();
  }

  // open file and load into editor
  QFile file(fname);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    QMessageBox::warning(this, tr("FreeMat"),
			 tr("Cannot read file %1:\n%2.")
			 .arg(fname)
			 .arg(file.errorString()));
    return;
  }
  QTextStream in(&file);
  QApplication::setOverrideCursor(Qt::WaitCursor);
  currentEditor()->setPlainText(in.readAll());
  QApplication::restoreOverrideCursor();
  
  // remember filename and update recent file list
  QFileInfo tokeep(fname);
  lastfile = tokeep.absolutePath();
  lastfile_set = true;
  setCurrentFile(fname);
  statusBar()->showMessage(tr("File loaded"), 2000);
  
//  if (lineNum>0)
//    emit gotoLineNumber(lineNum);
}

void FMEditor::setCurrentFile(const QString &fileName)
{
  setCurrentFilename(fileName);
  currentEditor()->document()->setModified(false);
  setWindowModified(false);
  updateTitles();
  

  QSettings settings("FreeMat", "Recent Files");
  QStringList files = settings.value("recentFileList").toStringList();
  files.removeAll(fileName);
  files.prepend(fileName);
  while (files.size() > MaxRecentFiles)
    files.removeLast();

  settings.setValue("recentFileList", files);

  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    FMEditor *tEditor = qobject_cast<FMEditor *>(widget);
    if (tEditor)
      tEditor->updateRecentFileActions();
  }
}

void FMEditor::updateRecentFileActions() 
{
 QSettings settings("FreeMat", "Recent Files");
 QStringList files = settings.value("recentFileList").toStringList();

 int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

 for (int i = 0; i < numRecentFiles; ++i) {
     QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
     recentFileActs[i]->setText(text);
     recentFileActs[i]->setData(files[i]);
     recentFileActs[i]->setVisible(true);
 }
 for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
     recentFileActs[j]->setVisible(false);

 separatorAct->setVisible(numRecentFiles > 0);
}


QString FMEditor::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void FMEditor::font() {
  bool ok;
  QFont new_font = QFontDialog::getFont(&ok, m_font, this);
  m_font = new_font;
  updateFont();
}

BreakPointIndicator::BreakPointIndicator(FMTextEdit *editor, FMEditPane* pane) : 
  QWidget(), tEditor(editor), tPane(pane) {
  setFixedWidth(fontMetrics().width(QLatin1String("0000")+5));
  connect(tEditor->document()->documentLayout(), 
	  SIGNAL(update(const QRectF &)),
	  this, SLOT(update()));
  connect(tEditor->verticalScrollBar(), SIGNAL(valueChanged(int)),
	  this, SLOT(update()));  
}
	
void BreakPointIndicator::mousePressEvent(QMouseEvent *e) {
  int contentsY = tEditor->verticalScrollBar()->value();
  qreal pageBottom = contentsY + tEditor->viewport()->height();
  int lineNumber = 1;
  QString fname(tPane->getFileName());
  Interpreter *eval(tPane->getInterpreter());
  for (QTextBlock block = tEditor->document()->begin();
       block.isValid(); block = block.next(), ++lineNumber) {
    QTextLayout *layout = block.layout();
    
    const QRectF boundingRect = layout->boundingRect();
    QPointF position = layout->position();
    if (position.y() + boundingRect.height() < contentsY)
      continue;
    if (position.y() > pageBottom)
      break;
    if ((e->y() >= (2+qRound(position.y()) - contentsY)) &&
	(e->y() < (2+qRound(position.y()) - contentsY) + width()/2)) {
      eval->toggleBP(fname,lineNumber);
    }
  }
}
						       
void BreakPointIndicator::paintEvent(QPaintEvent *) {
  int contentsY = tEditor->verticalScrollBar()->value();
  qreal pageBottom = contentsY + tEditor->viewport()->height();
  int lineNumber = 1;
  QPainter p(this);
  // Get the list of breakpoints
  QString fname(tPane->getFileName());
  Interpreter *eval(tPane->getInterpreter());
  int w = width()/2;
  for (QTextBlock block = tEditor->document()->begin();
       block.isValid(); block = block.next(), ++lineNumber) {
    QTextLayout *layout = block.layout();
    const QRectF boundingRect = layout->boundingRect();
    QPointF position = layout->position();
    if (position.y() + boundingRect.height() < contentsY)
      continue;
    if (position.y() > pageBottom)
      break;    
    if (eval->isBPSet(fname,lineNumber))
      p.drawPixmap(2, 2+qRound(position.y()) - contentsY, 
		   w-4,w-4,QPixmap(":/images/stop.png"),
		   0,0,32,32);
    if (eval->isInstructionPointer(fname,lineNumber))
      p.drawPixmap(w, 2+qRound(position.y()) - contentsY, 
		   w-4,w-4,QPixmap(":/images/player_play.png"),
		   0,0,32,32);
  }
}

LineNumber::LineNumber(FMTextEdit *editor) : QWidget(), tEditor(editor) {
  setFixedWidth(fontMetrics().width(QLatin1String("0000")+5));
  connect(tEditor->document()->documentLayout(), 
	  SIGNAL(update(const QRectF &)),
	  this, SLOT(update()));
  connect(tEditor->verticalScrollBar(), SIGNAL(valueChanged(int)),
	  this, SLOT(update()));
}

void LineNumber::paintEvent(QPaintEvent *)
{
  int contentsY = tEditor->verticalScrollBar()->value();
  qreal pageBottom = contentsY + tEditor->viewport()->height();
  int lineNumber = 1;
  setFont(tEditor->font());
  const QFontMetrics fm = fontMetrics();
  const int ascent = fontMetrics().ascent() + 1; // height = ascent + descent + 1
  
  QPainter p(this);
  for (QTextBlock block = tEditor->document()->begin();
       block.isValid(); block = block.next(), ++lineNumber) {
    QTextLayout *layout = block.layout();
    
    const QRectF boundingRect = layout->boundingRect();
    QPointF position = layout->position();
    if (position.y() + boundingRect.height() < contentsY)
      continue;
    if (position.y() > pageBottom)
      break;
    
    const QString txt = QString::number(lineNumber);
    p.drawText(width() - fm.width(txt), qRound(position.y()) - contentsY + ascent, txt);
  }
}

FMSynLightConf::FMSynLightConf(QWidget *parent) : QDialog(parent) {
  ui.setupUi(this);
  QSettings settings("FreeMat","FreeMat");
  connect(ui.enableCB,SIGNAL(toggled(bool)),ui.keywordButton,SLOT(setEnabled(bool)));
  connect(ui.enableCB,SIGNAL(toggled(bool)),ui.commentsButton,SLOT(setEnabled(bool)));
  connect(ui.enableCB,SIGNAL(toggled(bool)),ui.stringsButton,SLOT(setEnabled(bool)));
  connect(ui.enableCB,SIGNAL(toggled(bool)),ui.untermStringButton,SLOT(setEnabled(bool)));
  ui.enableCB->setChecked(settings.value("editor/syntax_enable",true).toBool());
  setLabelColor(ui.keywordLabel,
		settings.value("editor/syntax_colors/keyword",Qt::darkBlue).value<QColor>());
  setLabelColor(ui.commentsLabel,
		settings.value("editor/syntax_colors/comments",Qt::darkRed).value<QColor>());
  setLabelColor(ui.stringsLabel,
		settings.value("editor/syntax_colors/strings",Qt::darkGreen).value<QColor>());
  setLabelColor(ui.untermStringsLabel,
		settings.value("editor/syntax_colors/untermstrings",Qt::darkRed).value<QColor>());
  connect(ui.keywordButton,SIGNAL(clicked()),this,SLOT(setKeywordColor()));
  connect(ui.commentsButton,SIGNAL(clicked()),this,SLOT(setCommentColor()));
  connect(ui.stringsButton,SIGNAL(clicked()),this,SLOT(setStringColor()));
  connect(ui.untermStringButton,SIGNAL(clicked()),this,SLOT(setUntermStringColor()));
  connect(this,SIGNAL(accepted()),this,SLOT(save()));
}

void FMSynLightConf::save() {
  QSettings settings("FreeMat", "FreeMat");
  settings.setValue("editor/syntax_enable",ui.enableCB->isChecked());
  settings.setValue("editor/syntax_colors/keyword",getColor(ui.keywordLabel));
  settings.setValue("editor/syntax_colors/comments",getColor(ui.commentsLabel));
  settings.setValue("editor/syntax_colors/strings",getColor(ui.stringsLabel));
  settings.setValue("editor/syntax_colors/untermstrings",getColor(ui.untermStringsLabel));
  QMessageBox::information(this,"Updated settings","The settings will apply to new files you open in the editor.");
}

QColor FMSynLightConf::getColor(QLabel *l) {
  return (l->palette().color(QPalette::WindowText));
}

void FMSynLightConf::setLabelColor(QLabel *l, QColor c) {
  if (c.isValid()) {
    QPalette palette(l->palette());
    palette.setColor(QPalette::WindowText, c);
    l->setPalette(palette);
  }
}

void FMSynLightConf::querySetLabelColor(QLabel *l) {
  setLabelColor(l,QColorDialog::getColor(l->palette().color(QPalette::WindowText), this));
}

void FMSynLightConf::setKeywordColor() {
  querySetLabelColor(ui.keywordLabel);
}

void FMSynLightConf::setCommentColor() {
  querySetLabelColor(ui.commentsLabel);
}

void FMSynLightConf::setStringColor() {
  querySetLabelColor(ui.stringsLabel);
}

void FMSynLightConf::setUntermStringColor() {
  querySetLabelColor(ui.untermStringsLabel);
}

FMIndentConf::FMIndentConf(QWidget *parent) : QDialog(parent) {
  ui.setupUi(this);
  QSettings settings("FreeMat","FreeMat");
  ui.indentEnable->setChecked(settings.value("editor/indent_enable",true).toBool());
  ui.tabsize->setText(settings.value("editor/tab_size",4).toString());
  connect(this,SIGNAL(accepted()),this,SLOT(save()));
}

void FMIndentConf::save() {
  QSettings settings("FreeMat","FreeMat");
  settings.setValue("editor/indent_enable",ui.indentEnable->isChecked());
  settings.setValue("editor/tab_size",ui.tabsize->text().toInt());
  QMessageBox::information(this,"Updated settings","The settings will apply to new files you open in the editor.");
}


