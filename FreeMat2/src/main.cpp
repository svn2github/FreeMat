/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include <QDir>
#include <QtGui>
#include <QDebug>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "MainApp.hpp"
#include <qapplication.h>
#include "Exception.hpp"
#include "application.hpp"
#include "FuncMode.hpp"


MainApp *m_app;
FuncMode *m_func;

void usage() {
  printf("%s\n  Command Line Help\n",Interpreter::getVersionString().c_str());
  printf(" You can invoke FreeMat with the following command line options:\n");
  printf("     -f <command>  Runs FreeMat in command mode.  FreeMat will \n");
  printf("                   startup, run the given command, and then quit.\n");
  printf("                   Note that this option uses the remainder of the\n");
  printf("                   command line, so use it last.\n");
#ifdef Q_WS_X11
  printf("     -nogui        Suppress the GUI for FreeMat.\n");
  printf("     -noplastique  Do not force the plastique style for GUI.\n");
#endif
  printf("     -noX          Disables the graphics subsystem.\n");
  printf("     -e            uses a dumb terminal interface \n");
  printf("                   (no command line editing, etc.)\n");
  printf("                   This flag is primarily used when \n");
  printf("                   you want to capture input/output\n");
  printf("                   to FreeMat from another application.\n");
  printf("     -i <path>     Install FreeMat - provide the path to the\n");
  printf("                   FreeMat data directory (containing the\n");
  printf("                   scripts, help and other files.).  Normally\n");
  printf("                   these are installed in /usr/local/share/\n");
  printf("                   but regardless, you must run FreeMat -i once\n");
  printf("                   to indicate the location of this directory.\n");
  printf("                   Note that in this mode, FreeMat will only \n");
  printf("                   update its internal configuration and then\n");
  printf("                   exit.\n");
  printf("     -help         Get this help text\n");
  exit(0);
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

int main(int argc, char *argv[]) {  
  QCoreApplication *app;
  int nogui = parseFlagArg(argc,argv,"-nogui",false);
  int scriptMode = parseFlagArg(argc,argv,"-e",false); 
  int noX = parseFlagArg(argc,argv,"-noX",false);
  int help = parseFlagArg(argc,argv,"-help",false);
  int help2 = parseFlagArg(argc,argv,"--help",false);
  int funcMode = parseFlagArg(argc,argv,"-f",true);
  int nogreet = parseFlagArg(argc,argv,"-nogreet",false);
  int noplastique = parseFlagArg(argc,argv,"-noplastique",false);
  int installMode = parseFlagArg(argc,argv,"-i",true);
  
  if (installMode) {
    app = new QCoreApplication(argc, argv);
    QSettings settings("FreeMat","FreeMat");
    settings.setValue("root",argv[installMode+1]);
    std::cout << "FreeMat root path set to '" << argv[installMode+1] << "'\n";
    return 0;
  }

  if (help || help2) usage();
  if (!noX) {
    app = new QApplication(argc, argv);
#ifdef Q_WS_X11
    if (!noplastique)
      QApplication::setStyle(new QPlastiqueStyle);
#endif
  } else {
    app = new QCoreApplication(argc, argv);
    nogui = true;
  }
  
  if (scriptMode) nogui = 1;
  m_app = new MainApp;
  if (!nogui)
    m_app->SetupGUICase();
  else if (!scriptMode) 
    m_app->SetupInteractiveTerminalCase();
  else
    m_app->SetupDumbTerminalCase();
  m_app->SetGUIMode(!noX);
  m_app->SetSkipGreeting(nogreet);
  m_app->Run();
  //  QTimer::singleShot(0,m_app,SLOT(Run()));
  // In function mode, we need to send a command to the GUI
  if (funcMode) {
    m_func = new FuncMode(argv[funcMode+1]);
    QObject::connect(m_func,SIGNAL(SendCommand(QString)),
 		     m_app->GetKeyManager(),SLOT(QueueSilent(QString)));
    QTimer::singleShot(0,m_func,SLOT(Fire()));
  }
  return app->exec();
}
