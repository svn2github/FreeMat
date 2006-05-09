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

#include "Core.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include "Malloc.hpp"

namespace FreeMat {
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
  //!
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
  //where @|source|, @|pattern| and @|replace| are all strings. 
  //@@Example
  //Here are some simple examples
  //@<
  //strrep('hello','he','be')
  //strrep(strrep('matlab is nice','matlab','freemat'),'nice','better')
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
