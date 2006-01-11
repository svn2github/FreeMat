#include <QApplication>
#include <QTextEdit>
#include <QTextStream>
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

void OutputText(QString str) {
  m_text->insertPlainText(str);
  //  m_text->setCursor(m_text->textCursor().movePosition(QCursor::End));
  m_text->ensureCursorVisible();
  qDebug(str);
}

namespace FreeMat {
  class HelpTerminal : public KeyManager {
  public:
    HelpTerminal() {}
    virtual ~HelpTerminal() {}
    virtual void Initialize() {}
    virtual void RestoreOriginalMode() {}
    virtual void OutputRawString(std::string txt) {
      OutputText(txt.c_str());
    }
    virtual void ResizeEvent() {}
    virtual void MoveDown() {
      OutputText("\n");
    }
    virtual char* getLine(std::string aprompt) {
      static int count = 0;
      if (count == 0) {
	OutputText(aprompt.c_str());
	OutputText("a = 1:32\n");
	count++;
	return strdup("a = 1:32\n");
      } else {
	QEventLoop m_loop;
	m_loop.exec();
      }
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

WalkTree* Setup() {
  Context *context = new Context;
  m_term = new HelpTerminal;

  m_text = new QTextEdit;
  m_text->setReadOnly(true);
  m_text->resize(400,400);
  m_text->show();
  m_text->setFontFamily("Courier");

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
  WalkTree *twalk = new WalkTree(context,m_term);
  return twalk;
}

void ProcessFile(QFileInfo fileinfo) {
  QRegExp cmstrt("^\\s*//!");
  QRegExp modulename("^\\s*//@Module\\s*(\\b\\w+\\b)");
  QRegExp moduledesc("^\\s*//@Module\\s*(\\b.*)");
  QRegExp sectioname("^\\s*//@@Section\\s*(\\b\\w+\\b)");
  if (fileinfo.suffix() == "mpp") {
    OutputText("Processing File " + fileinfo.absoluteFilePath() + "...\n");
    QFile file(fileinfo.absoluteFilePath());
    if (file.open(QFile::ReadOnly)) {
      QTextStream fstr(&file);
      while (!fstr.atEnd()) {
	QString line(fstr.readLine(0));
	if (cmstrt.indexIn(line) >= 0) {
	  QString line(fstr.readLine(0));
	  OutputText("Analyze:" + line + "\n");
	  QString modname(Match(modulename,line));
	  QString moddesc(Match(moduledesc,line));
	  line = fstr.readLine(0);
	  QString secname(Match(sectionname,line));
	  // This should contain the module name
	  if (modulename.indexIn(line) < 0)
	    exit(1);
	  else 
	    OutputText("Module Name " + modulename.cap(1) + "\n");
	  if (moduledesc.indexIn(line) < 0)
	    exit(1);
	  else
	    OutputText("Module Description " + moduledesc.cap(1) + "\n");
	  while ((!fstr.atEnd()) && (cmstrt.indexIn(line) < 0)) {
	    OutputText(line);
	    OutputText("\n");
	    line = fstr.readLine(0);
	  }
	  OutputText(line);
	  OutputText("\n");
	}
      }
    }
  }
  qApp->processEvents();
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
  WalkTree* twalk = Setup();
  QDir dir("../../MFiles");
  ProcessDir(dir);
  m_term->outputMessage(" Freemat v2.0");
  m_term->outputMessage("\n");
  m_term->outputMessage(" Copyright (c) 2002-2005 by Samit Basu\n");
  try {
    while (1) {
      try {
	twalk->evalCLI();
      } catch (WalkTreeRetallException) {
	m_term->outputMessage("retall\n");
	twalk->clearStacks();
      } catch (WalkTreeReturnException &e) {
      }
    }
  } catch (WalkTreeQuitException &e) {
  } catch (std::exception& e) {
    std::cout << "Exception caught: " << e.what() << "\n";
  }
  m_term->RestoreOriginalMode();
  return 0;
}
