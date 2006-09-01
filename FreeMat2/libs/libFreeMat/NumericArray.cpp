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

//Contains routines to assist in manipulations of numeric arrays - used to 
//be part of the Array class, but have split them out here due to unwieldiness

#include "NumericArray.hpp"
#include "Array.hpp"
#include "FunctionDef.hpp"
#include "Dimensions.hpp"

// We want to get an n-dimimensional subset of an array
// b = a(v1,v2,v3)
// We want to generate the sequence
//  0 0 0
//  0 0 1
//  0 0 n-1
//  0 1 0
//  0 1 1
//  0 1 2
// We can either compute this using the module algorithm
// or incrementally.  So if we have an input of size
// N x P x M, then the ith element 
//
// The following code is a reasonable compromise of tightness/readibility.
// However, a benchmark shows that MATLAB is roughly 12% faster in a few
// instances.  I'm open to suggestions on how to squeeze that 12% out of this
// code in a clean (i.e., cross-platform) way.
//
//  This is the generic version (no special cases here).
template <class T>
void getNDimSubsetNumericNoColonReal(const T *sp, T* destp, 
				     int outDims[maxDims], 
				     int srcDims[maxDims],
				     constIndexPtr* ndx,
				     int numDims) {
  // Calculate the number of output elements
  int outCount = 1;
  for (int i=0;i<numDims;i++) outCount *= outDims[i];
  // Initialize the ndxpointer to zero
  int ndxptr[maxDims];
  for (int j=0;j<numDims;j++) 
    ndxptr[j] = 0;
  int ndxval[maxDims];
  int srcfact[maxDims];
  srcfact[0] = 1;
  for (int j=1;j<numDims;j++)
    srcfact[j] = srcfact[j-1]*srcDims[j-1];
  // For each output element
  for (int i=0;i<outCount;i++) {
    int srcadd = 0;
    // Retrieve the index values based on ndxptr
    // Use these to calculate the source address
    for (int j=0;j<numDims;j++) {
      int ndxval = ndx[j][ndxptr[j]]-1;
      srcadd += ndxval*srcfact[j];
    }
    // Copy the value
    *destp++ = sp[srcadd];
    // Update the ndxset
    ndxptr[0]++;
    for (int j=0;j<numDims-1;j++) {
      if (ndxptr[j] >= outDims[j]) {
	ndxptr[j] = 0;
	ndxptr[j+1]++;
      }
    }
  }
}

template <class T>
void setNDimSubsetNumericNoColonReal(T *sp, const T* destp, 
				     int outDims[maxDims], 
				     int srcDims[maxDims],
				     constIndexPtr* ndx,
				     int numDims, int advance) {
  // Calculate the number of output elements
  int outCount = 1;
  for (int i=0;i<numDims;i++) outCount *= outDims[i];
  // Initialize the ndxpointer to zero
  int ndxptr[maxDims];
  for (int j=0;j<numDims;j++) 
    ndxptr[j] = 0;
  int ndxval[maxDims];
  int srcfact[maxDims];
  srcfact[0] = 1;
  for (int j=1;j<numDims;j++)
    srcfact[j] = srcfact[j-1]*srcDims[j-1];
  // For each output element
  for (int i=0;i<outCount;i++) {
    int srcadd = 0;
    // Retrieve the index values based on ndxptr
    // Use these to calculate the source address
    for (int j=0;j<numDims;j++) {
      int ndxval = ndx[j][ndxptr[j]]-1;
      srcadd += ndxval*srcfact[j];
    }
    // Copy the value
    sp[srcadd] = *destp;
    destp += advance;
    // Update the ndxset
    ndxptr[0]++;
    for (int j=0;j<numDims-1;j++) {
      if (ndxptr[j] >= outDims[j]) {
	ndxptr[j] = 0;
	ndxptr[j+1]++;
      }
    }
  }
}

template <class T>
void getNDimSubsetNumericNoColonBurst(const T *sp, T* destp, 
				      int outDims[maxDims], 
				      int srcDims[maxDims],
				      constIndexPtr* ndx,
				      int numDims,
				      int burstLen) {
  // Calculate the number of output elements
  int outCount = 1;
  for (int i=0;i<numDims;i++) outCount *= outDims[i];
  // Initialize the ndxpointer to zero
  int ndxptr[maxDims];
  for (int j=0;j<numDims;j++) 
    ndxptr[j] = 0;
  int ndxval[maxDims];
  int srcfact[maxDims];
  srcfact[0] = 1;
  for (int j=1;j<numDims;j++)
    srcfact[j] = srcfact[j-1]*srcDims[j-1];
  // For each output element
  for (int i=0;i<outCount;i++) {
    int srcadd = 0;
    // Retrieve the index values based on ndxptr
    // Use these to calculate the source address
    for (int j=0;j<numDims;j++) {
      int ndxval = ndx[j][ndxptr[j]]-1;
      srcadd += ndxval*srcfact[j];
    }
    // Copy the value
    for (int k=0;k<burstLen;k++)
      *destp++ = sp[burstLen*srcadd+k];
    // Update the ndxset
    ndxptr[0]++;
    for (int j=0;j<numDims-1;j++) {
      if (ndxptr[j] >= outDims[j]) {
	ndxptr[j] = 0;
	ndxptr[j+1]++;
      }
    }
  }
}
  
