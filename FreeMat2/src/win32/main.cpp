#include "XWindow.hpp"
#include "WinTerminal.hpp"
#include <stdio.h>
#include "Module.hpp"
#include "LoadCore.hpp"
#include "GraphicsCore.hpp"
#include "System.hpp"
#include "PathSearch.hpp"
#include <stdlib.h>
#include <signal.h>

using namespace FreeMat;
namespace FreeMat {
	Context *context;
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
  SetupWinTerminalClass(hInstance);
  WinTerminal term(hInstance, iCmdShow);

  context = new Context;
  BuiltInFunctionDef *f2def = new BuiltInFunctionDef;
  f2def->retCount = 0;
  f2def->argCount = 5;
  f2def->name = "loadFunction";
  f2def->fptr = LoadFunction;
  context->insertFunctionGlobally(f2def);
  
  SpecialFunctionDef *sfdef = new SpecialFunctionDef;
  sfdef->retCount = 0;
  sfdef->argCount = 5;
  sfdef->name = "import";
  sfdef->fptr = ImportFunction;
  context->insertFunctionGlobally(sfdef);

  LoadCoreFunctions(context);
  LoadGraphicsCoreFunctions(context);  
  InitializeXWindowSystem(hInstance);
  InitializePlotSubsystem();
  InitializeImageSubsystem();
  const char *envPtr;
  envPtr = getenv("FREEMAT_PATH");
  if (envPtr)
    term.initialize(std::string(envPtr),context);
  else
    term.initialize(std::string(""),context);
  WalkTree *twalk = new WalkTree(context,&term);
  term.SetEvalEngine(twalk);
  term.outputMessage(" Freemat - build ");
  term.outputMessage(__DATE__);
  term.outputMessage("\n");
  term.outputMessage(" Copyright (c) 2002-2004 by Samit Basu\n");
  twalk->evalCLI();

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  return msg.wParam;
}

