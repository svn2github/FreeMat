#include <QApplication>
#include <QTextEdit>
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

void ProcessFile(QFileInfo file) {
  if (file.suffix() == "mpp")
    OutputText("Processing File " + file.absoluteFilePath() + "...\n");
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
}

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  WalkTree* twalk = Setup();
  QDir dir("../../");
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
