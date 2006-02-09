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

#include "Types.hpp"
#include "Dimensions.hpp"

namespace FreeMat {
  template <class T>
  void getNDimSubsetNumericDispatchBurst(int colonIndex, 
					 const T* srcptr, 
					 T* destptr, 
					 int outDimsInt[maxDims], 
					 int srcDimsInt[maxDims], 
					 constIndexPtr* indx, 
					 int L, int burstLen);
  
  template <class T>
  void getNDimSubsetNumericDispatchReal(int colonIndex, 
					const T* srcptr, 
					T* destptr, 
					int outDimsInt[maxDims], 
					int srcDimsInt[maxDims], 
					constIndexPtr* indx, int L);

  template <class T>
  void setNDimSubsetNumericDispatchBurst(int colonIndex, 
					 T* srcptr, 
					 const T* destptr, 
					 int outDimsInt[maxDims], 
					 int srcDimsInt[maxDims], 
					 constIndexPtr* indx, 
					 int L, int burstLen, 
					 int advance);
  
  template <class T>
  void setNDimSubsetNumericDispatchReal(int colonIndex, 
					T* srcptr, 
					const T* destptr, 
					int outDimsInt[maxDims], 
					int srcDimsInt[maxDims], 
					constIndexPtr* indx, int L,
					int advance);
}
