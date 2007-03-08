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

FMFindDialog::FMFindDialog(QWidget *parent) : QDialog(parent) {
  ui.setupUi(this);
  connect(ui.btFind,SIGNAL(clicked()),this,SLOT(find()));
  connect(ui.btClose,SIGNAL(clicked()),this,SLOT(hide()));
  setWindowIcon(QIcon(QString::fromUtf8(":/images/freemat_small_mod_64.png")));
  setWindowTitle("Find - " + QString::fromStdString(Interpreter::getVersionString()));
  ui.btFind->setIcon(QIcon(QString::fromUtf8(":/images/find.png")));
  ui.btClose->setIcon(QIcon(QString::fromUtf8(":/images/close.png")));
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
  setLineWrapMode(QTextEdit::NoWrap);
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
  line2.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
  QTextCursor pos(line1);
  pos.beginEditBlock();
  while (pos.position() <= line2.position()) {
    pos.insertText("%");
    pos.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor);
  }
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
  line2.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
  QTextCursor pos(line1);
  pos.beginEditBlock();
  while (pos.position() <= line2.position()) {
    pos.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor);
    if (pos.selectedText() == "%")
      pos.deleteChar();
    pos.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor);
    pos.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
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
      emit indent();
    }
    if (key == 13) {
      delayedIndent = true;
      emit indent();
    }
  }
  if (!tab)
    QTextEdit::keyPressEvent(e);
  else
    e->accept();
  if (delayedIndent) 
    emit indent();
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
  QRegExp keyword_in("\\b(if|for|function|try|while|switch)\\b");
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
  leading_space += indent_increment*3;
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
  QString indented(indentLine(toIndent,priorlines));
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
}

void FMEditor::updateFont() {
  for (int i=0;i<tab->count();i++) {
    QWidget *p = tab->widget(i);
    FMEditPane *te = qobject_cast<FMEditPane*>(p);
    te->setFont(m_font);
  }
}

void FMEditor::writeSettings() {
  QSettings settings("FreeMat", "FreeMat");
  settings.setValue("editor/pos", pos());
  settings.setValue("editor/size", size());
  settings.setValue("editor/font", m_font.toString());
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
  // Disconnect each of the contents changed signals
  if (prevEdit)
    disconnect(prevEdit->document(),SIGNAL(contentsChanged()),0,0);
  // NEED TO DISCONNECT...
  connect(currentEditor()->document(),SIGNAL(contentsChanged()),this,SLOT(documentWasModified()));
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
  connect(saveAsAct,SIGNAL(triggered()),this,SLOT(saveAs()));
  quitAct = new QAction(QIcon(":/images/quit.png"),"&Quit Editor",this);
  quitAct->setShortcut(Qt::Key_Q | Qt::CTRL);
  connect(quitAct,SIGNAL(triggered()),this,SLOT(close()));
  closeAct = new QAction(QIcon(":/images/close.png"),"&Close Tab",this);
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
  connect(commentAct,SIGNAL(triggered()),this,SLOT(comment()));
  uncommentAct = new QAction("Uncomment Region",this);
  connect(uncommentAct,SIGNAL(triggered()),this,SLOT(uncomment()));
  replaceAct = new QAction("Find and Replace",this);
  connect(replaceAct,SIGNAL(triggered()),this,SLOT(replace()));
  dbStepAct = new QAction(QIcon(":/images/dbgnext.png"),"&Step Over",this);
  connect(dbStepAct,SIGNAL(triggered()),this,SLOT(dbstep()));
  dbTraceAct = new QAction(QIcon(":/images/dbgstep.png"),"&Step Into",this);
  connect(dbTraceAct,SIGNAL(triggered()),this,SLOT(dbtrace()));
  dbContinueAct = new QAction(QIcon(":/images/dbgrun.png"),"&Continue",this);
  connect(dbContinueAct,SIGNAL(triggered()),this,SLOT(dbcontinue()));
  dbSetClearBPAct = new QAction(QIcon(":/images/stop.png"),"Set/Clear Breakpoint",this);
  connect(dbSetClearBPAct,SIGNAL(triggered()),this,SLOT(dbsetclearbp()));
  dbStopAct = new QAction(QIcon(":/images/player_stop.png"),"Stop Debugging",this);
  connect(dbStopAct,SIGNAL(triggered()),this,SLOT(dbstop()));
}

void FMEditor::comment() {
  currentEditor()->comment();
}

void FMEditor::uncomment() {
  currentEditor()->uncomment();
}

void FMEditor::find() {
  m_find->show();
  m_find->raise();
}

void FMEditor::replace() {
  m_replace->show();
  m_replace->raise();
}

