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

#include "Exception.hpp"
#include "Array.hpp"
#include "Utils.hpp"
#include "IEEEFP.hpp"
#include "File.hpp"
#include "Serialize.hpp"
#include <math.h>
#include "Types.hpp"
#include <algorithm>
#include "SparseMatrix.hpp"
#include "Math.hpp"
#include "LAPACK.hpp"
#include "MemPtr.hpp"
#include <QtCore>


//!
//@Module PCODE Convert a Script or Function to P-Code
//@@Section FREEMAT
//@@Usage
//Writes out a script or function as a P-code function.
//The general syntax for its use is:
//@[
//   pcode fun1 fun2 ...
//@]
//The compiled functions are written to the current
//directory.
//@@Signature
//sfunction pcode PCodeFunction
//inputs varargin
//outputs none
//!
ArrayVector PCodeFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  throw Exception("P-code functions currently unsupported");
}





