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
#include "Interpreter.hpp"

//!
//@Module VERSION The Current Version Number
//@@Section FREEMAT
//@@Usage
//The @|version| function returns the current version number for
//FreeMat (as a string).  The general syntax for its use is
//@[
//    v = version
//@]
//@@Example
//The current version of FreeMat is
//@<
//version
//@>
//@@Signature
//function  version VersionFunction
//inputs none
//outputs verstring
//!
ArrayVector VersionFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(QString(VERSION)));
}


//!
//@Module VERSTRING The Current Version String
//@@Section FREEMAT
//@@Usage
//The @|verstring| function returns the current version string for
//FreeMat.  The general syntax for its use is
//@[
//    version = verstring
//@]
//@@Example
//The current version of FreeMat is
//@<
//verstring
//@>
//@@Signature
//function verstring VerStringFunction
//inputs none
//output verstring
//!
ArrayVector VerStringFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(Interpreter::getVersionString()));
}
