#include <X11/Xlib.h>
#include <unistd.h>
#include "XWindow.hpp"
#include "Terminal.hpp"
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

Terminal term;

sig_t signal_suspend_default;
sig_t signal_resume_default;

void signal_suspend(int a) {
  term.RestoreOriginalMode();
  printf("Suspending FreeMat...\n");
  fflush(stdout);
  signal(SIGTSTP,signal_suspend_default);
  raise(SIGTSTP);
}

void signal_resume(int a) {
  fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
  printf("Resuming FreeMat...\n");
  term.SetRawMode();
  term.Redisplay();
}

void signal_resize(int a) {
  printf("Resize...\n");
  term.ResizeEvent();
}

void stdincb() {
  char c;
  while (read(STDIN_FILENO, &c, 1) == 1) {
    term.ProcessChar(c);
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
  PathSearcher psearch("PATH");
  return GetPathOnly(psearch.ResolvePath(argv0));
}

int main(int argc, char *argv[]) {
  std::string paths;

  paths = GetApplicationPath(argv[0]);
  
  signal_suspend_default = signal(SIGTSTP,signal_suspend);
  signal_resume_default = signal(SIGCONT,signal_resume);
  signal(SIGWINCH, signal_resize);
  d = XOpenDisplay(0);
  if (!d) {
    fprintf(stderr,"Error - unable to open X display\n");
    exit(1);
  }
  fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
  SetActiveDisplay(d);
  RegisterSTDINCallback(stdincb);
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
}
