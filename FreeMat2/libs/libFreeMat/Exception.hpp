/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef __Exception_hpp__
#define __Exception_hpp__

#include <string>
namespace FreeMat {

class Interpreter;

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
    void printMe(Interpreter *eval);
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
