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

#include "Data.hpp"
#include "Malloc.hpp"
#include "Sparse.hpp"

int DataMakeCount = 0;

Data::Data(Class aClass, const Dimensions& dims, void *s, bool sparseflag, 
	   rvstring fields, rvstring classname): 
  cp(s), owners(1), dimensions(dims), dataClass(aClass) {
  sparse = sparseflag;
  fieldNames = fields;
  sparse = sparseflag;
  className = classname;
  DataMakeCount++;
} 

Data* Data::putData(Class aClass, const Dimensions& dims, void *s, 
		    bool sparseflag, rvstring fields, 
		    rvstring classname) {
  if ((owners <= 1)) {
    freeDataBlock();
    cp = s;
    dataClass = aClass;
    dimensions = dims;
    fieldNames = fields;
    sparse = sparseflag;
    className = classname;
    owners = 1;
    return this;
  } else {
    owners--;
    return new Data(aClass,dims,s,sparseflag,fields,classname);
  }
}

void Data::freeDataBlock() {
  if (cp) {
    if (dataClass == FM_FUNCPTR_ARRAY) {
      FunctionDef **dp = (FunctionDef**) cp;
      delete[] dp;
    } else if (Array::isDataClassReferenceType(dataClass)) {
      Array* rp = (Array*) cp;
      delete [] rp;
    } else if (sparse) {
      DeleteSparseMatrix(dataClass,dimensions.get(1),cp);
    } else 
      Free(cp);
  }
}
  