template <class T>
void setNDimSubsetNumericNoColonBurst(T *sp, const T* destp, 
				      int outDims[maxDims], 
				      int srcDims[maxDims],
				      constIndexPtr* ndx,
				      int numDims,
				      int burstLen,
				      int advance) {
  // Calculate the number of output elements
  int outCount = 1;
  for (int i=0;i<numDims;i++) outCount *= outDims[i];
  // Initialize the ndxpointer to zero
  int ndxptr[maxDims];
  for (int j=0;j<numDims;j++) 
    ndxptr[j] = 0;
  int ndxval[maxDims];
  int srcfact[maxDims];
  srcfact[0] = 1;
  for (int j=1;j<numDims;j++)
    srcfact[j] = srcfact[j-1]*srcDims[j-1];
  // For each output element
  for (int i=0;i<outCount;i++) {
    int srcadd = 0;
    // Retrieve the index values based on ndxptr
    // Use these to calculate the source address
    for (int j=0;j<numDims;j++) {
      int ndxval = ndx[j][ndxptr[j]]-1;
      srcadd += ndxval*srcfact[j];
    }
    // Copy the value
    for (int k=0;k<burstLen;k++) 
      sp[burstLen*srcadd+k] = destp[k];
    destp += burstLen*advance;
    // Update the ndxset
    ndxptr[0]++;
    for (int j=0;j<numDims-1;j++) {
      if (ndxptr[j] >= outDims[j]) {
	ndxptr[j] = 0;
	ndxptr[j+1]++;
      }
    }
  }
}
  
//
// This is the first colon version, i.e. A(:,...) - the
// first colon means a rapid block copy can be used instead
// of element-at-a-time.
template <class T>
void getNDimSubsetNumericFirstColonReal(const T *sp, T* destp, 
					int outDims[maxDims], 
					int srcDims[maxDims],
					constIndexPtr* ndx,
					int numDims) {
  // Calculate the number of output elements
  int outCount = 1;
  for (int i=0;i<numDims;i++) outCount *= outDims[i];
  // Initialize the ndxpointer to zero
  int ndxptr[maxDims];
  for (int j=0;j<numDims;j++) 
    ndxptr[j] = 0;
  int ndxval[maxDims];
  int srcfact[maxDims];
  srcfact[0] = 1;
  for (int j=1;j<numDims;j++)
    srcfact[j] = srcfact[j-1]*srcDims[j-1];
  int numrows = outDims[0];
  // For each output element
  for (int i=0;i<outCount;i+=numrows) {
    int srcadd = 0;
    // Retrieve the index values based on ndxptr
    // Use these to calculate the source address
    for (int j=1;j<numDims;j++) {
      int ndxval = ndx[j][ndxptr[j]]-1;
      srcadd += ndxval*srcfact[j];
    }
    // Copy the value
    for (int k=0;k<numrows;k++)
      destp[i+k] = sp[srcadd+k];
    // Update the ndxset
    ndxptr[1]++;
    for (int j=1;j<numDims-1;j++) {
      if (ndxptr[j] >= outDims[j]) {
	ndxptr[j] = 0;
	ndxptr[j+1]++;
      }
    }
  }
}

template <class T>
void setNDimSubsetNumericFirstColonReal(T *sp, const T* destp, 
					int outDims[maxDims], 
					int srcDims[maxDims],
					constIndexPtr* ndx,
					int numDims,
					int advance) {
  // Calculate the number of output elements
  int outCount = 1;
  for (int i=0;i<numDims;i++) outCount *= outDims[i];
  // Initialize the ndxpointer to zero
  int ndxptr[maxDims];
  for (int j=0;j<numDims;j++) 
    ndxptr[j] = 0;
  int ndxval[maxDims];
  int srcfact[maxDims];
  srcfact[0] = 1;
  for (int j=1;j<numDims;j++)
    srcfact[j] = srcfact[j-1]*srcDims[j-1];
  int numrows = outDims[0];
  // For each output element
  for (int i=0;i<outCount;i+=numrows) {
    int srcadd = 0;
    // Retrieve the index values based on ndxptr
    // Use these to calculate the source address
    for (int j=1;j<numDims;j++) {
      int ndxval = ndx[j][ndxptr[j]]-1;
      srcadd += ndxval*srcfact[j];
    }
    // Copy the value
    for (int k=0;k<numrows;k++) {
      sp[srcadd+k] = *destp;
      destp += advance;
    }
    // Update the ndxset
    ndxptr[1]++;
    for (int j=1;j<numDims-1;j++) {
      if (ndxptr[j] >= outDims[j]) {
	ndxptr[j] = 0;
	ndxptr[j+1]++;
      }
    }
  }
}

