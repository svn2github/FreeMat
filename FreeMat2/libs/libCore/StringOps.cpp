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

#include "Core.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include "Malloc.hpp"

namespace FreeMat {
  //!
  //@Module STRCMP String Compare Function
  //@@Usage
  //Compares two strings for equality.  The general
  //syntax for its use is
  //@[
  //  p = strcmp(x,y)
  //@]
  //where @|x| and @|y| are two strings.  Returns @|true| if @|x|
  //and @|y| are the same size, and are equal (as strings).  Otherwise,
  //it returns @|false|.
  //@@Example
  //The following piece of code compares two strings:
  //@<
  //x1 = 'astring';
  //x2 = 'bstring';
  //x3 = 'astring';
  //strcmp(x1,x2)
  //strcmp(x1,x3)
  //@>
  //!
  ArrayVector StrCmpFunction(int nargout, const ArrayVector& arg) {
    Array retval, arg1, arg2;
    if (arg.size() != 2)
      throw Exception("strcmp function requires two string arguments");
    arg1 = arg[0];
    arg2 = arg[1];
    if (!(arg1.isString()))
      throw Exception("strcmp function requires two string arguments");
    if (!(arg2.isString()))
      throw Exception("strcmp function requires two string arguments");
    if (!(arg1.getDimensions().equals(arg2.getDimensions())))
      retval = Array::logicalConstructor(false);
    else {
      char *s1 = arg1.getContentsAsCString();
      char *s2 = arg2.getContentsAsCString();
      retval = Array::logicalConstructor(strcmp(s1,s2)==0);
      Free(s1);
      Free(s2);
    }
    ArrayVector o;
    o.push_back(retval);
    return o;
  }
}
