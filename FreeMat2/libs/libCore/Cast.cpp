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
  ArrayVector UInt8Function(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1) 
      throw Exception("type conversion function requires one argument");
    Array A(arg[0]);
    A.promoteType(FM_UINT8);
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }

  ArrayVector UInt16Function(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1) 
      throw Exception("type conversion function requires one argument");
    Array A(arg[0]);
    A.promoteType(FM_UINT16);
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }

  ArrayVector UInt32Function(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1) 
      throw Exception("type conversion function requires one argument");
    Array A(arg[0]);
    A.promoteType(FM_UINT32);
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }

  ArrayVector Int8Function(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1) 
      throw Exception("type conversion function requires one argument");
    Array A(arg[0]);
    A.promoteType(FM_INT8);
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }

  ArrayVector Int16Function(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1) 
      throw Exception("type conversion function requires one argument");
    Array A(arg[0]);
    A.promoteType(FM_INT16);
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }

  ArrayVector Int32Function(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1) 
      throw Exception("type conversion function requires one argument");
    Array A(arg[0]);
    A.promoteType(FM_INT32);
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }

  ArrayVector FloatFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1) 
      throw Exception("type conversion function requires one argument");
    Array A(arg[0]);
    A.promoteType(FM_FLOAT);
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }

  ArrayVector DoubleFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1) 
      throw Exception("type conversion function requires one argument");
    Array A(arg[0]);
    A.promoteType(FM_DOUBLE);
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }

  ArrayVector ComplexFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1) 
      throw Exception("type conversion function requires one argument");
    Array A(arg[0]);
    A.promoteType(FM_COMPLEX);
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }

  ArrayVector DcomplexFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1) 
      throw Exception("type conversion function requires one argument");
    Array A(arg[0]);
    A.promoteType(FM_DCOMPLEX);
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }

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
