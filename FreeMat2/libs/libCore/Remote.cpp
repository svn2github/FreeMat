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

#include "ServerSocket.hpp"
#include "Malloc.hpp"
#include "Array.hpp"
#include "Socket.hpp"
#include "Serialize.hpp"
#include "HandleList.hpp"

namespace FreeMat {
  ServerSocket *serv;
  typedef Serialize* Serptr;
  HandleList<Serptr> remoteConnections;

  ArrayVector RInitFunction(int nargout, const ArrayVector& arg) {
    serv = new ServerSocket(5098);
    return ArrayVector();
  }

  ArrayVector RStartFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("rstart function takes one string argument");
    char *systemArg = arg[0].getContentsAsCString();
    int status = system(systemArg);
    if (status < 0)
      throw Exception("remote start command failed");
    // Open a server socket
    Serialize *ser = new Serialize(serv->AcceptConnection());
    ser->handshakeServer();
    ArrayVector retval;
    retval.push_back(Array::uint32Constructor(remoteConnections.assignHandle(ser)));
    return retval;
  }

  ArrayVector RCallFunction(int nargout, const ArrayVector& arg) {
    int commhandle;
    Serialize *ser;
    int i;
    if (arg.size() < 2) 
      throw Exception("rcall function requires at least two arguments, the remote handle and the function to call");
    Array t(arg[0]);
    commhandle = t.getContentsAsIntegerScalar();
    ser = remoteConnections.lookupHandle(commhandle);
    Array s(arg[1]);
    if (!s.isString())
      throw Exception("second argument must be a string");
    ser->putString(arg[1].getContentsAsCString());
    ser->putInt(nargout);
    ser->putInt(arg.size()-2);
    for (i=2;i<arg.size();i++)
      ser->putArray(arg[i]);
    // Return the handle
    ArrayVector retval;
    retval.push_back(Array::uint32Constructor(commhandle));
    return retval;
  }

  ArrayVector RRetFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("rret function requires exactly one argument");
    Array t(arg[0]);
    int commhandle = t.getContentsAsIntegerScalar();
    Serialize *ser = remoteConnections.lookupHandle(commhandle);
    // Get the reply...
    int outCount;
    int i;
    outCount = ser->getInt();
    ArrayVector output;
    for (i=0;i<outCount;i++) {
      Array vec;
      ser->getArray(vec);
      output.push_back(vec);
    }    
    return output;
  }
}
