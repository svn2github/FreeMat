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

int main() {
  try {
    Socket *sck;
    Serialize *ser;
    context = new Context();
    BuiltInFunctionDef *f2def = new BuiltInFunctionDef;
    f2def = new BuiltInFunctionDef;
    f2def->retCount = 0;
    f2def->argCount = 5;
    f2def->name = "loadFunction";
    f2def->fptr = LoadFunction;
    context->insertFunctionGlobally(f2def);
    LoadCoreFunctions(context);
    Shell *aShell = new Shell(getenv("FREEMAT_PATH"),context);
    twalk = new WalkTree(context,aShell);
    sck = new Socket("localhost",5098);
    ser = new Serialize(sck);
    ser->handshakeClient();
    while(1) {
      // Get the name of the function
      char *fname = ser->getString();
      std::cout << "Got function name: " << fname << "\n";
      // Get the number of outputs
      int nargout = ser->getInt();
      std::cout << "Got number of outputs: " << nargout << "\n";
      // Get the number of inputs
      int nargin = ser->getInt();
      std::cout << "Got number of inputs: " << nargin << "\n";
      // The input arguments
      ArrayVector args;
      for (int i=0;i<nargin;i++) {
	Array vec;
	ser->getArray(vec);
	args.push_back(vec);
	std::cout << "Got argument: " << i << "\n";
      }
      FunctionDef *funcDef;
      if (!context->lookupFunction(fname,funcDef))
	throw Exception(std::string("function ") + fname + " undefined!");
      funcDef->updateCode();
      ArrayVector outputs(funcDef->evaluateFunction(twalk,args,nargout));
      ser->putInt(outputs.size());
      for (int i=0;i<outputs.size();i++)
	ser->putArray(outputs[i]);
    }
  } catch (Exception &e) {
    e.printMe();
  }
}
