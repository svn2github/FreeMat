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

#include "Module.hpp"
#include "FunctionDef.hpp"
#include "Exception.hpp"
#include "Context.hpp"
#include "ParserInterface.hpp"
#include "WalkTree.hpp"
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/dynlib.h"
#include "wx/filefn.h"

namespace FreeMat {

  extern Context *context;
//   SymbolTable<void*> libPointers;
  SymbolTable<wxDynamicLibrary*> libPointers;

  ArrayVector LoadFunction(int c_nargout,const ArrayVector& narg) throw(Exception){
    char *libfile;
    char *symbolName;
    char *funcName;
    int nargin;
    int nargout;
    ArrayVector arg(narg);

    if (arg.size() < 2) 
      throw Exception("Must supply at least the library file and symbol name");
    libfile = arg[0].getContentsAsCString();
    symbolName = arg[1].getContentsAsCString();
    if (arg.size() < 5)
      nargout = 0;
    else
      nargout = (int) arg[4].getContentsAsIntegerScalar();
    if (arg.size() < 4)
      nargin = 0;
    else
      nargin = (int) arg[3].getContentsAsIntegerScalar();
    if (arg.size() < 3)
      funcName = symbolName;
    else
      funcName = arg[2].getContentsAsCString();
    wxDynamicLibrary *lib = new wxDynamicLibrary(libfile);
    if (!lib->IsLoaded())
      throw Exception(std::string("Unable to open module: ") + ((const char *)libfile));
    void *func = lib->GetSymbol(symbolName);
    if (func == NULL)
      throw Exception(std::string("Unable to find symbol ") + ((const char*) symbolName));
    BuiltInFunctionDef *fdef = new BuiltInFunctionDef;
    fdef->retCount = nargout;
    fdef->argCount = nargin;
    fdef->name = strdup((const char*) funcName);
    fdef->fptr = (BuiltInFuncPtr) func;
    context->insertFunctionGlobally(fdef);
    return ArrayVector();
  }
  
  void skipWS(char* &cp) {
    while (*cp == ' ' || *cp == '\t' || *cp == '\n')
      cp++;
  }
  
  const char* matchTest(char* &cp, const char* tmplate) {
    if (strncmp(cp,tmplate,strlen(tmplate)) == 0) {
      cp += strlen(tmplate);
      return tmplate;
    } else 
      return NULL;
  }

  const char* parseTypeName(char* &cp) {
    const char* rp;
    skipWS(cp);
    if (rp = matchTest(cp,"int8"))
      return rp;
    if (rp = matchTest(cp,"uint8"))
      return rp;
    if (rp = matchTest(cp,"int16"))
      return rp;
    if (rp = matchTest(cp,"uint16"))
      return rp;
    if (rp = matchTest(cp,"int32"))
      return rp;
    if (rp = matchTest(cp,"uint32"))
      return rp;
    if (rp = matchTest(cp,"float"))
      return rp;
    if (rp = matchTest(cp,"complex"))
      return rp;
    if (rp = matchTest(cp,"double"))
      return rp;
    if (rp = matchTest(cp,"dcomplex"))
      return rp;
    if (rp = matchTest(cp,"string"))
      return rp;
    return NULL;
  }

  char* parseArgumentName(char* &cp) {
    bool byRef;
    char *rp, *op;
    int identLength;
    skipWS(cp);
    byRef = false;
    if (*cp == '&') {
      byRef = true;
      cp++;
    }
    skipWS(cp);
    if (!isalpha(*cp))
      throw Exception(std::string("malformed import function prototype") + 
		      " - error starting at " + cp);
    rp = cp;
    cp++;
    while (isalnum(*cp) || (*cp == '_'))
      cp++; 
    identLength = (cp - rp);
    if (byRef) {
      op = (char*) malloc(identLength+2);
      op[0] = '&';
      op[identLength+1] = 0;
      memcpy(op+1,rp,identLength);
    } else {
      op = (char*) malloc(identLength+1);
      op[identLength] = 0;
      memcpy(op,rp,identLength);
    }
    return op;
  }

  char* parseBoundsCheck(char* &cp) {
    int bracketDepth;
    if (*cp != '[') return NULL;
    cp++;
    bracketDepth = 1;
    char *rp;
    rp = cp;
    while ((bracketDepth > 0) && (*cp != 0)) {
      if (*cp == '[') bracketDepth++;
      if (*cp == ']') bracketDepth--;
      cp++;
    }
    if (bracketDepth > 0)
      throw Exception(std::string("malformed bounds check - error starting at ") + 
		      cp);
    char *op;
    int bcLength;
    bcLength = (cp - rp);
    op = (char*) malloc(bcLength+1);
    memcpy(op,rp,bcLength-1);
    op[bcLength-1] = '\n';
    op[bcLength] = 0;
    return op;
  }
  
  ArrayVector ImportFunction(int nargout, const ArrayVector& arg, 
			     WalkTree* eval) throw(Exception) {
    char *libfile;
    char *symbolname;
    char *funcname;
    char *rettype;
    char *arglist;
    wxPathList plist;
    wxString libfullpath;

    if (arg.size() < 5)
      throw Exception(std::string("import requires 5 arguments:") + 
		      "library name, symbol name, imported function name" +
		      "return type, argument list");
    plist.AddEnvList("FREEMAT_PATH");
    libfile = arg[0].getContentsAsCString();
    libfullpath = plist.FindValidPath(libfile);
    symbolname = arg[1].getContentsAsCString();
    funcname = arg[2].getContentsAsCString();
    rettype = arg[3].getContentsAsCString();
    arglist = arg[4].getContentsAsCString();
    wxDynamicLibrary *lib;
    if (!libPointers.findSymbol(libfullpath.c_str(),lib)) {
      std::cout << "loading " << libfile << "\n";
      lib = new wxDynamicLibrary(libfullpath);    
      if (!lib->IsLoaded())
	throw Exception(std::string("Unable to open library ") + 
			libfullpath.c_str());
      libPointers.insertSymbol(libfullpath.c_str(),lib);
    }
    void* func = lib->GetSymbol(symbolname);
    if (func == NULL)
      throw Exception(std::string("Unable to find symbol ") + 
		      symbolname + " in library " + libfullpath.c_str());
    stringVector types;
    stringVector arguments;
    ASTPtrVector checks;
    /**
     * Parse the arglist...
     */
    char *cp;
    cp = arglist;

    while (*cp != 0) {
      /**
       * Parse off the typename
       */
      const char *tn;
      tn = parseTypeName(cp);
      if (tn == NULL) 
	throw Exception(std::string("illegal syntax in function") + 
			std::string(" prototype (argument list) - ") + 
			std::string("expecting a valid type name"));
      types.push_back(tn);
      char *bc;
      bc = parseBoundsCheck(cp);
      if (bc != NULL) {
	parseString(bc);
	checks.push_back(getParsedScriptBlock()->down->down);
      } else
	checks.push_back(NULL);
      char *ar;
      ar = parseArgumentName(cp);
      arguments.push_back(ar);
      if (*cp != 0) {
	if (*cp != ',') throw Exception("expecting a ',' between arugments");
	cp++;
      }
    }
    ImportedFunctionDef *fptr = new ImportedFunctionDef((void (*)())func,
							types,
							arguments,
							checks,
							rettype);
    fptr->name = funcname;
    eval->getContext()->insertFunctionGlobally(fptr);
    return ArrayVector();
  }
}