template <class T>
void getNDimSubsetNumericFirstColonBurst(const T *sp, T* destp, 
					 int outDims[maxDims], 
					 int srcDims[maxDims],
					 constIndexPtr* ndx,
					 int numDims,
					 int burstLen) {
  // Calculate the number of output elements
  int outCount = 1;
  for (int i=0;i<numDims;i++) outCount *= outDims[i];
  // Initialize the ndxpointer to zero
  int ndxptr[maxDims];
  for (int j=0;j<numDims;j++) 
    ndxptr[j] = 0;
  int ndxval[maxDims];
  int srcfact[maxDims];
  srcfact[0] = 1;
  for (int j=1;j<numDims;j++)
    srcfact[j] = srcfact[j-1]*srcDims[j-1];
  int numrows = outDims[0];
  // For each output element
  for (int i=0;i<outCount;i+=numrows) {
    int srcadd = 0;
    // Retrieve the index values based on ndxptr
    // Use these to calculate the source address
    for (int j=1;j<numDims;j++) {
      int ndxval = ndx[j][ndxptr[j]]-1;
      srcadd += ndxval*srcfact[j];
    }
    // Copy the value
    for (int m=0;m<numrows;m++)
      for (int k=0;k<burstLen;k++)
	destp[(i+m)*burstLen+k] = sp[burstLen*srcadd+m*burstLen+k];
    // Update the ndxset
    ndxptr[1]++;
    for (int j=1;j<numDims-1;j++) {
      if (ndxptr[j] >= outDims[j]) {
	ndxptr[j] = 0;
	ndxptr[j+1]++;
      }
    }
  }
}

template <class T>
void setNDimSubsetNumericFirstColonBurst(T *sp, const T* destp, 
					 int outDims[maxDims], 
					 int srcDims[maxDims],
					 constIndexPtr* ndx,
					 int numDims,
					 int burstLen,
					 int advance) {
  // Calculate the number of output elements
  int outCount = 1;
  for (int i=0;i<numDims;i++) outCount *= outDims[i];
  // Initialize the ndxpointer to zero
  int ndxptr[maxDims];
  for (int j=0;j<numDims;j++) 
    ndxptr[j] = 0;
  int ndxval[maxDims];
  int srcfact[maxDims];
  srcfact[0] = 1;
  for (int j=1;j<numDims;j++)
    srcfact[j] = srcfact[j-1]*srcDims[j-1];
  int numrows = outDims[0];
  // For each output element
  for (int i=0;i<outCount;i+=numrows) {
    int srcadd = 0;
    // Retrieve the index values based on ndxptr
    // Use these to calculate the source address
    for (int j=1;j<numDims;j++) {
      int ndxval = ndx[j][ndxptr[j]]-1;
      srcadd += ndxval*srcfact[j];
    }
    // Copy the value
    for (int m=0;m<numrows;m++) 
      for (int k=0;k<burstLen;k++)
	sp[burstLen*srcadd+m*burstLen+k] = destp[(i+m)*burstLen*advance+k];
    // Update the ndxset
    ndxptr[1]++;
    for (int j=1;j<numDims-1;j++) {
      if (ndxptr[j] >= outDims[j]) {
	ndxptr[j] = 0;
	ndxptr[j+1]++;
      }
    }
  }
}
  
// This is a slight optimization - if a colon op is
// used, but not in the first index, we save the generation
// of the colon array in that index - a small optimization.
template <class T>
void getNDimSubsetNumericAnyColonReal(const T *sp, T* destp, 
				      int outDims[maxDims], 
				      int srcDims[maxDims],
				      constIndexPtr* ndx,
				      int numDims,
				      int colonIndex) {
  // Calculate the number of output elements
  int outCount = 1;
  for (int i=0;i<numDims;i++) outCount *= outDims[i];
  // Initialize the ndxpointer to zero
  int ndxptr[maxDims];
  for (int j=0;j<numDims;j++) 
    ndxptr[j] = 0;
  int ndxval[maxDims];
  int srcfact[maxDims];
  srcfact[0] = 1;
  for (int j=1;j<numDims;j++)
    srcfact[j] = srcfact[j-1]*srcDims[j-1];
  // For each output element
  for (int i=0;i<outCount;i++) {
    int srcadd = 0;
    // Retrieve the index values based on ndxptr
    // Use these to calculate the source address
    for (int j=0;j<numDims;j++) {
      int ndxval;
      if (j==colonIndex)
	ndxval = ndxptr[j];
      else
	ndxval = ndx[j][ndxptr[j]]-1;
      srcadd += ndxval*srcfact[j];
    }
    // Copy the value
    *destp++ = sp[srcadd];
    // Update the ndxset
    ndxptr[0]++;
    for (int j=0;j<numDims-1;j++) {
      if (ndxptr[j] >= outDims[j]) {
	ndxptr[j] = 0;
	ndxptr[j+1]++;
      }
    }
  }
}

