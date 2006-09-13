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
#ifndef __Editor_hpp__
#define __Editor_hpp__

#include <QTextDocument>
#include <QTextEdit>
#include <QMainWindow>
#include <QTabWidget>
#include <QLabel>
#include <QComboBox>
#include <QDialog>
#include <QCheckBox>
#include <QGroupBox>
#include "highlighter.hpp"

class FMFindDialog : public QDialog {
  Q_OBJECT
public:
  FMFindDialog(QWidget *parent = 0);
signals:
  void doFind(QString text, bool backwards, bool sensitive);
private slots:
  void find();
private:
  QLabel *label;
  QComboBox *combo;
  QCheckBox *caseCheckBox;
  QCheckBox *backwardCheckBox;
  QPushButton *findButton;
  QPushButton *closeButton;
  QGroupBox *buttonGroup;
  QLabel *status;
};

class FMTextEdit : public QTextEdit {
  Q_OBJECT
public:
  FMTextEdit();
  virtual ~FMTextEdit();
  void keyPressEvent(QKeyEvent*e);
  void comment();
  void uncomment();
signals:
  void indent();
};

class FMIndent : public QObject {
  Q_OBJECT
  FMTextEdit *m_te;
public:
  FMIndent();
  virtual ~FMIndent();
  void setDocument(FMTextEdit *te);
  FMTextEdit *document() const;
private slots:
  void update();
};

class LineNumber : public QWidget {
  Q_OBJECT
public:
  LineNumber(FMTextEdit *editor);
protected:
  virtual void paintEvent(QPaintEvent *);
private:
  FMTextEdit *tEditor;
};

class FMEditPane : public QWidget {
  Q_OBJECT
  FMTextEdit *tEditor;
  QString curFile;
public:
  FMEditPane();
  FMTextEdit* getEditor();
  void setFileName(QString filename);
  QString getFileName();
};

class FMEditor : public QMainWindow {
  Q_OBJECT
  QMenu *fileMenu, *editMenu, *toolsMenu;
  QToolBar *editToolBar, *fileToolBar;
  QAction *newAct, *saveAct, *quitAct, *copyAct, *pasteAct;
  QAction *cutAct, *fontAct, *openAct, *saveAsAct, *closeAct;
  QAction *openNewAct, *findAct, *commentAct, *uncommentAct;
  QTabWidget *tab;
  FMTextEdit *prevEdit;
  QFont m_font;
  FMFindDialog *m_find;
  QMenu *m_popup;
public:
  FMEditor();
  virtual ~FMEditor();
private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  bool maybeSave();
  bool saveFile(const QString& filename);
  void loadFile(const QString& filename);
  void setCurrentFile(const QString& filename);
  QString strippedName(const QString& fullfilename);
  FMTextEdit *currentEditor();
  void setCurrentFilename(QString filename);
  QString currentFilename();
  QString shownName();
  void updateTitles();
  void readSettings();
  void writeSettings();
  void updateFont();
protected:
  void contextMenuEvent(QContextMenuEvent *e);
private slots:
  bool save();
  bool saveAs();
  void open();
  void font();
  void addTab();
  void closeTab();
  void tabChanged(int);
  void documentWasModified();
  void find();
  void doFind(QString text, bool backwards, bool sensitive);
  void comment();
  void uncomment();
public:
  void closeEvent(QCloseEvent *event);
};

#endif
