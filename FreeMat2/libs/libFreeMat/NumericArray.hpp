
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
