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

#ifndef __CLIThread_hpp__
#define __CLIThread_hpp__

#include "wx/thread.h"
#include "Command.hpp"

#include "Interface.hpp"
class wxApp;

namespace FreeMat {
  
  /** Worker thread that handles the command line interface.
   * This is a simple worker thread that handles the command
   * line interface in the GUI code.  It is initialized by the
   * application, and then communicates with the GUI by sending
   * Command objects to it.
   */
  class CLIThread : public wxThread
  {
    wxApp* m_serv;
    Interface *io;
  public:
    /**
     * Create a CLI thread object with the given application pointer,
     * and the given I/O interface.
     */
    CLIThread(wxApp* serv, Interface* a_io);
    /**
     * This is the "run" function for the worker thread.  It
     * creates the global and base contexts, initializes the built-in
     * functions, and sets up the PathSearcher object.
     */
    virtual void *Entry();    
    /**
     * A virtual destructor.
     */
    virtual ~CLIThread();
    virtual void OnExit();
  }; 
}
#endif
