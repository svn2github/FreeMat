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

  //   RegisterSTDINCallback(stdincb);stst
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

  FLTKTerminalWindow *win = new FLTKTerminalWindow(400,300,"FreeMat v" VERSION,"");
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
