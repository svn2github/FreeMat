#include <FL/Fl.H>
#include <FL/x.H>
#include <unistd.h>
#include <fcntl.h>
#include "FLTKTerminal.hpp"
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
#include "config.h"
#if 0
#include "freemat.xpm"
#endif

using namespace FreeMat;

Display *d;
int screen_num = 0;

Terminal *term;
FLTKTerminalWindow *win;

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

bool checkBundleMode(char* argv0, char bundlefunc[1024]) {
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
  } else {
    retval = false;
  }
  // Close up the file handle
  fclose(fp);
  return retval;
}


void LoadBundleFunctions(char* argv0, WalkTree *twalk) {
  char bundlefunc[1024];
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
  if (memcmp(buf,"fmexe",5)==0) {
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
      twalk->getContext()->insertFunctionGlobally(adef,false);
    }
    delete(f);
  } 
  fclose(fp);
}

void usage() {
  printf("FreeMat Command Line Help\n");
  printf("   You can invoke FreeMat with the following command line options:\n");
  printf("     -f <command>  Runs FreeMat in command mode.  FreeMat will \n");
  printf("                   startup, run the given command, and then quit.\n");
  printf("                   Note that this option uses the remainder of the\n");
  printf("                   command line, so use it last.\n");
  printf("     -gui          Launch the GUI version of FreeMat.\n");
  printf("     -noX          Disables the graphics subsystem.\n");
  printf("     -e            uses a dumb terminal interface (no command line editing, etc.)\n");
  printf("                   This flag is primarily used when you want to capture input/output\n");
  printf("                   to FreeMat from another application.\n");
  printf("     -help         Get this help text\n");
  exit(0);
}

char ** unpackBundledArgs(int& myargc, char bundlefunc[1024], int argc, char *argv[]) {
  // The bundlefunc has to be tokenized into arguments
  bool instring;
  int argcount;
  instring = false;
  argcount = 0;
  char *cp = bundlefunc;
  while (*cp) {
    // Remove leading whitespace
    while ((*cp) && (*cp == ' ')) cp++;
    if (*cp) {
      argcount++;
      instring = false;
      while ((*cp) && (instring || (!instring && (*cp != ' ')))) {
	if (*cp == '\'')
	  instring = !instring;
	cp++;
      }
    }
  }
  myargc = argc+argcount;
  char **myargv = (char**) malloc(sizeof(char*)*(argcount+argc));
  myargv[0] = argv[0];
  char tbuff[1024];
  char *tp;

  instring = false;
  argcount = 0;
  cp = bundlefunc;
  while (*cp) {
    // Remove leading whitespace
    while ((*cp) && (*cp == ' ')) cp++;
    if (*cp) {
      argcount++;
      instring = false;
      tp = tbuff;
      memset(tbuff,0,1024);
      while ((*cp) && (instring || (!instring && (*cp != ' ')))) {
	*tp++ = *cp;
	if (*cp == '\'')
	  instring = !instring;
	cp++;
      }
      myargv[argcount] = strdup(tbuff);
    }
  }
  for (int i=1;i<argc;i++)
    myargv[i+argcount] = argv[i];
  
  return myargv;
}

