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
#include "LAPACK.hpp"

//!
//@Module XNRM2 BLAS Norm Calculation
//@@Section ARRAY
//@@Usage
//Calculates the 2-norm of a vector.  The syntax for its use
//is
//@[
//   y = xnrm2(A)
//@]
//where @|A| is the n-dimensional array to analyze.  This form
//uses the underlying BLAS implementation to compute the 2-norm.
//@@Signature
//function xnrm2 XNrm2Function
//inputs A
//outputs y
//!
ArrayVector XNrm2Function(int nargout, const ArrayVector& arg) {
  int one = 1;
  if (arg.size() < 1)
    throw Exception("xnrm2 requires at least one argument");
  Array A(arg[0].asDenseArray());
  if (A.dataClass() != Float)
    A = A.toClass(Double);
  int len = int(A.length());
  switch (A.dataClass()) {
  default:
    throw Exception("xnrm2 does not support arguments of type " + A.className());
  case Float:
    if (A.allReal())
      return ArrayVector(Array(snrm2_(&len,A.real<float>().data(),&one)).toClass(Float));
    else
      return ArrayVector(Array(scnrm2_(&len,A.fortran<float>().data(),&one)).toClass(Float));
  case Double:
    if (A.allReal())
      return ArrayVector(Array(dnrm2_(&len,A.real<double>().data(),&one)));
    else
      return ArrayVector(Array(dznrm2_(&len,A.fortran<double>().data(),&one)));
  }
}