// This is a slight optimization - if a colon op is
// used, but not in the first index, we save the generation
// of the colon array in that index - a small optimization.
template <class T>
void setNDimSubsetNumericAnyColonReal(T *sp, const T* destp, 
				      int outDims[maxDims], 
				      int srcDims[maxDims],
				      constIndexPtr* ndx,
				      int numDims,
				      int colonIndex,
				      int advance) {
  // Calculate the number of output elements
  int outCount = 1;
  for (int i=0;i<numDims;i++) outCount *= outDims[i];
  // Initialize the ndxpointer to zero
  int ndxptr[maxDims];
  for (int j=0;j<numDims;j++) 
    ndxptr[j] = 0;
  int ndxval[maxDims];
  int srcfact[maxDims];
  srcfact[0] = 1;
  for (int j=1;j<numDims;j++)
    srcfact[j] = srcfact[j-1]*srcDims[j-1];
  // For each output element
  for (int i=0;i<outCount;i++) {
    int srcadd = 0;
    // Retrieve the index values based on ndxptr
    // Use these to calculate the source address
    for (int j=0;j<numDims;j++) {
      int ndxval;
      if (j==colonIndex)
	ndxval = ndxptr[j];
      else
	ndxval = ndx[j][ndxptr[j]]-1;
      srcadd += ndxval*srcfact[j];
    }
    // Copy the value
    sp[srcadd] = *destp;
    destp += advance;
    // Update the ndxset
    ndxptr[0]++;
    for (int j=0;j<numDims-1;j++) {
      if (ndxptr[j] >= outDims[j]) {
	ndxptr[j] = 0;
	ndxptr[j+1]++;
      }
    }
  }
}

template <class T>
void getNDimSubsetNumericAnyColonBurst(const T *sp, T* destp, 
				       int outDims[maxDims], 
				       int srcDims[maxDims],
				       constIndexPtr* ndx,
				       int numDims,
				       int colonIndex,
				       int burstLen) {
  // Calculate the number of output elements
  int outCount = 1;
  for (int i=0;i<numDims;i++) outCount *= outDims[i];
  // Initialize the ndxpointer to zero
  int ndxptr[maxDims];
  for (int j=0;j<numDims;j++) 
    ndxptr[j] = 0;
  int ndxval[maxDims];
  int srcfact[maxDims];
  srcfact[0] = 1;
  for (int j=1;j<numDims;j++)
    srcfact[j] = srcfact[j-1]*srcDims[j-1];
  // For each output element
  for (int i=0;i<outCount;i++) {
    int srcadd = 0;
    // Retrieve the index values based on ndxptr
    // Use these to calculate the source address
    for (int j=0;j<numDims;j++) {
      int ndxval;
      if (j==colonIndex)
	ndxval = ndxptr[j];
      else
	ndxval = ndx[j][ndxptr[j]]-1;
      srcadd += ndxval*srcfact[j];
    }
    // Copy the value
    for (int k=0;k<burstLen;k++) 
      *destp++ = sp[burstLen*srcadd+k];
    // Update the ndxset
    ndxptr[0]++;
    for (int j=0;j<numDims-1;j++) {
      if (ndxptr[j] >= outDims[j]) {
	ndxptr[j] = 0;
	ndxptr[j+1]++;
      }
    }
  }
}

template <class T>
void setNDimSubsetNumericAnyColonBurst(T *sp, const T* destp, 
				       int outDims[maxDims], 
				       int srcDims[maxDims],
				       constIndexPtr* ndx,
				       int numDims,
				       int colonIndex,
				       int burstLen,
				       int advance) {
  // Calculate the number of output elements
  int outCount = 1;
  for (int i=0;i<numDims;i++) outCount *= outDims[i];
  // Initialize the ndxpointer to zero
  int ndxptr[maxDims];
  for (int j=0;j<numDims;j++) 
    ndxptr[j] = 0;
  int ndxval[maxDims];
  int srcfact[maxDims];
  srcfact[0] = 1;
  for (int j=1;j<numDims;j++)
    srcfact[j] = srcfact[j-1]*srcDims[j-1];
  // For each output element
  for (int i=0;i<outCount;i++) {
    int srcadd = 0;
    // Retrieve the index values based on ndxptr
    // Use these to calculate the source address
    for (int j=0;j<numDims;j++) {
      int ndxval;
      if (j==colonIndex)
	ndxval = ndxptr[j];
      else
	ndxval = ndx[j][ndxptr[j]]-1;
      srcadd += ndxval*srcfact[j];
    }
    // Copy the value
    for (int k=0;k<burstLen;k++) 
      sp[burstLen*srcadd+k] = destp[i*advance+k];
    // Update the ndxset
    ndxptr[0]++;
    for (int j=0;j<numDims-1;j++) {
      if (ndxptr[j] >= outDims[j]) {
	ndxptr[j] = 0;
	ndxptr[j+1]++;
      }
    }
  }
}

  
  
