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
  public:
    Interface() {};
    virtual ~Interface() {};
    /**
     *  Set the context to be handled by this interface
     */
    virtual void setContext(Context *ctxt) = 0;
    /**
     *  Get the current path set for the interface.
     */
    virtual std::string getPath() = 0;
    /**
     *  Set the path for the interface.
     */
    virtual void setPath(std::string) = 0;
    /**
     *  Force a rescan of the current path to look for 
     *  new function files.
     */
    virtual void rescanPath() = 0;
    /**
     *  Get a line of input from the user with the
     *  given prompt.
     */
    virtual char* getLine(const char* prompt) = 0;
    /**
     *  Return the width of the current "terminal" in
     *  characters.
     */
    virtual int getTerminalWidth() = 0;
    /**
     *  Output the following text message.
     */
    virtual void outputMessage(const char* msg) = 0;
    /**
     *  Output the following error message.
     */
    virtual void errorMessage(const char* msg) = 0;
    /**
     *  Output the following warning message.
     */
    virtual void warningMessage(const char* msg) = 0;
    /**
     * Set the context for an error message to the
     * following string.
     */
    virtual void setMessageContext(const char* msg) = 0;
    /**
     * Push the current message context onto the stack.
     */
    virtual void pushMessageContext() = 0;
    /**
     * Pop the current message context from the stack.
     */ 
    virtual void popMessageContext() = 0;
    /**
     * Get a copy of the message context stack.
     */
    virtual std::vector<std::string> getMessageContextStack() = 0;
    /**
     * Clear the message context stack.
     */
    virtual void clearMessageContextStack() = 0;
  };
}

#endif
