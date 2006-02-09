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

namespace FreeMat {
  typedef unsigned char  logical;
  typedef char           int8;
  typedef unsigned char  uint8;
  typedef short          int16;
  typedef unsigned short uint16;
  typedef int            int32;
  typedef unsigned int   uint32;
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
    FM_FLOAT,
    FM_DOUBLE,
    FM_COMPLEX,
    FM_DCOMPLEX,
    FM_STRING,
  } Class;
  typedef std::vector<std::string> stringVector;
}
#endif
