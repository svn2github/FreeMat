#include <qapplication.h>
#include "MainApp.hpp"
using namespace FreeMat;

#include "BaseTerminal.hpp"
#include "Module.hpp"
#include "Class.hpp"
#include "LoadCore.hpp"
#include "LoadFN.hpp"
#include "GraphicsCore.hpp"
#include "File.hpp"
#include "QTDraw.hpp"

MainApp::MainApp() {
}

MainApp::~MainApp() {
}

void MainApp::SetTerminal(BaseTerminal* term) {
  m_term = term;
}

void MainApp::SetHelpPath(std::string helpPath) {
  m_helpPath = helpPath;
}

int MainApp::Run() {
  Context *context = new Context;
  
  LoadModuleFunctions(context);
  LoadClassFunction(context);
  LoadCoreFunctions(context);
  LoadFNFunctions(context);
  LoadGraphicsCoreFunctions(context);  
  LoadQTDraw(context);
  InitializeFigureSubsystem();

  const char *envPtr;
  envPtr = getenv("FREEMAT_PATH");
  m_term->setContext(context);
  if (envPtr)
    m_term->setPath(std::string(envPtr));
  else 
    m_term->setPath(std::string(""));
  WalkTree *twalk = new WalkTree(context,m_term);
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
  qApp->quit();
  return 0;
}
