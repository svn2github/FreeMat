#include <X11/Xlib.h>
#include <unistd.h>
#include "XWindow.hpp"
#include "BitmapFont.hpp"
#include "Terminal.hpp"
#include <stdio.h>
#include "Module.hpp"
#include "LoadCore.hpp"
#include "GraphicsCore.hpp"
#include "System.hpp"
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>


using namespace FreeMat;

Display *d;
int screen_num = 0;

namespace FreeMat {
  Context *context;
}

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
  printf("Resuming FreeMat...\n");
  term.SetRawMode();
  term.Redisplay();
}

void stdincb() {
  char c;
  while (read(STDIN_FILENO, &c, 1) == 1) {
    term.ProcessChar(c);
  }
}

bool FileExists(std::string filename) {
  struct stat filestat;
  stat(filename.c_str(),&filestat);
  return (S_ISREG(filestat.st_mode));
}

std::string GetPathOnly(std::string a) {
  // Strip off application name
  int ndx;
  ndx = a.rfind("/");
  a.erase(ndx+1,a.size());
  return a;
}

std::string CheckEndSlash(std::string a) {
  if (a[a.size()-1] != '/')
    a.append("/");
  return a;
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
  // Case 3 - file in "PATH" variable
  std::string path(getenv("PATH"));
  bool found = false;
  std::string tpath;
  while (!found && !path.empty()) {
    int ndx;
    ndx = path.find(":");
    tpath = path.substr(0,ndx);
    found = FileExists(CheckEndSlash(tpath) + std::string(argv0));
    path.erase(0,ndx+1);
  }
  if (found) {
    return tpath;
  } else {
    fprintf(stderr,"Error: unable to determine application path - support files unavailable!\n\r");
    return std::string();
  }
}

int main(int argc, char *argv[]) {
  std::string paths;

  paths = GetApplicationPath(argv[0]);
  XPSetFontPath(paths);
  
  signal_suspend_default = signal(SIGTSTP,signal_suspend);
  signal_resume_default = signal(SIGCONT,signal_resume);
  d = XOpenDisplay(0);
  if (!d) {
    fprintf(stderr,"Error - unable to open X display\n");
    exit(1);
  }
  fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
  SetActiveDisplay(d);
  RegisterSTDINCallback(stdincb);
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
