#include <QApplication>
#include <QTextEdit>
#include <QTextStream>
#include <QPushButton>
#include <QVBoxLayout>
#include <QtDebug>
#include <QDir>
#include "KeyManager.hpp"
#include "Module.hpp"
#include "Class.hpp"
#include "LoadCore.hpp"
#include "LoadFN.hpp"
#include "GraphicsCore.hpp"
#include "File.hpp"
#include "Context.hpp"

QTextEdit *m_text;

QString output;
QStringList input;

void OutputText(QString str) {
  m_text->insertPlainText(str);
  //  m_text->setCursor(m_text->textCursor().movePosition(QCursor::End));
  m_text->ensureCursorVisible();
  qDebug() << str;
}


namespace FreeMat {
  class HelpTerminal : public KeyManager {
  public:
    HelpTerminal() {}
    virtual ~HelpTerminal() {}
    virtual void Initialize() {}
    virtual void RestoreOriginalMode() {}
    virtual void OutputRawString(std::string txt) {
      output += txt.c_str();
    }
    virtual void ResizeEvent() {}
    virtual void MoveDown() {
      output += "\n";
    }
    virtual char* getLine(std::string aprompt) {
      if (input.empty()) return 0;
      QString txt(input[0]);
      input.removeFirst();
      char *rettxt = strdup(qPrintable(txt));
      if (!input.empty())
	output += aprompt.c_str() + txt;
      return (strdup(qPrintable(txt)));      
    }
    virtual void MoveUp() {};
    virtual void MoveRight() {};
    virtual void MoveLeft() {};
    virtual void ClearEOL() {};
    virtual void ClearEOD() {};
    virtual void MoveBOL() {};
    virtual int getTerminalWidth() {return 80;}
  };
};

HelpTerminal *m_term;
using namespace FreeMat;
Context *context;

WalkTree* GetInterpreter() {
  m_term = new HelpTerminal;
  LoadModuleFunctions(context);
  LoadClassFunction(context);
  LoadCoreFunctions(context);
  LoadFNFunctions(context);
  LoadGraphicsCoreFunctions(context);  
  InitializeFigureSubsystem();
  const char *envPtr;
  envPtr = getenv("FREEMAT_PATH");
  m_term->setContext(context);
  if (envPtr)
    m_term->setPath(std::string(envPtr));
  else 
    m_term->setPath(std::string(""));
  m_term->setPath("../../MFiles");
  WalkTree *twalk = new WalkTree(context,m_term);
  return twalk;
}

QString EvaluateCommands(QStringList cmds) {
  input = cmds;
  WalkTree* twalk = GetInterpreter();
  try {
    while (!input.empty()) {
      try {
	twalk->evalCLI();
      } catch (WalkTreeRetallException) {
	twalk->clearStacks();
      } catch (WalkTreeReturnException &e) {
      } catch (WalkTreeQuitException) {
      }
    }
  } catch (WalkTreeQuitException &e) {
  } catch (std::exception& e) {
    std::cout << "Exception caught: " << e.what() << "\n";
  }
  delete twalk;
  delete m_term;
  return output;
}

void GUISetup() {
  QWidget *m_main = new QWidget;
  m_text = new QTextEdit;
  m_text->setReadOnly(true);
  m_text->resize(400,400);
  m_text->show();
  m_text->setFontFamily("Courier");
  QPushButton *quit = new QPushButton("Quit");
  QWidget::connect(quit,SIGNAL(clicked()),qApp,SLOT(quit()));
  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(m_text);
  layout->addWidget(quit);
  m_main->setLayout(layout);
  m_main->show();
}

void Halt(QString emsg) {
  OutputText(emsg);
  QEventLoop m_loop;
  m_loop.exec();  
  exit(0);
}

QString MustMatch(QRegExp re, QString source) {
  if (re.indexIn(source) < 0)
    Halt("Bad line: " + source);
  return re.cap(1);
}

bool TestMatch(QRegExp re, QString source) {
  return (re.indexIn(source) >= 0);
}

void CloseAllHandleWindows() {
  
}