// An optimized case - here, we have the case
// A(n1,n2,...nd,:,nd+1,...nm)
// We do this using a simple stride/start calculation
// The first element we calculate as in the regular case
// 
template <class T>
void getNDimSubsetNumericSliceReal(const T *sp, T* destp, 
				   int outDims[maxDims], 
				   int srcDims[maxDims],
				   constIndexPtr* ndx,
				   int numDims,
				   int colonIndex) {
  // Calculate the number of output elements
  int outCount = 1;
  for (int i=0;i<numDims;i++) outCount *= outDims[i];
  // Initialize the ndxpointer to zero
  int ndxptr[maxDims];
  for (int j=0;j<numDims;j++) 
    ndxptr[j] = 0;
  int ndxval[maxDims];
  int srcfact[maxDims];
  srcfact[0] = 1;
  for (int j=1;j<numDims;j++)
    srcfact[j] = srcfact[j-1]*srcDims[j-1];
  // Calculate the start element
  int start = 0;
  for (int j=0;j<numDims;j++) {
    int ndxval;
    if (j==colonIndex)
      ndxval = 0;
    else
      ndxval = ndx[j][0]-1;
    start += ndxval*srcfact[j];
  }
  // Next, calculate the stride distance
  // we do this by setting the colon component to 1
  int stride = 0;
  for (int j=0;j<numDims;j++) {
    int ndxval;
    if (j==colonIndex)
      ndxval = 1;
    else
      ndxval = ndx[j][0]-1;
    stride += ndxval*srcfact[j];
  }
  stride -= start;
  int srcadd = start;
  // The output equation is easy now
  for (int i=0;i<outCount;i++) {
    *destp++ = sp[srcadd];
    srcadd += stride;
  }
}

// An optimized case - here, we have the case
// A(n1,n2,...nd,:,nd+1,...nm)
// We do this using a simple stride/start calculation
// The first element we calculate as in the regular case
// 
template <class T>
void setNDimSubsetNumericSliceReal(T *sp, const T* destp, 
				   int outDims[maxDims], 
				   int srcDims[maxDims],
				   constIndexPtr* ndx,
				   int numDims,
				   int colonIndex,
				   int advance) {
  // Calculate the number of output elements
  int outCount = 1;
  for (int i=0;i<numDims;i++) outCount *= outDims[i];
  // Initialize the ndxpointer to zero
  int ndxptr[maxDims];
  for (int j=0;j<numDims;j++) 
    ndxptr[j] = 0;
  int ndxval[maxDims];
  int srcfact[maxDims];
  srcfact[0] = 1;
  for (int j=1;j<numDims;j++)
    srcfact[j] = srcfact[j-1]*srcDims[j-1];
  // Calculate the start element
  int start = 0;
  for (int j=0;j<numDims;j++) {
    int ndxval;
    if (j==colonIndex)
      ndxval = 0;
    else
      ndxval = ndx[j][0]-1;
    start += ndxval*srcfact[j];
  }
  // Next, calculate the stride distance
  // we do this by setting the colon component to 1
  int stride = 0;
  for (int j=0;j<numDims;j++) {
    int ndxval;
    if (j==colonIndex)
      ndxval = 1;
    else
      ndxval = ndx[j][0]-1;
    stride += ndxval*srcfact[j];
  }
  stride -= start;
  int srcadd = start;
  // The output equation is easy now
  for (int i=0;i<outCount;i++) {
    sp[srcadd] = *destp;
    destp += advance;
    srcadd += stride;
  }
}

template <class T>
void getNDimSubsetNumericSliceBurst(const T *sp, T* destp, 
				    int outDims[maxDims], 
				    int srcDims[maxDims],
				    constIndexPtr* ndx,
				    int numDims,
				    int colonIndex,
				    int burstLen) {
  // Calculate the number of output elements
  int outCount = 1;
  for (int i=0;i<numDims;i++) outCount *= outDims[i];
  // Initialize the ndxpointer to zero
  int ndxptr[maxDims];
  for (int j=0;j<numDims;j++) 
    ndxptr[j] = 0;
  int ndxval[maxDims];
  int srcfact[maxDims];
  srcfact[0] = 1;
  for (int j=1;j<numDims;j++)
    srcfact[j] = srcfact[j-1]*srcDims[j-1];
  // Calculate the start element
  int start = 0;
  for (int j=0;j<numDims;j++) {
    int ndxval;
    if (j==colonIndex)
      ndxval = 0;
    else
      ndxval = ndx[j][0]-1;
    start += ndxval*srcfact[j];
  }
  // Next, calculate the stride distance
  // we do this by setting the colon component to 1
  int stride = 0;
  for (int j=0;j<numDims;j++) {
    int ndxval;
    if (j==colonIndex)
      ndxval = 1;
    else
      ndxval = ndx[j][0]-1;
    stride += ndxval*srcfact[j];
  }
  stride -= start;
  int srcadd = start;
  // The output equation is easy now
  for (int i=0;i<outCount;i++) {
    for (int k=0;k<burstLen;k++) 
      *destp++ = sp[burstLen*srcadd+k];
    srcadd += stride;
  }
}
  
