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
#include "Scope.hpp"
#include "ServerSocket.hpp"
#include "Socket.hpp"
#include "Serialize.hpp"
#include "LoadCore.hpp"
#include "Module.hpp"
#include "TeclaInterface.hpp"
#include "Command.hpp"
#include "MPIWrap.hpp"
#include <wx/string.h>
#include <wx/utils.h>
#include "mpi.h"

 
#ifdef F77_DUMMY_MAIN
#  ifdef __cplusplus
     extern "C"
#  endif
   int F77_DUMMY_MAIN() { return 1; }
#endif

namespace FreeMat {
  Context *context;
  static WalkTree *twalk;
}

using namespace FreeMat;

namespace FreeMat {

  Command *resp;

  // These are surrogate functions that handle commands that would normally be handled 
  // by the GraphicsServer.  I have tried to minimize the dependancy on these, but a
  // few commands (notably the "systemfunction") still remain.
  void SendGUICommand(Command *cp) {
    switch(cp->cmdNum) {
    CMD_SystemCapture:
      {
	wxString command(cp->data.getContentsAsCString());
	wxArrayString output;
	wxExecute(command,output);
	Array *dp = new Array[output.GetCount()];
	for (int k=0;k<output.GetCount();k++) {
	  const char *wp = output[k].c_str();
	  dp[k] = Array::stringConstructor(std::string(wp));
	}
	Dimensions dim(2);
	dim[0] = output.GetCount();
	dim[1] = 1;
	Array res(Array::Array(FM_CELL_ARRAY,dim,dp));
	Command *rp;
	resp = new Command(CMD_SystemCaptureAcq,res);
	delete cp;
      }
      break;
    default:
      std::cerr << "Panic - unrecognized GUI message " << cp->cmdNum << "\n";
    }
  }
  
  Command* GetGUIResponse() {
    return resp;
  }
}


int main(int argc, char *argv[]) {
  try {
    MPI_Init(&argc, &argv);
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

    LoadMPIFunctions(context);
    LoadCoreFunctions(context);
    const char *envPtr;
    envPtr = getenv("FREEMAT_PATH");

    Interface *io = new TeclaInterface;
    if (envPtr)
      io->initialize(std::string(envPtr),context);
    else
      io->initialize(std::string(""),context);
    twalk = new WalkTree(context,io);

    bool keepRunning;
    keepRunning = true;
    while (keepRunning) {
//--> mpi
//mpicommrank  mpicommsize  mpieval      mpirecv      mpisend
//--> mpieval('a = randn(32)',1:3,1)
//--> mpieval('mpisend(a,0,32,1)',1:3,1)
//--> a1 = mpirecv(1,32,1)
//
// //       std::cout << "Waiting for input...\n";
      char cmdBuffer[4096];
      MPI_Status status;
      MPI_Recv(cmdBuffer,4096,MPI_CHAR,0,0,MPI_COMM_WORLD,&status);
      strcat(cmdBuffer,"\n");
      keepRunning = !(twalk->evaluateString(cmdBuffer));
    }
  } catch (Exception &e) {
  }
  MPI_Finalize();
}
