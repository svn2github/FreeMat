#include <X11/Xlib.h>
#include <unistd.h>
#include "XWindow.hpp"
#include "Terminal.hpp"
#include "DumbTerminal.hpp"
#include <stdio.h>
#include "Module.hpp"
#include "LoadCore.hpp"
#include "GraphicsCore.hpp"
#include "System.hpp"
#include "PathSearch.hpp"
#include <stdlib.h>
#include <signal.h>


using namespace FreeMat;

Display *d;
int screen_num = 0;

Terminal *term;

sig_t signal_suspend_default;
sig_t signal_resume_default;

void signal_suspend(int a) {
  term->RestoreOriginalMode();
  printf("Suspending FreeMat...\n");
  fflush(stdout);
  signal(SIGTSTP,signal_suspend_default);
  raise(SIGTSTP);
}

void signal_resume(int a) {
  fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
  printf("Resuming FreeMat...\n");
  term->SetRawMode();
  term->Redisplay();
}

void signal_resize(int a) {
  term->ResizeEvent();
}

void stdincb() {
  char c;
  while (read(STDIN_FILENO, &c, 1) == 1) {
    term->ProcessChar(c);
  }
}

std::string GetApplicationPath(char *argv0) {
  std::string retpath;
  // Case 1 - absolute path
  if (argv0[0] == '/') 
    return GetPathOnly(std::string(argv0));
  // Case 2 - relative path
  char buffer[4096];
  getcwd(buffer,sizeof(buffer));
  retpath = CheckEndSlash(std::string(buffer)) + std::string(argv0);
  // This file should exist
  if (FileExists(retpath))
    return GetPathOnly(std::string(retpath));
  std::string path(getenv("PATH"));
  PathSearcher psearch(path);
  return GetPathOnly(psearch.ResolvePath(argv0));
}

// Search through the arguments to freemat... look for the given
// flag.  if the flagarg variable is true, then an argument must
// be provided to the flag.  If the flag is not found, then a 
// 0 is returned.  Otherwise, the index into argv of the flag is
// returned.
int parseFlagArg(int argc, char *argv[], const char* flagstring, bool flagarg) {
  bool flagFound = false;
  int ndx;
  ndx = 1;
  while (!flagFound && ndx < argc) {
    flagFound = strcmp(argv[ndx],flagstring) == 0;
    if (!flagFound) ndx++;
  }
  if (flagFound && flagarg && (ndx == argc-1)) {
    fprintf(stderr,"Error: flag %s requires an argument!\n",flagstring);
    exit(1);
  }
  if (!flagFound)
    ndx = 0;
  return ndx;
}

void usage() {
  printf("FreeMat Command Line Help\n");
  printf("   You can invoke FreeMat with the following command line options:\n");
  printf("     -f <command>  Runs FreeMat in command mode.  FreeMat will \n");
  printf("                   startup, run the given command, and then quit.\n");
  printf("     -noX          Disables the graphics subsystem.\n");
  printf("     -e            uses a dumb terminal interface (no command line editing, etc.)\n");
  printf("                   This flag is primarily used when you want to capture input/output\n");
  printf("                   to FreeMat from another application.\n");
  printf("     -help         Get this help text\n");
  exit(0);
}

int main(int argc, char *argv[]) {
  int scriptMode;
  int funcMode;
  int withoutX;

  scriptMode = parseFlagArg(argc,argv,"-e",false);
  funcMode = parseFlagArg(argc,argv,"-f",true);
  withoutX = parseFlagArg(argc,argv,"-noX",false);
  if (parseFlagArg(argc,argv,"-help",false)) usage();
  
  // Instantiate the terminal class
  if (!scriptMode && !funcMode) {
    term = new Terminal;
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
  } else {
    term = new DumbTerminal;
  }

  signal_suspend_default = signal(SIGTSTP,signal_suspend);
  signal_resume_default = signal(SIGCONT,signal_resume);
  signal(SIGWINCH, signal_resize);
  if (!withoutX) {
    d = XOpenDisplay(0);
    if (!d) {
      fprintf(stderr,"Error - unable to open X display.  Please check the environment variable DISPLAY.\n");
      exit(1);
    }
    SetActiveDisplay(d);
  }
  RegisterSTDINCallback(stdincb);
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
  LoadGraphicsCoreFunctions(context);  
  InitializePlotSubsystem();
  InitializeImageSubsystem();

  const char *envPtr;
  envPtr = getenv("FREEMAT_PATH");
  term->setContext(context);
  if (envPtr)
    term->setPath(std::string(envPtr));
  else 
    term->setPath(std::string(""));
  WalkTree *twalk = new WalkTree(context,term);
  term->SetEvalEngine(twalk);
  term->Initialize();
  if (!funcMode) {
    term->outputMessage(" Freemat v");
    term->outputMessage(VERSION);
    term->outputMessage("\n");
    term->outputMessage(" Copyright (c) 2002-2004 by Samit Basu\n");
    while (twalk->getState() != FM_STATE_QUIT) {
      if (twalk->getState() == FM_STATE_RETALL) 
	term->clearMessageContextStack();
      twalk->resetState();
      twalk->evalCLI();
    }
  } else {
    char buffer[1024];
    sprintf(buffer,"%s\n",argv[funcMode+1]);
    twalk->evaluateString(buffer);
  }
  term->RestoreOriginalMode();
  return 0;
}
