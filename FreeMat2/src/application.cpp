#include "application.h"
#include <qapplication.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qfiledialog.h>
#include <qclipboard.h>
#include <iostream>
#include <qfontdialog.h>
#include <qsettings.h>
#include "WalkTree.hpp"
#include "highlighter.hpp"
#include <QtGui>

#define MAKEASCII(x) x.toAscii().constData()
#include <QtGui>

#include "filesave.xpm"
#include "../libs/libXP/freemat-2.xpm"

ApplicationWindow::~ApplicationWindow() {
}

void ApplicationWindow::createActions() {
  saveAct = new QAction("&Save Transcript",this);
  connect(saveAct,SIGNAL(triggered()),this,SLOT(save()));
  quitAct = new QAction("&Quit",this);
  connect(quitAct,SIGNAL(triggered()),this,SLOT(close()));
  copyAct = new QAction("&Copy",this);
  connect(copyAct,SIGNAL(triggered()),this,SLOT(copy()));
  pasteAct = new QAction("&Paste",this);
  connect(pasteAct,SIGNAL(triggered()),this,SLOT(paste()));
  fontAct = new QAction("&Font",this);
  connect(fontAct,SIGNAL(triggered()),this,SLOT(font()));
  aboutAct = new QAction("&About",this);
  connect(aboutAct,SIGNAL(triggered()),this,SLOT(about()));
  manualAct = new QAction("Online &Manual",this);
  connect(manualAct,SIGNAL(triggered()),this,SLOT(manual()));
  aboutQt = new QAction("About &Qt",this);
  connect(aboutQt,SIGNAL(triggered()),qApp,SLOT(aboutQt()));
}

void ApplicationWindow::createMenus() {
  fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction(saveAct);
  fileMenu->addAction(quitAct);
  editMenu = menuBar()->addMenu("&Edit");
  editMenu->addAction(copyAct);
  editMenu->addAction(pasteAct);
  editMenu->addAction(fontAct);
  helpMenu = menuBar()->addMenu("&Help");
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(manualAct);
  helpMenu->addAction(aboutQt);
}

void ApplicationWindow::createToolBars() {
  editToolBar = addToolBar("Edit");
  editToolBar->addAction(copyAct);
  editToolBar->addAction(pasteAct);
}

void ApplicationWindow::createStatusBar() {
  statusBar()->showMessage("Ready");
}

ApplicationWindow::ApplicationWindow() : QMainWindow() {
  QPixmap myIcon = QPixmap(freemat_2);
  setWindowIcon(myIcon);
  setWindowTitle(QString(WalkTree::getVersionString().c_str()) + " Command Window");
  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
}

void ApplicationWindow::closeEvent(QCloseEvent* ce) {
  ce->accept();
  exit(0);
  return;
}

void ApplicationWindow::SetGUITerminal(GUITerminal* term) {
  m_term = term;
  setCentralWidget(term);
  setMinimumSize(400,300);
  QSettings settings("FreeMat","FreeMat");
  QString font = settings.value("terminal/font").toString();
  if (!font.isNull()) {
    QFont new_font;
    new_font.fromString(font);
    m_term->setFont(new_font);
  }
  term->show();
}

void ApplicationWindow::save() {
  QString fn = QFileDialog::getSaveFileName();
  if (!fn.isEmpty()) {
    FILE *fp;
    fp = fopen(MAKEASCII(fn),"w");
    if (!fp) {
      char buffer[1000];
      sprintf(buffer,"Unable to save transcript to file %s!",
	      MAKEASCII(fn));
      QMessageBox::information(this,"Error",buffer,
			       QMessageBox::Ok);
      return;
    }
    int history_count;
    int width;
    char *textbuffer;
    textbuffer = m_term->getTextSurface(history_count, width);
    char *linebuf = new char[width+1];
    for (int i=0;i<history_count;i++) {
      // scan backwards for last non ' ' char
      int j=width-1;
      while ((j>0) && (textbuffer[i*width+j] == ' '))
	j--;
      j++;
      memcpy(linebuf,textbuffer+i*width,j*sizeof(char));
      linebuf[j] = 0;
#ifdef WIN32
      fprintf(fp,"%s\r\n",linebuf);
#else
      fprintf(fp,"%s\n",linebuf);
#endif
    }
    fclose(fp);
  }
}

