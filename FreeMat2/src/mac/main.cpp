#include <FL/Fl.H>
#include <FL/x.H>
#include <unistd.h>
#include <stdio.h>
#include "Module.hpp"
#include "LoadCore.hpp"
#include "LoadFN.hpp"
#include "GraphicsCore.hpp"
#include "System.hpp"
#include "PathSearch.hpp"
#include "ParserInterface.hpp"
#include "File.hpp"
#include <stdlib.h>
#include <signal.h>
#include "FLTKTerminal.hpp"
#include "WalkTree.hpp"
#include "config.h"

using namespace FreeMat;

// Retrieve the bundle path
std::string GetApplicationPath() {
  char path[1000];
  CFBundleRef mainBundle = CFBundleGetMainBundle();
  if (mainBundle) {
    CFURLRef bundleURL = NULL;
    CFRetain(mainBundle);
    bundleURL = CFBundleCopyBundleURL(mainBundle);
    if (bundleURL) {
      FSRef bundleFSRef;
      if (CFURLGetFSRef(bundleURL, &bundleFSRef)) {
	FSRefMakePath(&bundleFSRef, (UInt8 *) path, 1000);
      }
      CFRelease(bundleURL);
    }
    CFRelease(mainBundle);
  }
  return std::string(path);;
}

int main(int argc, char *argv[]) {
  // Get the bundle path
  std::string bundle_path = GetApplicationPath();
  // The help path is 
  std::string help_path = "file://"+
    bundle_path+"/Contents/Resources/html/index.html";
  // The MFiles path is
  std::string m_path = bundle_path+"/Contents/Resources/MFiles:";

  //   RegisterSTDINCallback(stdincb);stst
  Context *context = new Context;
  SpecialFunctionDef *sfdef = new SpecialFunctionDef;

  LoadModuleFunctions(context);
  LoadCoreFunctions(context);
  LoadFNFunctions(context);
  LoadGraphicsCoreFunctions(context);  
  InitializeFigureSubsystem();
  const char *envPtr;
  envPtr = getenv("FREEMAT_PATH");

  FLTKTerminalWindow *win = 
    new FLTKTerminalWindow(400,300,"FreeMat v" VERSION,
			   help_path.c_str());
  win->term()->setContext(context);
  if (envPtr)
    win->term()->setPath(m_path+std::string(envPtr));
  else 
    win->term()->setPath(m_path);
  win->show();
  WalkTree *twalk = new WalkTree(context,win->term());
  win->term()->outputMessage(" Freemat v");
  win->term()->outputMessage(VERSION);
  win->term()->outputMessage("\n");
  win->term()->outputMessage(" Copyright (c) 2002-2005 by Samit Basu\n");
  while (twalk->getState() != FM_STATE_QUIT) {
    if (twalk->getState() == FM_STATE_RETALL) 
      win->term()->clearMessageContextStack();
    twalk->resetState();
    twalk->evalCLI();
  }
  return 0;
}
