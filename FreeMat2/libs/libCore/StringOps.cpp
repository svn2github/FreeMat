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
  //@@Section STRING
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
      throw Exception("strcomp function requires two arguments");
    arg1 = arg[0];
    arg2 = arg[1];
    if (!(arg1.isString()))
      return singleArrayVector(Array::logicalConstructor(false));
    if (!(arg2.isString()))
      return singleArrayVector(Array::logicalConstructor(false));
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

  //!
  //@Module STRSTR String Search Function
  //@@Section STRING
  //@@Usage
  //Searches for the first occurance of one string inside another.
  //The general syntax for its use is
  //@[
  //   p = strstr(x,y)
  //@]
  //where @|x| and @|y| are two strings.  The returned integer @|p|
  //indicates the index into the string @|x| where the substring @|y|
  //occurs.  If no instance of @|y| is found, then @|p| is set to
  //zero.
  //@@Example
  //Some examples of @|strstr| in action
  //@<
  //strstr('hello','lo')
  //strstr('quick brown fox','own')
  //strstr('free stuff','lunch')
  //@>
  ArrayVector StrStrFunction(int nargout, const ArrayVector& arg) {
    Array retval, arg1, arg2;
    if (arg.size() != 2)
      throw Exception("strstr function requires two string arguments");
    arg1 = arg[0];
    arg2 = arg[1];
    if (!(arg1.isString()))
      throw Exception("strstr function requires two string arguments");
    if (!(arg2.isString()))
      throw Exception("strstr function requires two string arguments");
    char *s1 = arg1.getContentsAsCString();
    char *s2 = arg2.getContentsAsCString();
    char *cp;
    cp = strstr(s1,s2);
    int retndx;
    if (!cp)
      retndx = 0;
    else
      retndx = cp-s1+1;
    return singleArrayVector(Array::int32Constructor(retndx));
  }

  char* strrep(char* source, char* pattern, char* replace) {
    // Count how many instances of 'pattern' occur
    int instances = 0;
    char *cp = source;
    while (cp) {
      cp = strstr(cp,pattern);
      if (cp) {
	cp += strlen(pattern);
	instances++;
      }
    }
    // The output array should be large enough...
    int outlen = strlen(source) + instances*(strlen(replace) - strlen(pattern)) + 1;
    char *op = (char*) malloc(sizeof(char)*outlen);
    char *opt = op;
    // Retrace through the source array
    cp = source;
    char *lastp = source;
    while (cp) {
      cp = strstr(cp,pattern);
      if (cp) {
	memcpy(opt,lastp,(cp-lastp));
	opt += (cp-lastp);
	memcpy(opt,replace,strlen(replace));
	opt += strlen(replace);
	cp += strlen(pattern);
	lastp = cp;
	instances++;
      } else
	memcpy(opt,lastp,strlen(source)-(lastp-source)+1);
    }
    return op;
  }

  //!
  //@Module STRREP String Substitute Function
  //@@Section STRING
  //@@Usage
  //Replaces instances of one string in another string.  The general
  //syntax for its use is
  //@[
  //   p = strrep(source,pattern,replace)
  //@]
  //where @|source|, @|pattern| and @|replace| are all strings.  Optionally
  //they can be cell arrays of strings or scalar cell arrays.  
  //@@Example
  //Here are some simple examples
  //@<
  //strrep({'hello','hohew'},'he','be')
  //strrep({'hello','hohew'},'he',{'be'})
  //strrep({'hello','hohew'},'he',{'be','ce'})
  //@>
  //!
  ArrayVector StrRepFunction(int nargout, const ArrayVector& arg) {
    Array arg1, arg2, arg3;
    if (arg.size() != 3)
      throw Exception("strrep function requires three string arguments");
    arg1 = arg[0];
    arg2 = arg[1];
    arg3 = arg[2];
    if (!(arg1.isString()))
      throw Exception("strrep function requires three string arguments");
    if (!(arg2.isString()))
      throw Exception("strrep function requires three string arguments");
    if (!(arg3.isString()))
      throw Exception("strrep function requires three string arguments");
    char *s1 = arg1.getContentsAsCString();
    char *s2 = arg2.getContentsAsCString();
    char *s3 = arg3.getContentsAsCString();
    char *cp = strrep(s1,s2,s3);
    ArrayVector retval(singleArrayVector(Array::stringConstructor(cp)));
    free(cp);
    return retval;
  }
}
