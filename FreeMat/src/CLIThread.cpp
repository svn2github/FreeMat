// Copyright (c) 2002, 2003 Samit Basu
// 
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.

#include "WalkTree.hpp"
#include "Context.hpp"
#include "Exception.hpp"
#include "ParserInterface.hpp"
#include "Math.hpp"
#include "Malloc.hpp"
#include "Module.hpp"
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include "LoadCore.hpp"
#include "GraphicsCore.hpp"
#include "GraphicsServer.hpp"
#include "CLIThread.hpp"
#include <locale.h>
#include "Command.hpp"
#include <iostream>
#include "Interface.hpp"
#include <signal.h>
#include "VolView.hpp"

#include "wx/dynlib.h"

namespace FreeMat {
  ::wxSemaphore cmd_semaphore;
  Command *reply;
  Context *context;  
  static WalkTree *twalk;
  static ::wxApp *g_serv;
  char dummy[1000];

  void PostGUIReply(Command *a_reply) {
    reply = a_reply;
    cmd_semaphore.Post();
  }

  void SendGUICommand(Command *cmd) {
    wxCommandEvent eventCustom(wxEVT_GUI_COMMAND);
    Array a(cmd->data);
    eventCustom.SetClientData(cmd);
    wxPostEvent(g_serv,eventCustom);
  }

  Command* GetGUIResponse() {
    // Wait for the semaphore to tick
    cmd_semaphore.Wait();
    return reply;
  }

  CLIThread::CLIThread(::wxApp* serv, Interface* a_io) : 
    wxThread(wxTHREAD_JOINABLE) {
    m_serv = serv;
    g_serv = serv;
    io = a_io;
  }

  CLIThread::~CLIThread() {
  }

  void CLIThread::OnExit() {
  }

  void* CLIThread::Entry() {
    ASTPtr code;
    context = new Context();
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
    const char *envPtr;
    envPtr = getenv("FREEMAT_PATH");
    if (envPtr)
      io->initialize(std::string(envPtr),context);
    else
      io->initialize(std::string(""),context);
    twalk = new WalkTree(context,io);
    io->outputMessage(" Freemat - build ");
    io->outputMessage(__DATE__);
    io->outputMessage("\n");
    io->outputMessage(" Copyright (c) 2002,2003 by Samit Basu\n");
    try{
      twalk->evalCLI();
    } catch (...) {
      io->errorMessage(" Fatal error!  Unhandled exception...\n");
    }
    SendGUICommand(new Command(CMD_Quit));
    return NULL;
  }
}
