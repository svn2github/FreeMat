#include "Editor.hpp"
#include "WalkTree.hpp"
#include "highlighter.hpp"
#include <QtGui>

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
    e->accept();
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
  final.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
  final.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,new_pos);
  m_te->setTextCursor(final);
}

FMEditPane::FMEditPane() : QWidget() {
  tEditor = new FMTextEdit;
  LineNumber *tLN = new LineNumber(tEditor);
  QHBoxLayout *layout = new QHBoxLayout;
  layout->addWidget(tLN);
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

FMEditor::FMEditor() : QMainWindow() {
  setWindowIcon(QPixmap(":/images/freemat-2.xpm"));
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
}

void FMEditor::readSettings() {
  QSettings settings("FreeMat", "FreeMat");
  QPoint pos = settings.value("editor/pos", QPoint(200, 200)).toPoint();
  QSize size = settings.value("editor/size", QSize(400, 400)).toSize();
  resize(size);
  move(pos);
  QString font = settings.value("terminal/font").toString();
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
  tab->addTab(new FMEditPane,"untitled.m");
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
  setWindowTitle(QString("%1[*]").arg(shownName()) + " - " + QString::fromStdString(FreeMat::WalkTree::getVersionString()) + " Editor");
  documentWasModified();
}


void FMEditor::tabChanged(int newslot) {
  disconnect(copyAct,SIGNAL(triggered()),0,0);
  disconnect(copyAct,SIGNAL(triggered()),0,0);
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
  newAct = new QAction("&New Tab",this);
  connect(newAct,SIGNAL(triggered()),this,SLOT(addTab()));
  openAct = new QAction("&Open",this);
  connect(openAct,SIGNAL(triggered()),this,SLOT(open()));
  saveAct = new QAction("&Save",this);
  connect(saveAct,SIGNAL(triggered()),this,SLOT(save()));
  saveAsAct = new QAction("Save &As",this);
  connect(saveAsAct,SIGNAL(triggered()),this,SLOT(saveAs()));
  quitAct = new QAction("&Quit Editor",this);
  connect(quitAct,SIGNAL(triggered()),this,SLOT(close()));
  closeAct = new QAction("&Close Tab",this);
  connect(closeAct,SIGNAL(triggered()),this,SLOT(closeTab()));
  copyAct = new QAction("&Copy",this);
  pasteAct = new QAction("&Paste",this);
  fontAct = new QAction("&Font",this);
  connect(fontAct,SIGNAL(triggered()),this,SLOT(font()));
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
  editMenu->addAction(pasteAct);
  editMenu->addAction(fontAct);
}

void FMEditor::createToolBars() {
  fileToolBar = addToolBar("File");
  fileToolBar->addAction(newAct);
  fileToolBar->addAction(openAct);
  fileToolBar->addAction(saveAct);
  fileToolBar->addAction(closeAct);
  editToolBar = addToolBar("Edit");
  editToolBar->addAction(copyAct);
  editToolBar->addAction(pasteAct);
}

void FMEditor::createStatusBar() {
  statusBar()->showMessage("Ready");
}

void FMEditor::open() {
  if (maybeSave()) {
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
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
  QString fileName = QFileDialog::getSaveFileName(this);
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

LineNumber::LineNumber(FMTextEdit *editor) : QWidget(), tEditor(editor) {
  setFixedWidth(fontMetrics().width(QLatin1String("0000")+5));
  connect(tEditor->document()->documentLayout(), SIGNAL(update(const QRectF &)),
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

