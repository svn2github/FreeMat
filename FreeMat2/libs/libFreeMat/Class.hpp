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
#ifndef __Class_hpp__
#define __Class_hpp__

#include "Array.hpp"
#include "Interpreter.hpp"
#include "Types.hpp"

class UserClass {
  rvstring fieldNames;
  rvstring parentClasses;
public:
  UserClass();
  UserClass(rvstring fields, rvstring parents);
  bool matchClass(UserClass test);
  rvstring getParentClasses();
  ~UserClass();
};

Array ClassTrinaryOperator(Array a, Array b, Array c, std::string funcname, Interpreter* eval);

Array ClassBinaryOperator(Array a, Array b, std::string funcname,
			  Interpreter* eval);

Array ClassUnaryOperator(Array a, std::string funcname,
			 Interpreter* eval);

Array ClassMatrixConstructor(ArrayMatrix m, Interpreter* eval);

ArrayVector ClassFunction(int nargout, const ArrayVector& arg,
			  Interpreter* eval);

ArrayVector ClassRHSExpression(Array a, treeVector t, Interpreter* eval);

void ClassAssignExpression(Array* r, tree t, ArrayVector& value, Interpreter* eval);
  
bool ClassResolveFunction(Interpreter* eval, Array &args, std::string funcName, FuncPtr& val);
  
std::string ClassMangleName(std::string className, std::string funcName);

void LoadClassFunction(Context* context);

#endif