void FMEditor::createMenus() {
  fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction(newAct);
  fileMenu->addAction(openAct);
  fileMenu->addAction(saveAct);
  fileMenu->addAction(saveAsAct);
  fileMenu->addAction(closeAct);
  fileMenu->addAction(quitAct);
  editMenu = menuBar()->addMenu("&Edit");
  editMenu->addAction(copyAct);
  editMenu->addAction(cutAct);
  editMenu->addAction(pasteAct);
  editMenu->addAction(fontAct);
  toolsMenu = menuBar()->addMenu("&Tools");
  toolsMenu->addAction(findAct);
  toolsMenu->addAction(replaceAct);
  toolsMenu->addAction(commentAct);
  toolsMenu->addAction(uncommentAct);
  debugMenu = menuBar()->addMenu("&Debug");
  debugMenu->addAction(dbStepAct);
  debugMenu->addAction(dbTraceAct);
  debugMenu->addAction(dbContinueAct);
  debugMenu->addAction(dbSetClearBPAct);
  debugMenu->addAction(dbStopAct);
  m_popup = new QMenu;
  m_popup->addAction(copyAct);
  m_popup->addAction(cutAct);
  m_popup->addAction(pasteAct);
  m_popup->addSeparator();
  m_popup->addAction(findAct);
  m_popup->addAction(replaceAct);
  m_popup->addSeparator();
  m_popup->addAction(commentAct);
  m_popup->addAction(uncommentAct);
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
  debugToolBar->addAction(dbStepAct);
  debugToolBar->addAction(dbTraceAct);
  debugToolBar->addAction(dbContinueAct);
  debugToolBar->addAction(dbSetClearBPAct);
  debugToolBar->addAction(dbStopAct);
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

static QString GetOpenFileName(QWidget *w) {
  QString retfile;
  if (lastfile_set)
    retfile = QFileDialog::getOpenFileName(w,"Open File in Editor",lastfile,
					   "M files (*.m);;Text files (*.txt);;All files (*)");
  else
    retfile = QFileDialog::getOpenFileName(w,"Open File in Editor",QString(),
					   "M files (*.m);;Text files (*.txt);;All files (*)");
  if (!retfile.isEmpty()) {
    QFileInfo tokeep(retfile);
    lastfile = tokeep.absolutePath();
    lastfile_set = true;
  }
  return retfile;
}

static QString GetSaveFileName(QWidget *w) {
  QString retfile;
  if (lastfile_set)
    retfile = QFileDialog::getSaveFileName(w,"Save File",lastfile,
					   "M files (*.m);;Text files (*.txt);;All files (*)");
  else
    retfile = QFileDialog::getSaveFileName(w,"Save File",QString(),
					   "M files (*.m);;Text files (*.txt);;All files (*)");
  if (!retfile.isEmpty()) {
    QFileInfo tokeep(retfile);
    lastfile = tokeep.absolutePath();
    lastfile_set = true;
  }
  return retfile;  
}

void FMEditor::open() {
  if (currentEditor()->document()->isModified() ||
      (tab->tabText(tab->currentIndex()) != "untitled.m")) {
    tab->addTab(new FMEditPane(m_eval),"untitled.m");
    tab->setCurrentIndex(tab->count()-1);
    updateFont();
  }
  QString fileName = GetOpenFileName(this);
  if (!fileName.isEmpty()) {
    loadFile(fileName);
  }
}

bool FMEditor::save() {
  if (currentFilename().isEmpty()) {
    return saveAs();
  } else {
    return saveFile(currentFilename());
  }
}

bool FMEditor::saveAs() {
  QString fileName = GetSaveFileName(this);
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

void FMEditor::loadFile(const QString &fileName)
{
  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    QMessageBox::warning(this, tr("FreeMat"),
			 tr("Cannot read file %1:\n%2.")
			 .arg(fileName)
			 .arg(file.errorString()));
    return;
  }
  
  // Check for one of the editors that might be editing this file already
  for (int i=0;i<tab->count();i++) {
    QWidget *w = tab->widget(i);
    FMEditPane *te = qobject_cast<FMEditPane*>(w);
    if (te) {
      if (te->getFileName() == fileName) {
	tab->setCurrentIndex(i);
	return;
      }
    }
  }

  QTextStream in(&file);
  QApplication::setOverrideCursor(Qt::WaitCursor);
  currentEditor()->setPlainText(in.readAll());
  QApplication::restoreOverrideCursor();
  
  setCurrentFile(fileName);
  statusBar()->showMessage(tr("File loaded"), 2000);
}

void FMEditor::setCurrentFile(const QString &fileName)
{
  setCurrentFilename(fileName);
  currentEditor()->document()->setModified(false);
  setWindowModified(false);
  updateTitles();
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

