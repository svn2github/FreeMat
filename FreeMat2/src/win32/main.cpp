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
  LoadGraphicsCoreFunctions(context);  
  InitializeXWindowSystem(hInstance);
  InitializePlotSubsystem();
  InitializeImageSubsystem();
  char buffer[1000];
  GetAppPath(buffer,sizeof(buffer));
  InitializeHelpDirectory(buffer);

  const char *envPtr;
  envPtr = getenv("FREEMAT_PATH");
  term.setContext(context);
  if (envPtr)
    term.setPath(std::string(envPtr));
  else
    term.setPath(std::string(""));
  WalkTree *twalk = new WalkTree(context,&term);
  term.SetEvalEngine(twalk);
  term.outputMessage(" Freemat - build ");
  term.outputMessage(__DATE__);
  term.outputMessage("\n");
  term.outputMessage(" Copyright (c) 2002-2004 by Samit Basu\n");
  while (twalk->getState() != FM_STATE_QUIT) {
	  twalk->resetState();
	  twalk->evalCLI();
  }
  return 0;
}

