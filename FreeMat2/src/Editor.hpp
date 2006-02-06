#ifndef __Editor_hpp__
#define __Editor_hpp__

#include <QTextDocument>
#include <QTextEdit>
#include <QMainWindow>
#include <QTabWidget>
#include "highlighter.hpp"

class FMTextEdit : public QTextEdit {
  Q_OBJECT
public:
  FMTextEdit();
  virtual ~FMTextEdit();
  void keyPressEvent(QKeyEvent*e);
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
  QMenu *fileMenu, *editMenu;
  QToolBar *editToolBar, *fileToolBar;
  QAction *newAct, *saveAct, *quitAct, *copyAct, *pasteAct, *fontAct, *openAct, *saveAsAct, *closeAct;
  QTabWidget *tab;
  FMTextEdit *prevEdit;
  QFont m_font;
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
private slots:
  bool save();
  bool saveAs();
  void open();
  void font();
  void addTab();
  void closeTab();
  void tabChanged(int);
  void documentWasModified();
public:
  void closeEvent(QCloseEvent *event);
};

#endif
