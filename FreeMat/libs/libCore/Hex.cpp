/*
 * Copyright (c) 2009 Samit Basu
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
#include "Array.hpp"
#include "IEEEFP.hpp"
#include "Algorithms.hpp"
#include <QString>

//!
//@Module HEX2DEC Convert Hexadecimal Numbers To Decimal
//@@Section ELEMENTARY
//@@Usage
//Converts a hexadecimal number (encoded as a string matrix) into integers.
//The syntax for its use is
//@[
//   y = hex2dec(x)
//@]
//where @|x| is a character matrix where each row represents an integer
//in hexadecimal form.  The output is of type @|Double|.
//@@Examples
//@<
//hex2dec('3ff')
//@>
//Or for a more complex example
//@<
//hex2dec(['0ff';'2de';'123'])
//@>
//@@Tests
//@$exact#y1=hex2dec(x1)
//@@Signature
//function hex2dec Hex2DecFunction
//inputs x
//outputs y
//!
ArrayVector Hex2DecFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    throw Exception("hex2dec requires an argument");
  if (arg[0].isEmpty()) return ArrayVector(EmptyConstructor());
  if (!arg[0].isString() && (arg[0].dataClass() != Double))
    throw Exception("hex2dec argument must be a string");
  Array x(arg[0]);
  if (x.dataClass() == Double) x = x.toClass(StringArray);
  if (x.isVector()) 
    return ArrayVector(Array(double(x.asString().toInt(0,16))));
  else {
    StringVector sv(StringVectorFromArray(x));
    Array rp(Double,NTuple(sv.size(),1));
    BasicArray<double> &qp(rp.real<double>());
    for (int i=0;i<sv.size();i++)
      qp[index_t(i+1)] = sv[i].toInt(0,16);
    return ArrayVector(rp);
  }
  return ArrayVector();
}

//!
//@Module DEC2HEX Convert Decimal Number to Hexadecimal
//@@Section ELEMENTARY
//@@Usage
//Converts an integer value into its hexadecimal representation.  The syntax
//for its use is
//@[
//   y = dec2hex(x)
//@]
//where @|x| is an integer (and is promoted to a 64-bit integer if it is not).
//The returned value @|y| is a string containing the hexadecimal representation
//of that integer.  If you require a minimum length for the hexadecimal
//representation, you can specify an optional second argument
//@[
//   y = dec2hex(x,n)
//@]
//where @|n| indicates the minimum number of digits in the representation.
//@@Example
//Here are some simple examples:
//@<
//dec2hex(1023)
//@>
//@<
//dec2hex(58128493)
//@>
//@@Tests
//@$exact#y1=dec2hex(x1)
//@@Signature
//function dec2hex Dec2HexFunction
//inputs x digits
//outputs y
//!
ArrayVector Dec2HexFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("dec2hex requires at least one argument");
  Array x = arg[0].asDenseArray().toClass(Int64);
  const BasicArray<int64> &xp(x.constReal<int64>());
  int n = 0;
  if (arg.size() > 1) n = arg[1].asInteger();
  if ((n < 0) || (n > 32)) 
    throw Exception("illegal number of digits requested in dec2hex function");
  StringVector ret;
  int maxlen = 0;
  for (index_t i=1;i<=xp.length();i++) {
    QString t = QString("%1").arg(xp[i],n,16,QChar('0'));
    maxlen = qMax(maxlen,t.size());
  }
  if (n == 0) n = maxlen;
  for (index_t i=1;i<=xp.length();i++) {
    QString t = QString("%1").arg(xp[i],n,16,QChar('0')).toUpper();
    ret << t;
  }
  return ArrayVector(StringArrayFromStringVector(ret,QChar(' ')));
}


//!
//@Module NUM2HEX Convert Numbers to IEEE Hex Strings
//@@Section ELEMENTARY
//@@Usage
//Converts single and double precision arrays to IEEE hex strings.  The
//syntax for its use is
//@[
//   y = num2hex(x)
//@]
//where @|x| is either a @|float| or @|double| array.  The output @|y| is
//a @|n-by-p| character array, where @|n| is the number of elements in @|x|,
//and @|p| is 16 for @|double| arrays, and 8 for @|single| arrays.
//@@Example
//Some interesting numbers
//@<
//num2hex([1 0 0.1 -pi inf nan])
//@>
//The same in single precision
//@<
//num2hex(float([1 0 0.1 -pi inf nan]))
//@>
//@@Tests
//@$exact#y1=num2hex(x1)
//@@Signature
//function num2hex Num2HexFunction
//inputs x
//outputs y
//!

template <typename T>
static inline Array Num2HexFunction(const BasicArray<T> &data) {
  if (data.isEmpty())
    return Array(StringArray,NTuple(0,16));
  StringVector st;
  for (index_t i=1;i<=data.length();i++)
    st.push_back(ToHexString(data[i]));
  return StringArrayFromStringVector(st,QChar(' '));
}

ArrayVector Num2HexFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("num2hex fucntion requires a single argument");
  if ((arg[0].dataClass() != Float) && (arg[0].dataClass() != Double))
    throw Exception("num2hex only works on single and double arrays");
  Array x = arg[0].asDenseArray();
  if (x.dataClass() == Float) 
    return ArrayVector(Num2HexFunction(x.constReal<float>()));
  else 
    return ArrayVector(Num2HexFunction(x.constReal<double>()));
}
