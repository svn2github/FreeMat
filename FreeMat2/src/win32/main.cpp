#include <windows.h>
#include "FLTKTerminal.hpp"
#include "WalkTree.hpp"
#include "Module.hpp"
#include "LoadCore.hpp"
#include "LoadFN.hpp"
#include "GraphicsCore.hpp"
#include "System.hpp"

char help_path[1024];

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

void SetupHelpPath() {
	char apppath[1024];
	GetAppPath(apppath,sizeof(apppath));
	sprintf(help_path,"%s/freemat.chm",apppath);
}

int main(int argc, char *argv[]) {
  Context *context = new Context;
  SetupHelpPath();
  LoadModulefunctions(context);
  LoadCoreFunctions(context);
  LoadFNFunctions(context);
  LoadGraphicsCoreFunctions(context);  
  InitializeFigureSubsystem();
  const char *envPtr;
  envPtr = getenv("FREEMAT_PATH");

  FLTKTerminalWindow *win = new FLTKTerminalWindow(400,300,"FreeMat v " VERSION,help_path);
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
