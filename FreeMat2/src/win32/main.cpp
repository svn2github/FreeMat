#if 0
#include "XWindow.hpp"
#include "WinTerminal.hpp"
#include <stdio.h>
#include "Module.hpp"
#include "LoadCore.hpp"
#include "LoadFN.hpp"
#include "GraphicsCore.hpp"
#include "System.hpp"
#include "PathSearch.hpp"
#include <stdlib.h>
#include <signal.h>
#endif

#include <windows.h>
#include "FLTKTerminal.hpp"
#include "WalkTree.hpp"
#include "Module.hpp"
#include "LoadCore.hpp"
#include "LoadFN.hpp"
#include "GraphicsCore.hpp"
#include "System.hpp"

using namespace FreeMat;
#define VERSION "1.08"

int GetAppPath(LPTSTR pstr,int length){
 int ret,i;
 ret=GetModuleFileName(NULL,pstr,length); // this gets the name of the running app
 if(!ret)return 0;
 i=lstrlen(pstr)-1;
 for(;i>=0;i--){ // replace backslash with terminating null
  if(pstr[i]==TEXT('\\')){
   pstr[i]=TEXT('\0');
   break;
  }
 }
 return lstrlen(pstr); //return length of final string
}

int main(int argc, char *argv[]) {
  Context *context = new Context;
  SpecialFunctionDef *sfdef = new SpecialFunctionDef;
  sfdef->retCount = 0;
  sfdef->argCount = 5;
  sfdef->name = "loadlib";
  sfdef->fptr = LoadLibFunction;
  context->insertFunctionGlobally(sfdef);
  
  sfdef = new SpecialFunctionDef;
  sfdef->retCount = 0;
  sfdef->argCount = 5;
  sfdef->name = "import";
  sfdef->fptr = ImportFunction;
  context->insertFunctionGlobally(sfdef);

  LoadCoreFunctions(context);
  LoadFNFunctions(context);
  LoadGraphicsCoreFunctions(context);  
  InitializeFigureSubsystem();
  const char *envPtr;
  envPtr = getenv("FREEMAT_PATH");

  FLTKTerminalWindow *win = new FLTKTerminalWindow(400,300,"FreeMat v " VERSION);
  win->term()->setContext(context);
  if (envPtr)
    win->term()->setPath(std::string(envPtr));
  else 
    win->term()->setPath(std::string(""));
  WalkTree *twalk = new WalkTree(context,win->term());
  win->term()->outputMessage(" Freemat v");
  win->term()->outputMessage(VERSION);
  win->term()->outputMessage("\n");
  win->term()->outputMessage(" Copyright (c) 2002-2004 by Samit Basu\n");
  while (twalk->getState() != FM_STATE_QUIT) {
    if (twalk->getState() == FM_STATE_RETALL) 
      win->term()->clearMessageContextStack();
    twalk->resetState();
    twalk->evalCLI();
  }
  return 0;
}

#if 0
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
  SetupWinTerminalClass(hInstance);
  WinTerminal term(hInstance, iCmdShow);

  Context *context = new Context;
  SpecialFunctionDef *sfdef = new SpecialFunctionDef;
  sfdef->retCount = 0;
  sfdef->argCount = 5;
  sfdef->name = "loadFunction";
  sfdef->fptr = LoadLibFunction;
  context->insertFunctionGlobally(sfdef);

  sfdef = new SpecialFunctionDef;
  sfdef->retCount = 0;
  sfdef->argCount = 5;
  sfdef->name = "import";
  sfdef->fptr = ImportFunction;
  context->insertFunctionGlobally(sfdef);

  LoadCoreFunctions(context);
  LoadFNFunctions(context);
  LoadGraphicsCoreFunctions(context);  
  InitializeXWindowSystem(hInstance);
  InitializeFigureSubsystem();
  const char *envPtr;
  envPtr = getenv("FREEMAT_PATH");
  term.setContext(context);
  if (envPtr)
    term.setPath(std::string(envPtr));
  else
    term.setPath(std::string(""));
  WalkTree *twalk = new WalkTree(context,&term);
  term.SetEvalEngine(twalk);
  term.outputMessage(" Freemat v1.07 ");
  term.outputMessage("\n");
  term.outputMessage(" Copyright (c) 2002-2004 by Samit Basu\n");
  while (twalk->getState() != FM_STATE_QUIT) {
    if (twalk->getState() == FM_STATE_RETALL) 
      term.clearMessageContextStack();
    twalk->resetState();
    twalk->evalCLI();
  }
  return 0;
}
#endif