void ProcessFile(QFileInfo fileinfo) {
  QRegExp docblock_pattern("^\\s*//!");
  QRegExp modulename_pattern("^\\s*//@Module\\s*(\\b\\w+\\b)");
  QRegExp moduledesc_pattern("^\\s*//@Module\\s*(\\b.*)");
  QRegExp sectioname_pattern("^\\s*//@@Section\\s*(\\b\\w+\\b)");
  QRegExp groupname_pattern("^\\s*//@@(.*)");
  QRegExp execin_pattern("^\\s*//@<");
  QRegExp execout_pattern("^\\s*//@>");
  QRegExp ccomment_pattern("^\\s*//(.*)");
  
  context = new Context;
  modulename_pattern.setCaseSensitivity(Qt::CaseInsensitive);
  moduledesc_pattern.setCaseSensitivity(Qt::CaseInsensitive);
  sectioname_pattern.setCaseSensitivity(Qt::CaseInsensitive);
  groupname_pattern.setCaseSensitivity(Qt::CaseInsensitive);
  if (fileinfo.suffix() == "mpp") {
    OutputText("Processing File " + fileinfo.absoluteFilePath() + "...\n");
    QFile file(fileinfo.absoluteFilePath());
    if (file.open(QFile::ReadOnly)) {
      QTextStream fstr(&file);
      while (!fstr.atEnd()) {
	QString line(fstr.readLine(0));
	if (TestMatch(docblock_pattern,line)) {
	  QString line(fstr.readLine(0));
	  QString modname(MustMatch(modulename_pattern,line));
	  OutputText("Module Name " + modname + "\n");
	  QString moddesc(MustMatch(moduledesc_pattern,line));
	  OutputText("Module Description " + moddesc + "\n");
	  line = fstr.readLine(0);
	  QString secname(MustMatch(sectioname_pattern,line));
	  OutputText("Section Name " + secname + "\n");
	  line = fstr.readLine(0);
	  while (!fstr.atEnd() && !TestMatch(docblock_pattern,line)) {
	    QString groupname(MustMatch(groupname_pattern,line));
	    OutputText("Group Name " + groupname + "\n");
	    line = fstr.readLine(0);
	    while (!fstr.atEnd() && !TestMatch(groupname_pattern,line) && !TestMatch(docblock_pattern,line)) {
	      if (TestMatch(execin_pattern,line)) {
		QStringList cmdlist;
		line = fstr.readLine(0);
		while (!fstr.atEnd() && !TestMatch(execout_pattern,line)) {
		  if (TestMatch(ccomment_pattern,line)) 
		    cmdlist.push_back(MustMatch(ccomment_pattern,line)+"\n");
		  line = fstr.readLine(0);
		}
		cmdlist.push_back("quit;\n");
		QString resp(EvaluateCommands(cmdlist));
		OutputText("ExecBlock: " + resp);
		if (fstr.atEnd())
		  Halt("Unmatched docblock detected!");
		line = fstr.readLine(0);
	      } else {
		if (TestMatch(ccomment_pattern,line)) 
		  line = MustMatch(ccomment_pattern,line);
		OutputText(groupname + ":" + line + "\n");
		line = fstr.readLine(0);
	      }
	    }
	    if (fstr.atEnd())
	      Halt("Unmatched docblock detected!");
	  }
	  if (fstr.atEnd())
	    Halt("Unmatched docblock detected!");
	}
      }
    }
  }
  qApp->processEvents();
  CloseAllHandleWindows();
  delete context;
}

void ProcessDir(QDir dir) {
  OutputText("Processing Directory " + dir.absolutePath() + "...\n");
  dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
  QFileInfoList list = dir.entryInfoList();
  for (int i=0;i<list.size();i++) {
    QFileInfo fileInfo = list.at(i);
    if (fileInfo.isDir())
      ProcessDir(QDir(fileInfo.absoluteFilePath()));
    else
      ProcessFile(fileInfo);
  }
  qApp->processEvents();
}

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  GUISetup();
  ProcessDir(QDir("."));
  return app.exec();
}
