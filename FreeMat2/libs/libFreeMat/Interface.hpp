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

#ifndef __Interface_hpp__
#define __Interface_hpp__

#include <string>
#include <vector>

namespace FreeMat {
  class Context;

  class Interface {
    Context *m_context;
    std::string m_path;
    std::vector<std::string> dirTab;    
    void scanDirectory(std::string scdir, bool, std::string prefixo);
    void procFile(std::string fname, std::string fullname, bool);
  public:
    Interface();
    virtual ~Interface();
    std::vector<std::string> GetCompletions(std::string line, int word_end, 
					    std::string &matchString);
    /**
     *  Get the current path set for the interface.
     */
    virtual std::string getPath();
    /**
     *  Set the path for the interface.
     */
    virtual void setPath(std::string);
    /**
     *  Set the context for the interface.
     */
    virtual void setContext(Context *ctxt);
    /**
     *  Force a rescan of the current path to look for 
     *  new function files.
     */
    virtual void rescanPath();
    /**
     *  Get a line of input from the user with the
     *  given prompt.
     */ 
    virtual char* getLine(std::string prompt) = 0;
    /**
     *  Return the width of the current "terminal" in
     *  characters.
     */
    virtual int getTerminalWidth() = 0;
    /**
     *  Output the following text message.
     */
    virtual void outputMessage(std::string msg) = 0;
    /**
     *  Output the following error message.
     */
    virtual void errorMessage(std::string msg) = 0;
    /**
     *  Output the following warning message.
     */
    virtual void warningMessage(std::string msg) = 0;
  };

  char* TildeExpand(char* path);
}

#endif
