
#include "Types.hpp"
#include "Dimensions.hpp"

namespace FreeMat {
  template <class T>
  void getNDimSubsetNumericNoColon(const T *sp, T* destp, 
				   int outDims[maxDims], 
				   int srcDims[maxDims],
				   constIndexPtr* ndx,
				   int numDims);
  template <class T>
  void getNDimSubsetNumericFirstColon(const T *sp, T* destp, 
				      int outDims[maxDims], 
				      int srcDims[maxDims],
				      constIndexPtr* ndx,
				      int numDims);
  template <class T>
  void getNDimSubsetNumericAnyColon(const T *sp, T* destp, 
				    int outDims[maxDims], 
				    int srcDims[maxDims],
				    constIndexPtr* ndx,
				    int numDims,
				    int colonIndex);
  template <class T>
  void getNDimSubsetSlice(const T *sp, T* destp, 
			  int outDims[maxDims], 
			  int srcDims[maxDims],
			  constIndexPtr* ndx,
			  int numDims,
			  int colonIndex);
}
