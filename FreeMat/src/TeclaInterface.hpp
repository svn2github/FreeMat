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

#ifndef __TeclaInterface_hpp__
#define __TeclaInterface_hpp__

#include "Interface.hpp"
#include "Context.hpp"
#include "Types.hpp"
#include "libtecla.h"

namespace FreeMat {
  /**
   * This is the class that abstracts the libTecla library - it
   * provides shell-like interface for the program to use (command
   * line editing, file searching, etc...).
   */
  class TeclaInterface : public Interface {
    /**
     * The libtecla object that provides the input string from the command
     * line.
     */
  public:
    GetLine *gl;
    PathCache *pc;
    PcaPathConf *ppc;
    CplFileConf *cfc;
    std::string pathSpec;
    Context *context;
    WordCompletion *cw;    
    stringVector transientFuncs;
    bool transientScan;
    void processFilename(const char *pathname);
  public:
    TeclaInterface();
    ~TeclaInterface();
    virtual void initialize(std::string path, Context *ctxt);
    virtual void rescanPath();
    virtual char *getLine(const char* prompt);
    virtual int getTerminalWidth();
    virtual void outputMessage(const char* msg);
    virtual void errorMessage(const char* msg);
    virtual void warningMessage(const char* msg);
    virtual void setMessageContext(const char* msg);  
  };
}


#endif
