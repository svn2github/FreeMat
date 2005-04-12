// Copyright (c) 2002, 2003 Samit Basu
// 
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.

#ifndef __Core_hpp__
#define __Core_hpp__

#include "Array.hpp"
#include "WalkTree.hpp"

namespace FreeMat {
  ArrayVector ComplexFunction(int, const ArrayVector& arg);
  ArrayVector CosFunction(int, const ArrayVector& arg);
  ArrayVector ArccosFunction(int, const ArrayVector& arg);
  ArrayVector DcomplexFunction(int, const ArrayVector& arg);
  ArrayVector DoubleFunction(int, const ArrayVector& arg);
  ArrayVector EFunction(int, const ArrayVector& arg);
  ArrayVector ExpFunction(int, const ArrayVector& arg);
  ArrayVector FFTFunction(int, const ArrayVector& arg);
  ArrayVector FloatFunction(int, const ArrayVector& arg);
  ArrayVector IFFTFunction(int, const ArrayVector& arg);
  ArrayVector IFunction(int, const ArrayVector& arg);
  ArrayVector InfFunction(int, const ArrayVector& arg);
  ArrayVector Int16Function(int, const ArrayVector& arg);
  ArrayVector Int32Function(int, const ArrayVector& arg);
  ArrayVector Int8Function(int, const ArrayVector& arg);
  ArrayVector LogicalFunction(int, const ArrayVector& arg);
  ArrayVector LogFunction(int, const ArrayVector& arg);
  ArrayVector NaNFunction(int, const ArrayVector& arg);
  ArrayVector OnesFunction(int, const ArrayVector& arg);
  ArrayVector PiFunction(int, const ArrayVector& arg);
  ArrayVector SinFunction(int, const ArrayVector& arg);
  ArrayVector ArcsinFunction(int, const ArrayVector& arg);
  ArrayVector StrCmpFunction(int, const ArrayVector& arg);
  ArrayVector UInt16Function(int, const ArrayVector& arg);
  ArrayVector UInt32Function(int, const ArrayVector& arg);
  ArrayVector UInt8Function(int, const ArrayVector& arg);
  ArrayVector ZerosFunction(int, const ArrayVector& arg);
  ArrayVector DispFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector LengthFunction(int, const ArrayVector& arg);
  ArrayVector QRDFunction(int, const ArrayVector& arg);
  ArrayVector EigFunction(int, const ArrayVector& arg);
  ArrayVector EigsFunction(int, const ArrayVector& arg);
  ArrayVector SVDFunction(int, const ArrayVector& arg);
  ArrayVector DiagFunction(int, const ArrayVector& arg);
  ArrayVector IsEmptyFunction(int, const ArrayVector& arg);
  ArrayVector ErrorFunction(int, const ArrayVector& arg);
  ArrayVector LasterrFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector SaveFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector LoadFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector ChangeDirFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector ListFilesFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector SystemFunction(int, const ArrayVector& arg);
  ArrayVector PrintWorkingDirectoryFunction(int, const ArrayVector& arg);
  ArrayVector SizeFunction(int, const ArrayVector& arg);
  ArrayVector TestFunction(int, const ArrayVector& arg);
  ArrayVector ErrorFunction(int, const ArrayVector& arg);
  ArrayVector TypeOfFunction(int, const ArrayVector& arg);
  ArrayVector StructFunction(int, const ArrayVector& arg);
  ArrayVector RInitFunction(int, const ArrayVector& arg);
  ArrayVector RStartFunction(int, const ArrayVector& arg);
  ArrayVector RCallFunction(int, const ArrayVector& arg);
  ArrayVector RRetFunction(int, const ArrayVector& arg);
  ArrayVector FopenFunction(int, const ArrayVector& arg);
  ArrayVector FreadFunction(int, const ArrayVector& arg);
  ArrayVector FwriteFunction(int, const ArrayVector& arg);
  ArrayVector FcloseFunction(int, const ArrayVector& arg);
  ArrayVector FseekFunction(int, const ArrayVector& arg);
  ArrayVector FtellFunction(int, const ArrayVector& arg);
  ArrayVector FeofFunction(int, const ArrayVector& arg);
  ArrayVector FgetlineFunction(int, const ArrayVector& arg);
  ArrayVector MinFunction(int, const ArrayVector& arg);
  ArrayVector MaxFunction(int, const ArrayVector& arg);
  ArrayVector SumFunction(int, const ArrayVector& arg);
  ArrayVector CumsumFunction(int, const ArrayVector& arg);
  ArrayVector MeanFunction(int, const ArrayVector& arg);
  ArrayVector VarFunction(int, const ArrayVector& arg);
  ArrayVector ProdFunction(int, const ArrayVector& arg);
  ArrayVector ReshapeFunction(int, const ArrayVector& arg);
  ArrayVector PrintfFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector SprintfFunction(int, const ArrayVector& arg);
  ArrayVector FprintfFunction(int, const ArrayVector& arg);
  ArrayVector FscanfFunction(int, const ArrayVector& arg);
  ArrayVector Str2NumFunction(int, const ArrayVector& arg);
  ArrayVector CotFunction(int, const ArrayVector& arg);
  ArrayVector TanFunction(int, const ArrayVector& arg);
  ArrayVector ArctanFunction(int, const ArrayVector& arg);
  ArrayVector Arctan2Function(int, const ArrayVector& arg);
  ArrayVector CscFunction(int, const ArrayVector& arg);  
  ArrayVector SecFunction(int, const ArrayVector& arg);  
  ArrayVector WhoFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector ExistFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector IsSetFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector ClearFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector FevalFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector BuiltinFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector FdumpFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector EvalFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector EvalInFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector SourceFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector ConnectFunction(int, const ArrayVector& arg);  
  ArrayVector AcceptFunction(int, const ArrayVector& arg);  
  ArrayVector ServerFunction(int, const ArrayVector& arg);  
  ArrayVector SendFunction(int, const ArrayVector& arg);
  ArrayVector ReceiveFunction(int, const ArrayVector& arg);
  ArrayVector RandFunction(int, const ArrayVector& arg);
  ArrayVector RandnFunction(int, const ArrayVector& arg);
  ArrayVector SeedFunction(int, const ArrayVector& arg);
  ArrayVector RandBetaFunction(int, const ArrayVector& arg);
  ArrayVector RandBinFunction(int, const ArrayVector& arg);
  ArrayVector RandNBinFunction(int, const ArrayVector& arg);
  ArrayVector RandPoissonFunction(int, const ArrayVector& arg);
  ArrayVector RandIFunction(int, const ArrayVector& arg);
  ArrayVector RandChiFunction(int, const ArrayVector& arg);
  ArrayVector RandNChiFunction(int, const ArrayVector& arg);
  ArrayVector RandExpFunction(int, const ArrayVector& arg);
  ArrayVector RandFFunction(int, const ArrayVector& arg);
  ArrayVector RandNFFunction(int, const ArrayVector& arg);
  ArrayVector RandGammaFunction(int, const ArrayVector& arg);
  ArrayVector RandMultiFunction(int, const ArrayVector& arg);
  ArrayVector ConjFunction(int, const ArrayVector& arg);
  ArrayVector RealFunction(int, const ArrayVector& arg);
  ArrayVector ImagFunction(int, const ArrayVector& arg);
  ArrayVector CeilFunction(int, const ArrayVector& arg);
  ArrayVector FloorFunction(int, const ArrayVector& arg);
  ArrayVector AbsFunction(int, const ArrayVector& arg);
  ArrayVector FindFunction(int, const ArrayVector& arg);
  ArrayVector HelpFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector SleepFunction(int, const ArrayVector& arg);
  ArrayVector SetPrintLimitFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector GetPrintLimitFunction(int, const ArrayVector& arg, WalkTree* eval);
  void InitializeFileSubsystem();
  ArrayVector GetPathFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector SetPathFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector WhichFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector WhereFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector IsSparseFunction(int, const ArrayVector& arg);
  ArrayVector SponesFunction(int, const ArrayVector& arg);
  ArrayVector NNZFunction(int, const ArrayVector& arg);
  ArrayVector IsNaNFunction(int, const ArrayVector& arg);
  ArrayVector IsInfFunction(int, const ArrayVector& arg);
  ArrayVector RepMatFunction(int, const ArrayVector& arg);
  ArrayVector EpsFunction(int, const ArrayVector& arg);
  ArrayVector FepsFunction(int, const ArrayVector& arg);
  ArrayVector Int2BinFunction(int, const ArrayVector& arg);
  ArrayVector Bin2IntFunction(int, const ArrayVector& arg);
  ArrayVector PCodeFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector GetLineFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector DbAutoFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector DbStopFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector DbDeleteFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector DbListFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector DbStepFunction(int, const ArrayVector& arg, WalkTree* eval);
  ArrayVector SparseFunction(int, const ArrayVector& arg);
  ArrayVector FullFunction(int, const ArrayVector& arg);
  ArrayVector LUFunction(int, const ArrayVector& arg);
  ArrayVector SortFunction(int nargout, const ArrayVector& arg);
  ArrayVector UniqueFunction(int nargout, const ArrayVector& arg);
  ArrayVector TicFunction(int nargout, const ArrayVector& arg);
  ArrayVector TocFunction(int nargout, const ArrayVector& arg);
  ArrayVector ComputerFunction(int nargout, const ArrayVector& arg);
}

#endif
