#include <FL/Fl.H>
#include <FL/x.H>
#include <X11/Xlib.h>
#include <unistd.h>
#include "XWindow.hpp"
#include "Terminal.hpp"
#include "DumbTerminal.hpp"
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
#include "Plot3D.hpp"

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

void stdincb(int fd, void *data) {
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

bool checkBundleMode(char* argv0, char bundlefunc[1024], WalkTree *twalk) {
  // Get the path
  std::string apppath = GetApplicationPath(argv0);
  std::string appname = GetFilenameOnly(argv0);
  // Check for existence of the app binary
  if (!FileExists(apppath + "/" + appname)) {
    fprintf(stderr,"Unable to resolve %s to a functional pathname.\n",argv0);
    exit(1);
  }
  // Open us up.
  std::string appfull = apppath + "/" + appname;
  FILE *fp = fopen(appfull.c_str(),"rb");
  // Seek to the end minus 1024 bytes
  fseek(fp,-1024, SEEK_END);
  // Read next 5 bytes into a buffer
  char buf[5];
  fread(buf,5,sizeof(char),fp);
  // These should be "fmexe" if this is a bound executable
  bool retval;
  if (memcmp(buf,"fmexe",5)==0) {
    retval = true;
    // The next piece of information should be the 
    // offset of the p-code data
    int pcodeOffset;
    fread(&pcodeOffset,1,sizeof(int),fp);
    // The next piece of information should be the
    // name of the startup function
    int namelen;
    fread(&namelen,1,sizeof(int),fp);
    fread(bundlefunc,namelen,sizeof(char),fp);
    // Seek to the p-code data
    fseek(fp,pcodeOffset,SEEK_SET);
    int pcodeCount;
    fread(&pcodeCount,1,sizeof(int),fp);
    int i;
    File *f = new File(fp);
    for (i=0;i<pcodeCount;i++) {
      Serialize *s = new Serialize(f);
      s->handshakeClient();
      s->checkSignature('p',1);
      MFunctionDef *adef = ThawMFunction(s);
      adef->pcodeFunction = true;
      twalk->getContext()->insertFunctionGlobally(adef);
    }
    delete(f);
    fclose(fp);
  } else {
    retval = false;
  }
  return retval;
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
  // First thing to do is determine if we are bundled
  // or not. 
  int bundledMode;
  int scriptMode;
  int funcMode;
  int withoutX;
  char bundlefunc[1024];
  
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
    fl_open_display();
    /*
   d = XOpenDisplay(0);
    if (!d) {
      fprintf(stderr,"Error - unable to open X display.  Please check the environment variable DISPLAY.\n");
      exit(1);
    }
    SetActiveDisplay(d);
    */
  }
  //   RegisterSTDINCallback(stdincb);stst
  Fl::add_fd(STDIN_FILENO,FL_READ,stdincb);
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
  term->setContext(context);
  if (envPtr)
    term->setPath(std::string(envPtr));
  else 
    term->setPath(std::string(""));
  WalkTree *twalk = new WalkTree(context,term);
  term->SetEvalEngine(twalk);
  term->Initialize();
  bundledMode = checkBundleMode(argv[0], bundlefunc, twalk);
  if (!funcMode && !bundledMode) {
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
    if (funcMode) 
      sprintf(buffer,"%s\n",argv[funcMode+1]);
    else
      sprintf(buffer,"%s\n",bundlefunc);
    ParserState parserState = parseString(buffer);
    if (parserState != ScriptBlock) {
      printf("Error: syntax error on argument to -f\r\n");
      term->RestoreOriginalMode();
      return 1;
    }
    ASTPtr tree = getParsedScriptBlock();
    try {
      twalk->block(tree);
    } catch(Exception &e) {
      e.printMe(term);
      term->RestoreOriginalMode();
      return 5;	
    }
  }
  term->RestoreOriginalMode();
  return 0;
}