template <class T>
void setNDimSubsetNumericSliceBurst(T *sp, const T* destp, 
				    int outDims[maxDims], 
				    int srcDims[maxDims],
				    constIndexPtr* ndx,
				    int numDims,
				    int colonIndex,
				    int burstLen,
				    int advance) {
  // Calculate the number of output elements
  int outCount = 1;
  for (int i=0;i<numDims;i++) outCount *= outDims[i];
  // Initialize the ndxpointer to zero
  int ndxptr[maxDims];
  for (int j=0;j<numDims;j++) 
    ndxptr[j] = 0;
  int ndxval[maxDims];
  int srcfact[maxDims];
  srcfact[0] = 1;
  for (int j=1;j<numDims;j++)
    srcfact[j] = srcfact[j-1]*srcDims[j-1];
  // Calculate the start element
  int start = 0;
  for (int j=0;j<numDims;j++) {
    int ndxval;
    if (j==colonIndex)
      ndxval = 0;
    else
      ndxval = ndx[j][0]-1;
    start += ndxval*srcfact[j];
  }
  // Next, calculate the stride distance
  // we do this by setting the colon component to 1
  int stride = 0;
  for (int j=0;j<numDims;j++) {
    int ndxval;
    if (j==colonIndex)
      ndxval = 1;
    else
      ndxval = ndx[j][0]-1;
    stride += ndxval*srcfact[j];
  }
  stride -= start;
  int srcadd = start;
  // The output equation is easy now
  for (int i=0;i<outCount;i++) {
    for (int k=0;k<burstLen;k++) 
      sp[burstLen*srcadd+k] = destp[k];
    destp += advance*burstLen;
    srcadd += stride;
  }
}
  
template <class T>
void getNDimSubsetNumericDispatchBurst(int colonIndex, 
				       const T* srcptr, 
				       T* destptr, 
				       int outDimsInt[maxDims], 
				       int srcDimsInt[maxDims], 
				       constIndexPtr* indx, int L,
				       int burstLen) {
  int elCount = 1;
  for (int i=0;i<L;i++) elCount *= outDimsInt[i];
  if (colonIndex < 0)
    getNDimSubsetNumericNoColonBurst<T>(srcptr,
					destptr,outDimsInt,srcDimsInt,
					indx, L, burstLen);
  else if (colonIndex == 0)
    getNDimSubsetNumericFirstColonBurst<T>(srcptr,
					   destptr,outDimsInt,srcDimsInt,
					   indx, L, burstLen);
  else if (elCount > srcDimsInt[colonIndex])
    getNDimSubsetNumericAnyColonBurst<T>(srcptr,
					 destptr,outDimsInt,srcDimsInt,
					 indx, L,colonIndex, burstLen);
  else
    getNDimSubsetNumericSliceBurst<T>(srcptr,
				      destptr,outDimsInt,srcDimsInt,
				      indx,L,colonIndex, burstLen);
}
  
template <class T>
void setNDimSubsetNumericDispatchBurst(int colonIndex, 
				       T* srcptr, 
				       const T* destptr, 
				       int outDimsInt[maxDims], 
				       int srcDimsInt[maxDims], 
				       constIndexPtr* indx, int L,
				       int burstLen, int advance) {
  int elCount = 1;
  for (int i=0;i<L;i++) elCount *= outDimsInt[i];
  if (colonIndex < 0)
    setNDimSubsetNumericNoColonBurst<T>(srcptr,
					destptr,outDimsInt,srcDimsInt,
					indx, L, burstLen, advance);
  else if (colonIndex == 0)
    setNDimSubsetNumericFirstColonBurst<T>(srcptr,
					   destptr,outDimsInt,srcDimsInt,
					   indx, L, burstLen, advance);
  else if (elCount > srcDimsInt[colonIndex])
    setNDimSubsetNumericAnyColonBurst<T>(srcptr,
					 destptr,outDimsInt,srcDimsInt,
					 indx, L,colonIndex, burstLen, advance);
  else
    setNDimSubsetNumericSliceBurst<T>(srcptr,
				      destptr,outDimsInt,srcDimsInt,
				      indx,L,colonIndex, burstLen, advance);
}
  
template <class T>
void getNDimSubsetNumericDispatchReal(int colonIndex, 
				      const T* srcptr, 
				      T* destptr, 
				      int outDimsInt[maxDims], 
				      int srcDimsInt[maxDims], 
				      constIndexPtr* indx, int L) {
  int elCount = 1;
  for (int i=0;i<L;i++) elCount *= outDimsInt[i];
  if (colonIndex < 0)
    getNDimSubsetNumericNoColonReal<T>(srcptr,
				       destptr,outDimsInt,srcDimsInt,
				       indx, L);
  else if (colonIndex == 0)
    getNDimSubsetNumericFirstColonReal<T>(srcptr,
					  destptr,outDimsInt,srcDimsInt,
					  indx, L);
  else if (elCount > srcDimsInt[colonIndex])
    getNDimSubsetNumericAnyColonReal<T>(srcptr,
					destptr,outDimsInt,srcDimsInt,
					indx, L, colonIndex);
  else
    getNDimSubsetNumericSliceReal<T>(srcptr,
				     destptr,outDimsInt,srcDimsInt,
				     indx, L, colonIndex);
}

template <class T>
void setNDimSubsetNumericDispatchReal(int colonIndex, 
				      T* srcptr, 
				      const T* destptr, 
				      int outDimsInt[maxDims], 
				      int srcDimsInt[maxDims], 
				      constIndexPtr* indx, int L,
				      int advance) {
  int elCount = 1;
  for (int i=0;i<L;i++) elCount *= outDimsInt[i];
  if (colonIndex < 0)
    setNDimSubsetNumericNoColonReal<T>(srcptr,
				       destptr,outDimsInt,srcDimsInt,
				       indx, L, advance);
  else if (colonIndex == 0)
    setNDimSubsetNumericFirstColonReal<T>(srcptr,
					  destptr,outDimsInt,srcDimsInt,
					  indx, L, advance);
  else if (elCount > srcDimsInt[colonIndex])
    setNDimSubsetNumericAnyColonReal<T>(srcptr,
					destptr,outDimsInt,srcDimsInt,
					indx, L,colonIndex, advance);
  else
    setNDimSubsetNumericSliceReal<T>(srcptr,
				     destptr,outDimsInt,srcDimsInt,
				     indx,L,colonIndex, advance);
}
  
