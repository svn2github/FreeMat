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

#ifndef __PathSearcher_hpp__
#define __PathSearcher_hpp__

#include <map>
#include <string>
#include <vector>
#include "Context.hpp"

namespace FreeMat {

  typedef std::vector<std::string> DirList;
  /**
   * The path searcher object searches a path list
   * as specified by env_var, and places entries into
   * the given context.  This is a place-holder object - 
   * although it works, I am going to replace it with
   * calls to the libtecla's file caching library.
   */
  class PathSearcher {
  public:
    PathSearcher(char* env_var, Context*);
    ~PathSearcher() {}
    void rescanPath();
  private:
    void scanDirectory(std::string);
    bool isDirectory(std::string) {}
    void procFile(char*, std::string);

    DirList dirTab;
    Context *context;
  };

  extern PathSearcher *globalPathSearcher;
}

#endif