int main(int argc, char *argv[]) {
  // First thing to do is determine if we are bundled
  // or not. 
  int bundledMode;
  char bundlefunc[1024];
  bundledMode = checkBundleMode(argv[0],bundlefunc);

  int myargc;
  char **myargv;

  // If we are in bundled mode, then we have to unpack the arguments in bundlefunc
  // and adjust the argument array.
  if (bundledMode)
    myargv = unpackBundledArgs(myargc,bundlefunc,argc,argv);
  else {
    myargc = argc;
    myargv = argv;
  }

  int scriptMode;
  int funcMode;
  int withoutX;
  int guimode;
  
  scriptMode = parseFlagArg(myargc,myargv,"-e",false);
  funcMode = parseFlagArg(myargc,myargv,"-f",true);
  withoutX = parseFlagArg(myargc,myargv,"-noX",false);
  guimode = parseFlagArg(myargc,myargv,"-gui",false);
  if (parseFlagArg(myargc,myargv,"-help",false)) usage();

  if (!withoutX) {
    fl_open_display();
    Fl::visual(FL_RGB);
  }
  
  // Instantiate the terminal class
  if (!scriptMode && !funcMode && !guimode) {
    term = new Terminal;
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
  } else if (!guimode) {
    term = new DumbTerminal;
  }

  if (!guimode) {
    signal_suspend_default = signal(SIGTSTP,signal_suspend);
    signal_resume_default = signal(SIGCONT,signal_resume);
    signal(SIGWINCH, signal_resize);
    Fl::add_fd(STDIN_FILENO,FL_READ,stdincb);
  }
  Context *context = new Context;
  LoadModuleFunctions(context);
  LoadCoreFunctions(context);
  LoadFNFunctions(context);
  LoadGraphicsCoreFunctions(context);  
  InitializeFigureSubsystem();

  const char *envPtr;
  envPtr = getenv("FREEMAT_PATH");

  if (!guimode) {
    term->setContext(context);
    if (envPtr)
      term->setPath(std::string(envPtr));
    else 
      term->setPath(std::string(""));
    WalkTree *twalk = new WalkTree(context,term);
    term->SetEvalEngine(twalk);
    try {
      term->Initialize();
    } catch(Exception &e) {
      fprintf(stderr,"Unable to initialize terminal.  Try to start FreeMat with the '-e' option.");
      exit(1);
    }
    LoadBundleFunctions(myargv[0], twalk);
    if (!funcMode && !bundledMode) {
      term->outputMessage(" Freemat v");
      term->outputMessage(VERSION);
      term->outputMessage("\n");
      term->outputMessage(" Copyright (c) 2002-2005 by Samit Basu\n");
      while (twalk->getState() != FM_STATE_QUIT) {
	twalk->resetState();
	twalk->evalCLI();
      }
    } else {
      char buffer[1024];
      if (funcMode) {
	sprintf(buffer,"%s",myargv[funcMode+1]);
	for (int i=funcMode+2;i<myargc;i++) {
	  strcat(buffer," ");
	  strcat(buffer,myargv[i]);
	}
	strcat(buffer,"\n");
      }
      ParserState parserState = parseString(buffer);
      if (parserState != ScriptBlock) {
	printf("Error: syntax error in command line arguments to FreeMat\r\n");
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
  } else {
    // We need to find the help files...
    // To do so, we need to search through the
    // path.
    std::string helppath;
    if (envPtr) {
      PathSearcher psearch(envPtr);
      try {
	helppath = psearch.ResolvePath("../html/index.html");
      } catch (Exception& E) {
	helppath = "/usr/local/share/FreeMat/html/index.html";
      }
    } else 
      helppath = "/usr/local/share/FreeMat/html/index.html";
    win = new FLTKTerminalWindow(400,300,"FreeMat v" VERSION,
				 helppath.c_str());
    win->term()->setContext(context);
//     Pixmap p, mask;
//     XpmCreatePixmapFromData(fl_display, DefaultRootWindow(fl_display),
// 			    freemat, &p, &mask, NULL);
//     win->icon((char*) p)
    win->show();
    if (envPtr)
      win->term()->setPath(std::string(envPtr));
    else 
      win->term()->setPath(std::string(""));
    WalkTree *twalk = new WalkTree(context,win->term());
    win->term()->outputMessage(" Freemat v");
    win->term()->outputMessage(VERSION);
    win->term()->outputMessage("\n");
    win->term()->outputMessage(" Copyright (c) 2002-2005 by Samit Basu\n");
    while (twalk->getState() != FM_STATE_QUIT) {
      twalk->resetState();
      twalk->evalCLI();
    }
  }
  return 0;
}