template void getNDimSubsetNumericDispatchBurst<float>(int colonIndex, 
						       const float* srcptr, 
						       float* destptr, 
						       int outDimsInt[maxDims], 
						       int srcDimsInt[maxDims], 
						       constIndexPtr* indx, int L,
						       int burstLen);
template void getNDimSubsetNumericDispatchBurst<double>(int colonIndex, 
							const double* srcptr, 
							double* destptr, 
							int outDimsInt[maxDims], 
							int srcDimsInt[maxDims], 
							constIndexPtr* indx, int L,
							int burstLen);
template void getNDimSubsetNumericDispatchBurst<Array>(int colonIndex, 
						       const Array* srcptr, 
						       Array* destptr, 
						       int outDimsInt[maxDims], 
						       int srcDimsInt[maxDims], 
						       constIndexPtr* indx, int L,
						       int burstLen);
template void getNDimSubsetNumericDispatchReal<float>(int colonIndex, 
						      const float* srcptr, 
						      float* destptr, 
						      int outDimsInt[maxDims], 
						      int srcDimsInt[maxDims], 
						      constIndexPtr* indx, int L);
//   template void getNDimSubsetNumericDispatchReal<logical>(int colonIndex, 
// 							const logical* srcptr, 
// 							logical* destptr, 
// 							int outDimsInt[maxDims], 
// 							int srcDimsInt[maxDims], 
// 							constIndexPtr* indx, int L);
template void getNDimSubsetNumericDispatchReal<double>(int colonIndex, 
						       const double* srcptr, 
						       double* destptr, 
						       int outDimsInt[maxDims], 
						       int srcDimsInt[maxDims], 
						       constIndexPtr* indx, int L);
template void getNDimSubsetNumericDispatchReal<char>(int colonIndex, 
						     const char* srcptr, 
						     char* destptr, 
						     int outDimsInt[maxDims], 
						     int srcDimsInt[maxDims], 
						     constIndexPtr* indx, int L);
template void getNDimSubsetNumericDispatchReal<int8>(int colonIndex, 
						     const int8* srcptr, 
						     int8* destptr, 
						     int outDimsInt[maxDims], 
						     int srcDimsInt[maxDims], 
						     constIndexPtr* indx, int L);
template void getNDimSubsetNumericDispatchReal<uint8>(int colonIndex, 
						      const uint8* srcptr, 
						      uint8* destptr, 
						      int outDimsInt[maxDims], 
						      int srcDimsInt[maxDims], 
						      constIndexPtr* indx, int L);
template void getNDimSubsetNumericDispatchReal<int16>(int colonIndex, 
						      const int16* srcptr, 
						      int16* destptr, 
						      int outDimsInt[maxDims], 
						      int srcDimsInt[maxDims], 
						      constIndexPtr* indx, int L);
template void getNDimSubsetNumericDispatchReal<uint16>(int colonIndex, 
						       const uint16* srcptr, 
						       uint16* destptr, 
						       int outDimsInt[maxDims], 
						       int srcDimsInt[maxDims], 
						       constIndexPtr* indx, int L);
template void getNDimSubsetNumericDispatchReal<int32>(int colonIndex, 
						      const int32* srcptr, 
						      int32* destptr, 
						      int outDimsInt[maxDims], 
						      int srcDimsInt[maxDims], 
						      constIndexPtr* indx, int L);
template void getNDimSubsetNumericDispatchReal<uint32>(int colonIndex, 
						       const uint32* srcptr, 
						       uint32* destptr, 
						       int outDimsInt[maxDims], 
						       int srcDimsInt[maxDims], 
						       constIndexPtr* indx, int L);
template void getNDimSubsetNumericDispatchReal<int64>(int colonIndex, 
						      const int64* srcptr, 
						      int64* destptr, 
						      int outDimsInt[maxDims], 
						      int srcDimsInt[maxDims], 
						      constIndexPtr* indx, int L);
template void getNDimSubsetNumericDispatchReal<uint64>(int colonIndex, 
						       const uint64* srcptr, 
						       uint64* destptr, 
						       int outDimsInt[maxDims], 
						       int srcDimsInt[maxDims], 
						       constIndexPtr* indx, int L);
template void getNDimSubsetNumericDispatchReal<Array>(int colonIndex, 
						      const Array* srcptr, 
						      Array* destptr, 
						      int outDimsInt[maxDims], 
						      int srcDimsInt[maxDims], 
						      constIndexPtr* indx, int L);
