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

#ifndef __Core_hpp__
#define __Core_hpp__

#include "Array.hpp"
#include "Interpreter.hpp"

ArrayVector ComplexFunction(int, const ArrayVector& arg);
ArrayVector CosFunction(int, const ArrayVector& arg);
ArrayVector ArccosFunction(int, const ArrayVector& arg);
ArrayVector DcomplexFunction(int, const ArrayVector& arg);
ArrayVector DoubleFunction(int, const ArrayVector& arg);
ArrayVector EFunction(int, const ArrayVector& arg);
ArrayVector ExpFunction(int, const ArrayVector& arg);
ArrayVector ExpM1Function(int, const ArrayVector& arg);
ArrayVector FFTFunction(int, const ArrayVector& arg);
ArrayVector FloatFunction(int, const ArrayVector& arg);
ArrayVector IFFTFunction(int, const ArrayVector& arg);
ArrayVector IFunction(int, const ArrayVector& arg);
ArrayVector InfFunction(int, const ArrayVector& arg);
ArrayVector Int16Function(int, const ArrayVector& arg);
ArrayVector Int32Function(int, const ArrayVector& arg);
ArrayVector Int64Function(int, const ArrayVector& arg);
ArrayVector Int8Function(int, const ArrayVector& arg);
ArrayVector LogicalFunction(int, const ArrayVector& arg);
ArrayVector LogFunction(int, const ArrayVector& arg);
ArrayVector Log1PFunction(int, const ArrayVector& arg);
ArrayVector NaNFunction(int, const ArrayVector& arg);
ArrayVector OnesFunction(int, const ArrayVector& arg);
ArrayVector PiFunction(int, const ArrayVector& arg);
ArrayVector SinFunction(int, const ArrayVector& arg);
ArrayVector ArcsinFunction(int, const ArrayVector& arg);
ArrayVector StrCmpFunction(int, const ArrayVector& arg);
ArrayVector UInt16Function(int, const ArrayVector& arg);
ArrayVector UInt32Function(int, const ArrayVector& arg);
ArrayVector UInt64Function(int, const ArrayVector& arg);
ArrayVector UInt8Function(int, const ArrayVector& arg);
ArrayVector StringFunction(int, const ArrayVector& arg);
ArrayVector ZerosFunction(int, const ArrayVector& arg);
ArrayVector DispFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector LengthFunction(int, const ArrayVector& arg);
ArrayVector QRDFunction(int, const ArrayVector& arg);
ArrayVector EigFunction(int, const ArrayVector& arg);
ArrayVector EigsFunction(int, const ArrayVector& arg);
ArrayVector SVDFunction(int, const ArrayVector& arg);
ArrayVector DiagFunction(int, const ArrayVector& arg);
ArrayVector IsEmptyFunction(int, const ArrayVector& arg);
ArrayVector ErrorFunction(int, const ArrayVector& arg);
ArrayVector WarningFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector LasterrFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector SaveFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector LoadFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector ChangeDirFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector ListFilesFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector DoCLIFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector RMDirFunction(int, const ArrayVector& arg);
ArrayVector MKDirCoreFunction(int, const ArrayVector& arg);
ArrayVector FilePartsFunction(int, const ArrayVector& arg);
ArrayVector SystemFunction(int, const ArrayVector& arg);
ArrayVector PrintWorkingDirectoryFunction(int, const ArrayVector& arg);
ArrayVector DirSepFunction(int, const ArrayVector& arg);
ArrayVector FieldNamesFunction(int, const ArrayVector& arg);
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
ArrayVector CumprodFunction(int, const ArrayVector& arg);
ArrayVector MeanFunction(int, const ArrayVector& arg);
ArrayVector VarFunction(int, const ArrayVector& arg);
ArrayVector ProdFunction(int, const ArrayVector& arg);
ArrayVector ReshapeFunction(int, const ArrayVector& arg);
ArrayVector PrintfFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector VerStringFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector SprintfFunction(int, const ArrayVector& arg);
ArrayVector FprintfFunction(int, const ArrayVector& arg);
ArrayVector FscanfFunction(int, const ArrayVector& arg);
ArrayVector SscanfFunction(int, const ArrayVector& arg);
ArrayVector Str2NumFunction(int, const ArrayVector& arg);
ArrayVector CotFunction(int, const ArrayVector& arg);
ArrayVector TanFunction(int, const ArrayVector& arg);
ArrayVector ArctanFunction(int, const ArrayVector& arg);
ArrayVector Arctan2Function(int, const ArrayVector& arg);
ArrayVector CscFunction(int, const ArrayVector& arg);  
ArrayVector SecFunction(int, const ArrayVector& arg);  
ArrayVector WhoFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector ExistFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector IsSetFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector ClearFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector FevalFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector BuiltinFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector FdumpFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector EvalFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector EvalInFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector AssignInFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector SourceFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector PauseFunction(int, const ArrayVector& arg, Interpreter* eval);
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
ArrayVector RoundFunction(int, const ArrayVector& arg);
ArrayVector AbsFunction(int, const ArrayVector& arg);
ArrayVector FindFunction(int, const ArrayVector& arg);
ArrayVector HelpFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector HelpWinFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector EditorFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector PathToolFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector SleepFunction(int, const ArrayVector& arg);
ArrayVector SetPrintLimitFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector GetPrintLimitFunction(int, const ArrayVector& arg, Interpreter* eval);
void InitializeFileSubsystem();
ArrayVector GetPathFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector SetPathFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector WhichFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector WhereFunction(int, const ArrayVector& arg, Interpreter* eval);
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
ArrayVector PCodeFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector GetLineFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector DbAutoFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector DbStopFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector DbDeleteFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector DbListFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector DbStepFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector SparseFunction(int, const ArrayVector& arg, Interpreter* eval);
ArrayVector FullFunction(int, const ArrayVector& arg);
ArrayVector LUFunction(int, const ArrayVector& arg);
ArrayVector SortFunction(int nargout, const ArrayVector& arg);
ArrayVector UniqueFunction(int nargout, const ArrayVector& arg);
ArrayVector TicFunction(int nargout, const ArrayVector& arg);
ArrayVector TocFunction(int nargout, const ArrayVector& arg);
ArrayVector XNrm2Function(int nargout, const ArrayVector& arg);
ArrayVector ClockFunction(int nargout, const ArrayVector& arg);
ArrayVector ClockToTimeFunction(int nargout, const ArrayVector& arg);
ArrayVector MFilenameFunction(int nargout, const ArrayVector& arg, Interpreter* eval);
ArrayVector ComputerFunction(int nargout, const ArrayVector& arg);
ArrayVector StrStrFunction(int nargout, const ArrayVector& arg);
ArrayVector CellFunction(int nargout, const ArrayVector& arg);
ArrayVector StrRepStringFunction(int nargout, const ArrayVector& arg);
ArrayVector PermuteFunction(int nargout, const ArrayVector& arg);
ArrayVector Conv2Function(int nargout, const ArrayVector& arg);
ArrayVector InvFunction(int nargout, const ArrayVector& arg);
ArrayVector RcondFunction(int nargout, const ArrayVector& arg);
ArrayVector EndFunction(int nargout, const  ArrayVector& arg);
ArrayVector XMLReadFunction(int nargout, const ArrayVector& arg);
ArrayVector HTMLReadFunction(int nargout, const ArrayVector& arg);
ArrayVector URLWriteFunction(int nargout, const ArrayVector& arg);
ArrayVector RPCInitFunction(int nargout, const ArrayVector& arg);
ArrayVector RPCIdFunction(int nargout, const ArrayVector& arg);
ArrayVector RPCRegFunction(int nargout, const ArrayVector& arg);
ArrayVector RPCPutFunction(int nargout, const ArrayVector& arg);
ArrayVector RPCGetFunction(int nargout, const ArrayVector& arg);
ArrayVector TrueFunction(int nargout, const ArrayVector& arg);
ArrayVector FalseFunction(int nargout, const ArrayVector& arg);
ArrayVector RegExpRepDriverFunction(int nargout, const ArrayVector& arg);
ArrayVector RegExpFunction(int nargout, const ArrayVector& arg);
ArrayVector RegExpIFunction(int nargout, const ArrayVector& arg);
void Tokenize(const std::string& str, std::vector<std::string>& tokens,
	      const std::string& delimiters = " \n");
bool inBundleMode();
ArrayVector MatLoadFunction(int nargout, const ArrayVector& arg, Interpreter* eval);
ArrayVector MatSaveFunction(int nargout, const ArrayVector& arg, Interpreter* eval);
void SwapBuffer(char* cp, int count, int elsize);
#endif
