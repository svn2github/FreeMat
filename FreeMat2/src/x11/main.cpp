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

int main(int argc, char *argv[]) {
  
  signal_suspend_default = signal(SIGTSTP,signal_suspend);
  signal_resume_default = signal(SIGCONT,signal_resume);
  signal(SIGWINCH, signal_resize);
  d = XOpenDisplay(0);
  if (!d) {
    fprintf(stderr,"Error - unable to open X display\n");
    exit(1);
  }
  SetActiveDisplay(d);
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

  // Check for simple scripting mode
  bool scriptMode = false;
  int scriptSpec = 1;
  while (!scriptMode && scriptSpec < argc) {
    scriptMode = scriptMode | strcmp(argv[scriptSpec],"-e") == 0;
    scriptSpec++;
  }
  if (scriptSpec > argc) scriptMode = false;

  bool funcMode = false;
  int funcSpec = 1;
  while (!funcMode && funcSpec < argc) {
    funcMode = funcMode | strcmp(argv[funcSpec],"-f") == 0;
    funcSpec++;
  }
  if (funcSpec >= argc) funcMode = false;

  // Instantiate the terminal class
  if (!scriptMode && !funcMode) {
    term = new Terminal;
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
  } else {
    term = new DumbTerminal;
  }

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
  term->outputMessage(" Freemat v1.03 ");
  term->outputMessage("\n");
  term->outputMessage(" Copyright (c) 2002-2004 by Samit Basu\n");
  if (!funcMode) {
    while (twalk->getState() != FM_STATE_QUIT) {
      twalk->resetState();
      twalk->evalCLI();
    }
  } else {
    char buffer[1024];
    sprintf(buffer,"%s\n",argv[funcSpec]);
    twalk->evaluateString(buffer);
  }
  term->RestoreOriginalMode();
  return 0;
}