template void getNDimSubsetNumericDispatchReal<FunctionDefPtr>(int colonIndex, 
							       const FunctionDefPtr* srcptr, 
							       FunctionDefPtr* destptr, 
							       int outDimsInt[maxDims], 
							       int srcDimsInt[maxDims], 
							       constIndexPtr* indx, int L);


template void setNDimSubsetNumericDispatchBurst<float>(int colonIndex, 
						       float* srcptr, 
						       const float* destptr, 
						       int outDimsInt[maxDims], 
						       int srcDimsInt[maxDims], 
						       constIndexPtr* indx, int L,
						       int burstLen,int advance);
template void setNDimSubsetNumericDispatchBurst<double>(int colonIndex, 
							double* srcptr, 
							const double* destptr, 
							int outDimsInt[maxDims], 
							int srcDimsInt[maxDims], 
							constIndexPtr* indx, int L,
							int burstLen,int advance);
template void setNDimSubsetNumericDispatchBurst<Array>(int colonIndex, 
						       Array* srcptr, 
						       const Array* destptr, 
						       int outDimsInt[maxDims], 
						       int srcDimsInt[maxDims], 
						       constIndexPtr* indx, int L,
						       int burstLen,int advance);
template void setNDimSubsetNumericDispatchReal<float>(int colonIndex, 
						      float* srcptr, 
						      const float* destptr, 
						      int outDimsInt[maxDims], 
						      int srcDimsInt[maxDims], 
						      constIndexPtr* indx, int L,int advance);
//   template void setNDimSubsetNumericDispatchReal<logical>(int colonIndex, 
// 							  logical* srcptr, 
// 							  const logical* destptr, 
// 							  int outDimsInt[maxDims], 
// 							  int srcDimsInt[maxDims], 
// 							  constIndexPtr* indx, int L,int advance);
template void setNDimSubsetNumericDispatchReal<double>(int colonIndex, 
						       double* srcptr, 
						       const double* destptr, 
						       int outDimsInt[maxDims], 
						       int srcDimsInt[maxDims], 
						       constIndexPtr* indx, int L,int advance);
template void setNDimSubsetNumericDispatchReal<char>(int colonIndex, 
						     char* srcptr, 
						     const char* destptr, 
						     int outDimsInt[maxDims], 
						     int srcDimsInt[maxDims], 
						     constIndexPtr* indx, int L,int advance);
template void setNDimSubsetNumericDispatchReal<int8>(int colonIndex, 
						     int8* srcptr, 
						     const int8* destptr, 
						     int outDimsInt[maxDims], 
						     int srcDimsInt[maxDims], 
						     constIndexPtr* indx, int L,int advance);
template void setNDimSubsetNumericDispatchReal<uint8>(int colonIndex, 
						      uint8* srcptr, 
						      const uint8* destptr, 
						      int outDimsInt[maxDims], 
						      int srcDimsInt[maxDims], 
						      constIndexPtr* indx, int L,int advance);
template void setNDimSubsetNumericDispatchReal<int16>(int colonIndex, 
						      int16* srcptr, 
						      const int16* destptr, 
						      int outDimsInt[maxDims], 
						      int srcDimsInt[maxDims], 
						      constIndexPtr* indx, int L,int advance);
template void setNDimSubsetNumericDispatchReal<uint16>(int colonIndex, 
						       uint16* srcptr, 
						       const uint16* destptr, 
						       int outDimsInt[maxDims], 
						       int srcDimsInt[maxDims], 
						       constIndexPtr* indx, int L,int advance);
template void setNDimSubsetNumericDispatchReal<int32>(int colonIndex, 
						      int32* srcptr, 
						      const int32* destptr, 
						      int outDimsInt[maxDims], 
						      int srcDimsInt[maxDims], 
						      constIndexPtr* indx, int L,int advance);
template void setNDimSubsetNumericDispatchReal<uint32>(int colonIndex, 
						       uint32* srcptr, 
						       const uint32* destptr, 
						       int outDimsInt[maxDims], 
						       int srcDimsInt[maxDims], 
						       constIndexPtr* indx, int L,int advance);
template void setNDimSubsetNumericDispatchReal<int64>(int colonIndex, 
						      int64* srcptr, 
						      const int64* destptr, 
						      int outDimsInt[maxDims], 
						      int srcDimsInt[maxDims], 
						      constIndexPtr* indx, int L,int advance);
template void setNDimSubsetNumericDispatchReal<uint64>(int colonIndex, 
						       uint64* srcptr, 
						       const uint64* destptr, 
						       int outDimsInt[maxDims], 
						       int srcDimsInt[maxDims], 
						       constIndexPtr* indx, int L,int advance);
template void setNDimSubsetNumericDispatchReal<Array>(int colonIndex, 
						      Array* srcptr, 
						      const Array* destptr, 
						      int outDimsInt[maxDims], 
						      int srcDimsInt[maxDims], 
						      constIndexPtr* indx, int L,int advance);
template void setNDimSubsetNumericDispatchReal<FunctionDefPtr>(int colonIndex, 
							       FunctionDefPtr* srcptr, 
							       const FunctionDefPtr* destptr, 
							       int outDimsInt[maxDims], 
							       int srcDimsInt[maxDims], 
							       constIndexPtr* indx, int L,int advance);
