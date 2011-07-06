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

#include "Math.hpp"

//!
//@Module INV Invert Matrix
//@@Section TRANSFORMS
//@@Usage
//Inverts the argument matrix, provided it is square and invertible.
//The syntax for its use is
//@[
//   y = inv(x)
//@]
//Internally, the @|inv| function uses the matrix divide operators.
//For sparse matrices, a sparse matrix solver is used.
//@@Example
//Here we invert some simple matrices
//@<
//a = randi(zeros(3),5*ones(3))
//b = inv(a)
//a*b
//b*a
//@>
//@@Tests
//@$near#y1=inv(x1)
//@@Signature
//function inv InvFunction jitsafe
//inputs x
//outputs y
//!
ArrayVector InvFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("inv function needs at least one argument");
  return ArrayVector(InvertMatrix(arg[0]));
}
