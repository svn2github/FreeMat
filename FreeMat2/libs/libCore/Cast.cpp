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

#include "Array.hpp"

namespace FreeMat {
  //!
  //@Module LOGICAL Convert to Logical
  //@@Section TYPECAST
  //@@Usage
  //Converts the argument to a logical array.  The syntax
  //for its use is
  //@[
  //   y = logical(x)
  //@]
  //where @|x| is an @|n|-dimensional numerical array.  Any nonzero 
  //element maps to a logical 1.
  //@@Example
  //Here we convert an integer array to @|logical|:
  //@<
  //logical([1,2,3,0,0,0,5,2,2])
  //@>
  //The sampe example with double precision values:
  //@<
  //logical([pi,pi,0,e,0,-1])
  //@>
  //!
  ArrayVector LogicalFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1) 
      throw Exception("type conversion function requires one argument");
    Array A(arg[0]);
    A.promoteType(FM_LOGICAL);
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }

  //!
  //@Module STRING Convert Array to String
  //@@Section TYPECAST
  //@@Usage
  //Converts the argument array into a string.  The syntax for its
  //use is
  //@[
  //   y = string(x)
  //@]
  //where @|x| is an @|n|-dimensional numerical array.
  //@@Example
  //Here we take an array containing ASCII codes for a string, and
  //convert it into a string.
  //@<
  //a = [104,101,108,108,111]
  //string(a)
  //@>
  //!
  ArrayVector StringFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("type conversion function requires one argument");
    Array A(arg[0]);
    A.promoteType(FM_STRING);
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }

  //!
  //@Module UINT8 Convert to Unsigned 8-bit Integer
  //@@Section TYPECAST
  //@@Usage
  //Converts the argument to an unsigned 8-bit Integer.  The syntax
  //for its use is
  //@[
  //   y = uint8(x)
  //@]
  //where @|x| is an @|n|-dimensional numerical array.  Conversion
  //follows the general C rules (e.g., if @|x| is outside the normal
  //range for an unsigned 8-bit integer of @|[0,255]|, the least significant
  //8 bits of @|x| are used after conversion to an integer).  Note that
  //both @|NaN| and @|Inf| both map to 0.
  //@@Example
  //The following piece of code demonstrates several uses of @|uint8|.
  //@<
  //uint8(200)
  //@>
  //In the next example, an integer outside the range  of the type is passed in.  The result is the 8 least significant bits of the argument.
  //@<
  //uint8(400)
  //@>
  //In the next example, a negative integer is passed in.  The result is the 8 least significant bits of the argument, \emph{after} taking the 2's complement.
  //@<
  //uint8(-100)
  //@>
  //In the next example, a positive double precision argument is passed in.  The result is the unsigned integer that is closest to the argument.
  //@<
  //uint8(pi)
  //@>
  //In the next example, a complex argument is passed in.  The result is the unsigned integer that is closest to the real part of the argument.
  //@<
  //uint8(5+2*i)
  //@>
  //In the next example, a string argument is passed in.  The string argument is converted into an integer array corresponding to the ASCII values of each character.
  //@<
  //uint8('helo')
  //@>
  //In the last example, a cell-array is passed in.  For cell-arrays and structure arrays, the result is an error.
  //@<
  //uint8({4})
  //@>
  //!
  ArrayVector UInt8Function(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1) 
      throw Exception("type conversion function requires one argument");
    Array A(arg[0]);
    A.promoteType(FM_UINT8);
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }

  //!
  //@Module UINT16 Convert to Unsigned 16-bit Integer
  //@@Section TYPECAST
  //@@Usage
  //Converts the argument to an unsigned 16-bit Integer.  The syntax
  //for its use is
  //@[
  //   y = uint16(x)
  //@]
  //where @|x| is an @|n|-dimensional numerical array.  Conversion
  //follows the general C rules (e.g., if @|x| is outside the normal
  //range for an unsigned 16-bit integer of @|[0,65535]|, the least significant
  //16 bits of @|x| are used after conversion to an integer).  Note that
  //both @|NaN| and @|Inf| both map to 0.
  //@@Example
  //The following piece of code demonstrates several uses of @|uint16|.
  //@<
  //uint16(200)
  //@>
  //In the next example, an integer outside the range  of the type is passed in.  The result is the 16 least significant bits of the argument.
  //@<
  //uint16(99400)
  //@>
  //In the next example, a negative integer is passed in.  The result is the 16 least significant bits of the argument, \emph{after} taking the 2's complement.
  //@<
  //uint16(-100)
  //@>
  //In the next example, a positive double precision argument is passed in.  The result is the unsigned integer that is closest to the argument.
  //@<
  //uint16(pi)
  //@>
  //In the next example, a complex argument is passed in.  The result is the unsigned integer that is closest to the real part of the argument.
  //@<
  //uint16(5+2*i)
  //@>
  //In the next example, a string argument is passed in.  The string argument is converted into an integer array corresponding to the ASCII values of each character.
  //@<
  //uint16('helo')
  //@>
  //In the last example, a cell-array is passed in.  For cell-arrays and structure arrays, the result is an error.
  //@<
  //uint16({4})
  //@>
  //!
  ArrayVector UInt16Function(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1) 
      throw Exception("type conversion function requires one argument");
    Array A(arg[0]);
    A.promoteType(FM_UINT16);
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }

  //!
  //@Module UINT32 Convert to Unsigned 32-bit Integer
  //@@Section TYPECAST
  //@@Usage
  //Converts the argument to an unsigned 32-bit Integer.  The syntax
  //for its use is
  //@[
  //   y = uint32(x)
  //@]
  //where @|x| is an @|n|-dimensional numerical array.  Conversion
  //follows the general C rules (e.g., if @|x| is outside the normal
  //range for an unsigned 32-bit integer of @|[0,4294967295]|, the least 
  //significant 32 bits of @|x| are used after conversion to an integer).  
  //Note that both @|NaN| and @|Inf| both map to 0.
  //@@Example
  //The following piece of code demonstrates several uses of @|uint32|.
  //@<
  //uint32(200)
  //@>
  //In the next example, an integer outside the range  of the type is passed in.  The result is the 32 least significant bits of the argument.
  //@<
  //uint32(40e9)
  //@>
  //In the next example, a negative integer is passed in.  The result is the 32 least significant bits of the argument, \emph{after} taking the 2's complement.
  //@<
  //uint32(-100)
  //@>
  //In the next example, a positive double precision argument is passed in.  The result is the unsigned integer that is closest to the argument.
  //@<
  //uint32(pi)
  //@>
  //In the next example, a complex argument is passed in.  The result is the unsigned integer that is closest to the real part of the argument.
  //@<
  //uint32(5+2*i)
  //@>
  //In the next example, a string argument is passed in.  The string argument is converted into an integer array corresponding to the ASCII values of each character.
  //@<
  //uint32('helo')
  //@>
  //In the last example, a cell-array is passed in.  For cell-arrays and structure arrays, the result is an error.
  //@<
  //uint32({4})
  //@>
  //!
  ArrayVector UInt32Function(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1) 
      throw Exception("type conversion function requires one argument");
    Array A(arg[0]);
    A.promoteType(FM_UINT32);
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }

  //!
  //@Module INT8 Convert to Signed 8-bit Integer
  //@@Section TYPECAST
  //@@Usage
  //Converts the argument to an signed 8-bit Integer.  The syntax
  //for its use is
  //@[
  //   y = int8(x)
  //@]
  //where @|x| is an @|n|-dimensional numerical array.  Conversion
  //follows the general C rules (e.g., if @|x| is outside the normal
  //range for a signed 8-bit integer of @|[-128,127]|, the least significant
  //8 bits of @|x| are used after conversion to a signed integer).  Note that
  //both @|NaN| and @|Inf| both map to 0.
  //@@Example
  //The following piece of code demonstrates several uses of @|int8|.  First, the routine uses
  //@<
  //int8(100)
  //int8(-100)
  //@>
  //In the next example, an integer outside the range  of the type is passed in.  The result is the 8 least significant bits of the argument.
  //@<
  //int8(400)
  //@>
  //In the next example, a positive double precision argument is passed in.  The result is the signed integer that is closest to the argument.
  //@<
  //int8(pi)
  //@>
  //In the next example, a complex argument is passed in.  The result is the signed integer that is closest to the real part of the argument.
  //@<
  //int8(5+2*i)
  //@>
  //In the next example, a string argument is passed in.  The string argument is converted into an integer array corresponding to the ASCII values of each character.
  //@<
  //int8('helo')
  //@>
  //In the last example, a cell-array is passed in.  For cell-arrays and structure arrays, the result is an error.
  //@<
  //int8({4})
  //@>
  //!
  ArrayVector Int8Function(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1) 
      throw Exception("type conversion function requires one argument");
    Array A(arg[0]);
    A.promoteType(FM_INT8);
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }

  //!
  //@Module INT16 Convert to Signed 16-bit Integer
  //@@Section TYPECAST
  //@@Usage
  //Converts the argument to an signed 16-bit Integer.  The syntax
  //for its use is
  //@[
  //   y = int16(x)
  //@]
  //where @|x| is an @|n|-dimensional numerical array.  Conversion
  //follows the general C rules (e.g., if @|x| is outside the normal
  //range for a signed 16-bit integer of @|[-32768,32767]|, the least significant
  //16 bits of @|x| are used after conversion to a signed integer).  Note that
  //both @|NaN| and @|Inf| both map to 0.
  //@@Example
  //The following piece of code demonstrates several uses of @|int16|.  First, the routine uses
  //@<
  //int16(100)
  //int16(-100)
  //@>
  //In the next example, an integer outside the range  of the type is passed in.  The result is the 16 least significant bits of the argument.
  //@<
  //int16(40000)
  //@>
  //In the next example, a positive double precision argument is passed in.  The result is the signed integer that is closest to the argument.
  //@<
  //int16(pi)
  //@>
  //In the next example, a complex argument is passed in.  The result is the signed integer that is closest to the real part of the argument.
  //@<
  //int16(5+2*i)
  //@>
  //In the next example, a string argument is passed in.  The string argument is converted into an integer array corresponding to the ASCII values of each character.
  //@<
  //int16('helo')
  //@>
  //In the last example, a cell-array is passed in.  For cell-arrays and structure arrays, the result is an error.
  //@<
  //int16({4})
  //@>
  //!
  ArrayVector Int16Function(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1) 
      throw Exception("type conversion function requires one argument");
    Array A(arg[0]);
    A.promoteType(FM_INT16);
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }

  //!
  //@Module INT32 Convert to Signed 32-bit Integer
  //@@Section TYPECAST
  //@@Usage
  //Converts the argument to an signed 32-bit Integer.  The syntax
  //for its use is
  //@[
  //   y = int32(x)
  //@]
  //where @|x| is an @|n|-dimensional numerical array.  Conversion
  //follows the general C rules (e.g., if @|x| is outside the normal
  //range for a signed 32-bit integer of @|[-2147483648,2147483647]|, the least significant
  //32 bits of @|x| are used after conversion to a signed integer).  Note that
  //both @|NaN| and @|Inf| both map to 0.
  //@@Example
  //The following piece of code demonstrates several uses of @|int32|.  First, the routine uses
  //@<
  //int32(100)
  //int32(-100)
  //@>
  //In the next example, an integer outside the range  of the type is passed in.  The result is the 32 least significant bits of the argument.
  //@<
  //int32(40e9)
  //@>
  //In the next example, a positive double precision argument is passed in.  The result is the signed integer that is closest to the argument.
  //@<
  //int32(pi)
  //@>
  //In the next example, a complex argument is passed in.  The result is the signed integer that is closest to the real part of the argument.
  //@<
  //int32(5+2*i)
  //@>
  //In the next example, a string argument is passed in.  The string argument is converted into an integer array corresponding to the ASCII values of each character.
  //@<
  //int32('helo')
  //@>
  //In the last example, a cell-array is passed in.  For cell-arrays and structure arrays, the result is an error.
  //@<
  //int32({4})
  //@>
  //!
  ArrayVector Int32Function(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1) 
      throw Exception("type conversion function requires one argument");
    Array A(arg[0]);
    A.promoteType(FM_INT32);
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }

  //!
  //@Module SINGLE Convert to 32-bit Floating Point
  //@@Section TYPECAST
  //@@Usage
  //A synonym for the @|float| function, converts the argument to
  //a 32-bit floating point number.  The syntax
  //for its use is
  //@[
  //   y = float(x)
  //@]
  //where @|x| is an @|n|-dimensional numerical array.  Conversion 
  //follows the general C rules.  Note that both @|NaN| and @|Inf| 
  //are both preserved under type conversion.
  //!

  //!
  //@Module FLOAT Convert to 32-bit Floating Point
  //@@Section TYPECAST
  //@@Usage
  //Converts the argument to a 32-bit floating point number.  The syntax
  //for its use is
  //@[
  //   y = float(x)
  //@]
  //where @|x| is an @|n|-dimensional numerical array.  Conversion follows the general C rules.  Note that both @|NaN| and @|Inf| are both preserved under type conversion.
  //@@Example
  //The following piece of code demonstrates several uses of @|float|.  First, we convert from an integer (the argument is an integer because no decimal is present):
  //@<
  //float(200)
  //@>
  //In the next example, a double precision argument is passed in (the presence of a decimal without the @|f| suffix implies double precision).
  //@<
  //float(400.0)
  //@>
  //In the next example, a dcomplex argument is passed in.  The result is the real part of the argument, and in this context, @|float| is equivalent to the function @|real|.
  //@<
  //float(3.0+4.0*i)
  //@>
  //In the next example, a string argument is passed in.  The string argument is converted into an integer array corresponding to the ASCII values of each character.
  //@<
  //float('helo')
  //@>
  //In the last example, a cell-array is passed in.  For cell-arrays and structure arrays, the result is an error.
  //@<
  //float({4})
  //@>
  //!
  ArrayVector FloatFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1) 
      throw Exception("type conversion function requires one argument");
    Array A(arg[0]);
    A.promoteType(FM_FLOAT);
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }

  //!
  //@Module DOUBLE Convert to 64-bit Floating Point
  //@@Section TYPECAST
  //@@Usage
  //Converts the argument to a 64-bit floating point number.  The syntax
  //for its use is
  //@[
  //   y = double(x)
  //@]
  //where @|x| is an @|n|-dimensional numerical array.  Conversion follows the general C rules.  Note that both @|NaN| and @|Inf| are both preserved under type conversion.
  //@@Example
  //The following piece of code demonstrates several uses of @|double|.  First, we convert from an integer (the argument is an integer because no decimal is present):
  //@<
  //double(200)
  //@>
  //In the next example, a single precision argument is passed in (the presence of  the @|f| suffix implies single precision).
  //@<
  //double(400.0f)
  //@>
  //In the next example, a dcomplex argument is passed in.  The result is the real part of the argument, and in this context, @|double| is equivalent to the function @|real|.
  //@<
  //double(3.0+4.0*i)
  //@>
  //In the next example, a string argument is passed in.  The string argument is converted into an integer array corresponding to the ASCII values of each character.
  //@<
  //double('helo')
  //@>
  //In the last example, a cell-array is passed in.  For cell-arrays and structure arrays, the result is an error.
  //@<
  //double({4})
  //@>
  //!
  ArrayVector DoubleFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1) 
      throw Exception("type conversion function requires one argument");
    Array A(arg[0]);
    A.promoteType(FM_DOUBLE);
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }

  //!
  //@Module COMPLEX Convert to 32-bit Complex Floating Point
  //@@Section TYPECAST
  //@@Usage
  //Converts the argument to a 32-bit complex floating point number.  The syntax
  //for its use is
  //@[
  //   y = complex(x)
  //@]
  //where @|x| is an @|n|-dimensional numerical array.  Conversion follows the general C rules.  Note that both @|NaN| and @|Inf| in the real and imaginary parts are both preserved under type conversion.
  //@@Example
  //The following piece of code demonstrates several uses of @|complex|.  First, we convert from an integer (the argument is an integer because no decimal is present):
  //@<
  //complex(200)
  //@>
  //In the next example, a double precision argument is passed in (the presence of a decimal without the @|f| suffix implies double precision).
  //@<
  //complex(400.0)
  //@>
  //In the next example, a dcomplex argument is passed in. 
  //@<
  //complex(3.0+4.0*i)
  //@>
  //In the next example, a string argument is passed in.  The string argument is converted into an integer array corresponding to the ASCII values of each character.
  //@<
  //complex('he')
  //@>
  //In the next example, the @|NaN| argument is converted.
  //@<
  //complex(nan)
  //@>
  //In the last example, a cell-array is passed in.  For cell-arrays and structure arrays, the result is an error.
  //@<
  //complex({4})
  //@>
  //!
  ArrayVector ComplexFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1) 
      throw Exception("type conversion function requires one argument");
    Array A(arg[0]);
    A.promoteType(FM_COMPLEX);
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }

  //!
  //@Module DCOMPLEX Convert to 32-bit Complex Floating Point
  //@@Section TYPECAST
  //@@Usage
  //Converts the argument to a 32-bit complex floating point number.  The syntax
  //for its use is
  //@[
  //   y = dcomplex(x)
  //@]
  //where @|x| is an @|n|-dimensional numerical array.  Conversion follows the general C rules.  Note that both @|NaN| and @|Inf| in the real and imaginary parts are both preserved under type conversion.
  //@@Example
  //The following piece of code demonstrates several uses of @|dcomplex|.  First, we convert from an integer (the argument is an integer because no decimal is present):
  //@<
  //dcomplex(200)
  //@>
  //In the next example, a double precision argument is passed in (the presence of a decimal without the @|f| suffix implies double precision).
  //@<
  //dcomplex(400.0)
  //@>
  //In the next example, a complex argument is passed in. 
  //@<
  //dcomplex(3.0+4.0*i)
  //@>
  //In the next example, a string argument is passed in.  The string argument is converted into an integer array corresponding to the ASCII values of each character.
  //@<
  //dcomplex('h')
  //@>
  //In the next example, the @|NaN| argument is converted.
  //@<
  //dcomplex(nan)
  //@>
  //In the last example, a cell-array is passed in.  For cell-arrays and structure arrays, the result is an error.
  //@<
  //dcomplex({4})
  //@>
  //!
  ArrayVector DcomplexFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1) 
      throw Exception("type conversion function requires one argument");
    Array A(arg[0]);
    A.promoteType(FM_DCOMPLEX);
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }

  //!
  //@Module TYPEOF Determine the Type of an Argument
  //@@Section TYPECAST
  //@@Usage
  //Returns a string describing the type of an array.  The syntax for its use is
  //@[
  //   y = typeof(x),
  //@]
  //The returned string is one of
  //\begin{itemize}
  //\item @|'cell'| for cell-arrays
  //\item @|'struct'| for structure-arrays
  //\item @|'logical'| for logical arrays
  //\item @|'uint8'| for unsigned 8-bit integers
  //\item @|'int8'| for signed 8-bit integers
  //\item @|'uint16'| for unsigned 16-bit integers
  //\item @|'int16'| for signed 16-bit integers
  //\item @|'uint32'| for unsigned 32-bit integers
  //\item @|'int32'| for signed 32-bit integers
  //\item @|'float'| for 32-bit floating point numbers
  //\item @|'double'| for 64-bit floating point numbers
  //\item @|'complex'| for complex floating point numbers with 32-bits per field
  //\item @|'dcomplex'| for complex floating point numbers with 64-bits per field
  //\item @|'string'| for string arrays
  //\end{itemize}
  //@@Example
  //The following piece of code demonstrates the output of the @|typeof| command for each possible type.  The first example is with a simple cell array.
  //@<
  //typeof({1})
  //@>
  //The next example uses the @|struct| constructor to make a simple scalar struct.
  //@<
  //typeof(struct('foo',3))
  //@>
  //The next example uses a comparison between two scalar integers to generate a scalar logical type.
  //@<
  //typeof(3>5)
  //@>
  //For the smaller integers, and the 32-bit unsigned integer types, the typecast operations are used to generate the arguments.
  //@<
  //typeof(uint8(3))
  //typeof(int8(8))
  //typeof(uint16(3))
  //typeof(int16(8))
  //typeof(uint32(3))
  //@>
  //The 32-bit signed integer type is the default for integer arguments.
  //@<
  //typeof(-3)
  //typeof(8)
  //@>
  //Float, double, complex and double-precision complex types can be created using the suffixes.
  //@<
  //typeof(1.0f)
  //typeof(1.0D)
  //typeof(1.0f+i)
  //typeof(1.0D+2.0D*i)
  //@>
  //!
  ArrayVector TypeOfFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("typeof function requires exactly one argument");
    Array retval;
    Class t = arg[0].getDataClass();
    switch(t) {
    case FM_CELL_ARRAY:
      retval = Array::stringConstructor(std::string("cell"));
      break;
    case FM_STRUCT_ARRAY:
      retval = Array::stringConstructor(std::string("struct"));
      break;
    case FM_LOGICAL:
      retval = Array::stringConstructor(std::string("logical"));
      break;
    case FM_UINT8:
      retval = Array::stringConstructor(std::string("uint8"));
      break;
    case FM_INT8:
      retval = Array::stringConstructor(std::string("int8"));
      break;
    case FM_UINT16:
      retval = Array::stringConstructor(std::string("uint16"));
      break;
    case FM_INT16:
      retval = Array::stringConstructor(std::string("int16"));
      break;
    case FM_UINT32:
      retval = Array::stringConstructor(std::string("uint32"));
      break;
    case FM_INT32:
      retval = Array::stringConstructor(std::string("int32"));
      break;
    case FM_FLOAT:
      retval = Array::stringConstructor(std::string("float"));
      break;
    case FM_DOUBLE:
      retval = Array::stringConstructor(std::string("double"));
      break;
    case FM_COMPLEX:
      retval = Array::stringConstructor(std::string("complex"));
      break;
    case FM_DCOMPLEX:
      retval = Array::stringConstructor(std::string("dcomplex"));
      break;
    case FM_STRING:
      retval = Array::stringConstructor(std::string("string"));
      break;
    }
    ArrayVector r;
    r.push_back(retval);
    return r;
  }
}