void ApplicationWindow::copy() {
  char *copytextbuf = m_term->getSelectionText();
  if (!copytextbuf) return;
  QClipboard *cb = QApplication::clipboard();
  cb->setText(copytextbuf, QClipboard::Clipboard);
  free(copytextbuf);
}

void ApplicationWindow::paste() {
  QClipboard *cb = QApplication::clipboard();
  QString text;
  if (cb->supportsSelection())
    text = cb->text(QClipboard::Selection);
  if (text.isNull())
    text = cb->text(QClipboard::Clipboard);
  if (!text.isNull()) {
    const char *cp = MAKEASCII(text);
    while (*cp) 
      m_term->ProcessChar(*cp++);
  }
}

void ApplicationWindow::font() {
  QFont old_font = m_term->getFont();
  bool ok;
  QFont new_font = QFontDialog::getFont(&ok, old_font, this);
  if (ok) {
    QSettings settings("FreeMat","FreeMat");
    settings.setValue("terminal/font",new_font.toString());
    m_term->setFont(new_font);
  }
}

FMTextEdit::FMTextEdit() : QTextEdit() {
  setLineWrapMode(QTextEdit::NoWrap);
}

FMTextEdit::~FMTextEdit() {
}

void FMTextEdit::keyPressEvent(QKeyEvent*e) {
  bool tab = false;
  int keycode = e->key();
  if (keycode) {
    QByteArray p(e->text().toAscii());
    char key;
    if (!e->text().isEmpty())
      key = p[0];
    else
      key = 0;
    if (key == 0x09) {
      tab = true;
      emit indent();
    }
  }
  if (!tab)
    QTextEdit::keyPressEvent(e);
  else
    e->ignore();
}


FMIndent::FMIndent() {
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
  QRegExp keyword_in("\\b(if|else|for|function|try|catch|while|switch)\\b");
  QRegExp keyword_out("\\bend\\b");
  int indent_in_count = countMatches(a,keyword_in);
  int indent_out_count = countMatches(a,keyword_out);
  int indent_increment = indent_in_count - indent_out_count;
  return indent_increment;
}

QString indentLine(QString toIndent, QStringList priorText) {
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
  // Adjust this for the case that our current line is an "end" command
  QString b = stripLine(toIndent);
  QRegExp end_only("^\\s*\\bend\\b");
  if (b.indexOf(end_only) >= 0)
    indent_increment--;
  // 6.  Find the start of non-white text in <a>, add incremental index*tab size - this 
  //     is the amount of white space at the beginning of the current line.
  QRegExp whitespace("^\\s*");
  int leading_space = 0;
  int k = 0;
  if ((k = a.indexOf(whitespace,0)) != -1)
    leading_space = whitespace.matchedLength();
  leading_space += indent_increment*3;
  leading_space = qMax(leading_space,0);
  // Indenting is simpler for us than for QSA or for C++.  We simply
  // apply the following rules:
  // 7.  Adjust the current line.
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
  cursor.movePosition(QTextCursor::Up);
  cursor.movePosition(QTextCursor::EndOfLine);
  cursor.movePosition(QTextCursor::Start,QTextCursor::KeepAnchor);
  QStringList priorlines(cursor.selection().toPlainText().split("\n"));
  QString indented(indentLine(toIndent,priorlines));
  save.movePosition(QTextCursor::StartOfLine);
  save.movePosition(QTextCursor::EndOfLine,QTextCursor::KeepAnchor);
  save.insertText(indented);
  // Move the cursor to where it was relative to the original text
  // The number of characters inserted 
  int orig_length = toIndent.length();
  int new_length = indented.length();
  int new_pos = qMax(curpos + new_length-orig_length,0);
  final.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,new_pos);
  m_te->setTextCursor(final);
}

void ApplicationWindow::about() {
  FMIndent *ind = new FMIndent;
  FMTextEdit* t = new FMTextEdit;

  connect(t,SIGNAL(indent()),ind,SLOT(update()));
  Highlighter *n = new Highlighter(t->document());
  ind->setDocument(t);
  t->show();
}

void ApplicationWindow::manual() {
  emit startHelp();
}

void ApplicationWindow::editor() {
}
