//Contains routines to assist in manipulations of numeric arrays - used to 
//be part of the Array class, but have split them out here due to unwieldiness

#include "NumericArray.hpp"

namespace FreeMat {
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
  void getNDimSubsetNumericNoColon(const T *sp, T* destp, 
				   int outDims[maxDims], 
				   int srcDims[maxDims],
				   constIndexPtr* ndx,
				   int numDims) {
    // Calculate the number of output elements
    int outCount = 1;
    for (int i=0;i<numDims;i++) outCount *= outDims[i];
    // Initialize the ndxpointer to zero
    int ndxptr[numDims];
    for (int j=0;j<numDims;j++) 
      ndxptr[j] = 0;
    int ndxval[numDims];
    int srcfact[numDims];
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
  
  //
  // This is the first colon version, i.e. A(:,...) - the
  // first colon means a rapid block copy can be used instead
  // of element-at-a-time.
  template <class T>
  void getNDimSubsetNumericFirstColon(const T *sp, T* destp, 
				      int outDims[maxDims], 
				      int srcDims[maxDims],
				      constIndexPtr* ndx,
				      int numDims) {
    // Calculate the number of output elements
    int outCount = 1;
    for (int i=0;i<numDims;i++) outCount *= outDims[i];
    // Initialize the ndxpointer to zero
    int ndxptr[numDims];
    for (int j=0;j<numDims;j++) 
      ndxptr[j] = 0;
    int ndxval[numDims];
    int srcfact[numDims];
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
      memcpy(destp+i,sp+srcadd,numrows*sizeof(T));
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
  void getNDimSubsetNumericAnyColon(const T *sp, T* destp, 
				    int outDims[maxDims], 
				    int srcDims[maxDims],
				    constIndexPtr* ndx,
				    int numDims,
				    int colonIndex) {
    // Calculate the number of output elements
    int outCount = 1;
    for (int i=0;i<numDims;i++) outCount *= outDims[i];
    // Initialize the ndxpointer to zero
    int ndxptr[numDims];
    for (int j=0;j<numDims;j++) 
      ndxptr[j] = 0;
    int ndxval[numDims];
    int srcfact[numDims];
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

  
  // An optimized case - here, we have the case
  // A(n1,n2,...nd,:,nd+1,...nm)
  // We do this using a simple stride/start calculation
  // The first element we calculate as in the regular case
  // 
  template <class T>
  void getNDimSubsetSlice(const T *sp, T* destp, 
			  int outDims[maxDims], 
			  int srcDims[maxDims],
			  constIndexPtr* ndx,
			  int numDims,
			  int colonIndex) {
    // Calculate the number of output elements
    int outCount = 1;
    for (int i=0;i<numDims;i++) outCount *= outDims[i];
    // Initialize the ndxpointer to zero
    int ndxptr[numDims];
    for (int j=0;j<numDims;j++) 
      ndxptr[j] = 0;
    int ndxval[numDims];
    int srcfact[numDims];
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

  template void getNDimSubsetNumericNoColon<float>(const float*, float*, int*, int*, constIndexPtr*, int);
  template void getNDimSubsetNumericFirstColon<float>(const float*, float*, int[maxDims], int[maxDims], constIndexPtr*, int);
  template void getNDimSubsetNumericAnyColon<float>(const float*, float*, int[maxDims], int[maxDims], constIndexPtr*, int, int);
  template void getNDimSubsetSlice<float>(const float*, float*, int[maxDims], int[maxDims], constIndexPtr*, int, int);
}
