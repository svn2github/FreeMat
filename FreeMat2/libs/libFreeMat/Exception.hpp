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

#ifndef __Exception_hpp__
#define __Exception_hpp__

#include "Interface.hpp"
#include <string>
namespace FreeMat {

/**
 * The exception class.  This is a minimal class for now that
 * allows for a hierarchical error structure (if desired) later
 * on.  Since we simply print most messages to the console,
 * the exception types are not encoded using RTTI...
 */
  class Exception {
    char *msg;
  public:
    /**
     * Construct an exception object with a given C-string.
     */
    Exception(const char*msg_in);
    /**
     * Construct an exception object with a given STL-string.
     */
    Exception(std::string msg_in);
    /**
     * Copy constructor.
     */
    Exception(const Exception& copy);
    /**
     * Assignment operator.
     */
    void operator=(const Exception &copy);
    /**
     * Standard destructor.
     */
    ~Exception();
    /**
     * Output the contents of the exception to the console.
     */
    void printMe(Interface *io);
    /**
     *
     */
    bool matches(const char *tst_msg);
    /**
     * Get a copy of the message member function.  Receiver
     * is responsibile for Free-ing the array when done.
     */
    char* getMessageCopy();
  };

  void printExceptionCount();
}

#endif
