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
#include <QDebug>

void * Data::copyDataBlock(void *dp) {
  qDebug() << "Copy!\n";
  if (dataClass == FM_FUNCPTR_ARRAY) {
    FunctionDef **cp = new FunctionDef*[dimensions.getElementCount()];
    for (int i=0;i<dimensions.getElementCount();i++)
      cp[i] = ((FunctionDef**) dp)[i];
    return (void*)cp;
  } else if (dataClass == FM_CELL_ARRAY) {
    Array *cp = new Array[dimensions.getElementCount()];
    for (int i=0;i<dimensions.getElementCount();i++)
      cp[i] = ((Array*)dp)[i];
    return (void*)cp;
  } else if (dataClass == FM_STRUCT_ARRAY) {
    Array *cp = new Array[dimensions.getElementCount()];
    for (int i=0;i<dimensions.getElementCount()*fieldNames.size();i++)
      cp[i] = ((Array*)dp)[i];
    return (void*)cp;
  } else {
    void *cp = Malloc(dimensions.getElementCount()*ByteSize(dataClass));
    memcpy(cp,dp,dimensions.getElementCount()*ByteSize(dataClass));
    return cp;
  }
}

void Data::FreeData() {
  if (cp) {
    if (dataClass == FM_FUNCPTR_ARRAY)
      delete[] ((FunctionDef**) cp);
    else if ((dataClass == FM_CELL_ARRAY) ||
	     (dataClass == FM_STRUCT_ARRAY))
      delete[] ((Array*) cp);
    else if (sparse)
      DeleteSparseMatrix(dataClass,dimensions.get(1),cp);
    else
      Free(cp);
  }
}
  

