//Contains routines to assist in manipulations of numeric arrays - used to 
//be part of the Array class, but have split them out here due to unwieldiness

#include "NumericArray.hpp"
#include "Array.hpp"
#include "FunctionDef.hpp"

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
  void getNDimSubsetNumericNoColonReal(const T *sp, T* destp, 
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
      memcpy(destp+burstLen*i,sp+burstLen*srcadd,numrows*sizeof(T)*burstLen);
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
      for (int k=0;k<burstLen;k++) 
	*destp++ = sp[burstLen*srcadd+k];
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
					  indx, L,colonIndex);
    else
      getNDimSubsetNumericSliceReal<T>(srcptr,
				       destptr,outDimsInt,srcDimsInt,
				       indx,L,colonIndex);
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
//  template void getNDimSubsetNumericDispatchReal<logical>(int colonIndex, 
//							const logical* srcptr, 
//							logical* destptr, 
//							int outDimsInt[maxDims], 
//							int srcDimsInt[maxDims], 
//							constIndexPtr* indx, int L);
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
//   template void getNDimSubsetNumericDispatchReal<int8>(int colonIndex, 
// 							const int8* srcptr, 
// 							int8* destptr, 
// 							int outDimsInt[maxDims], 
// 							int srcDimsInt[maxDims], 
// 							constIndexPtr* indx, int L);
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
}
