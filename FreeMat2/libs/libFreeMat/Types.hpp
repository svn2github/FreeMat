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

#ifndef __Types_hpp__
#define __Types_hpp__

#include <string>
#include <vector>
#include <QtGlobal>
#include "RefVec.hpp"

typedef quint8    logical;
typedef qint8     int8;
typedef quint8    uint8;
typedef qint16    int16;
typedef quint16   uint16;
typedef qint32    int32;
typedef quint32   uint32;
typedef qint64    int64;
typedef quint64   uint64;
typedef unsigned int   indexType;
typedef const indexType * constIndexPtr;

typedef enum {
  FM_FUNCPTR_ARRAY,
  FM_CELL_ARRAY,
  FM_STRUCT_ARRAY,
  FM_LOGICAL,
  FM_UINT8,
  FM_INT8,
  FM_UINT16,
  FM_INT16,
  FM_UINT32,
  FM_INT32,
  FM_UINT64,
  FM_INT64,
  FM_FLOAT,
  FM_DOUBLE,
  FM_COMPLEX,
  FM_DCOMPLEX,
  FM_STRING,
} Class;

typedef std::vector<std::string> stringVector;
typedef RefVec<std::string> rvstring;

#endif
