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

#include "Core.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include "Malloc.hpp"
#include <math.h>

//FIXME
#ifndef WIN32
//#include "config.h"
#ifdef USE_MPI
#include "MPIWrap.hpp"
#endif
#endif

namespace FreeMat {
  void LoadGUICoreFunctions(Context* context) {
     context->addSpecialFunction("helpwin",HelpWinFunction,0,0);
     context->addSpecialFunction("editor",EditorFunction,0,0);
     context->addSpecialFunction("pathtool",PathToolFunction,0,0);
  }

  void LoadCoreFunctions(Context* context) {
    context->addFunction("cos",CosFunction,1,1,"x");
    context->addFunction("acos",ArccosFunction,1,1,"x");
    context->addFunction("csc",CscFunction,1,1,"x");
    context->addFunction("sec",SecFunction,1,1,"x");
    context->addFunction("sin",SinFunction,1,1,"x");
    context->addFunction("asin",ArcsinFunction,1,1,"x");
    context->addFunction("tan",TanFunction,1,1,"x");
    context->addFunction("atan",ArctanFunction,1,1,"x");
    context->addFunction("atan2",Arctan2Function,2,1,"y","x");
    context->addFunction("cot",CotFunction,1,1,"x");
    context->addFunction("exp",ExpFunction,1,1,"x");
    context->addFunction("log",LogFunction,1,1,"x");
    context->addFunction("min",MinFunction,3,2,"x","z","n");
    context->addFunction("max",MaxFunction,3,2,"x","z","n");
    context->addFunction("sum",SumFunction,2,1,"x","d");
    context->addFunction("cumsum",CumsumFunction,2,1,"x","d");
    context->addFunction("mean",MeanFunction,2,1,"x","d");
    context->addFunction("var",VarFunction,2,1,"x","d");
    context->addFunction("prod",ProdFunction,2,1,"x","d");
    context->addFunction("ceil",CeilFunction,1,1,"x");
    context->addFunction("floor",FloorFunction,1,1,"x");
    context->addFunction("round",RoundFunction,1,1,"x");
    context->addFunction("fft",FFTFunction,3,1,"x","n","d");
    context->addFunction("ifft",IFFTFunction,3,1,"x","n","d");
    context->addSpecialFunction("disp",DispFunction,-1,0,"x");
    context->addSpecialFunction("dbstop",DbStopFunction,2,0,"file","line");
    context->addSpecialFunction("dbstep",DbStepFunction,1,0,"line");
    context->addSpecialFunction("dblist",DbListFunction,0,0,"dblist");
    context->addSpecialFunction("dbdelete",DbDeleteFunction,1,0,"number");
    context->addFunction("i",IFunction,0,1);
    context->addFunction("j",IFunction,0,1);
    context->addFunction("pi",PiFunction,0,1);
    context->addFunction("e",EFunction,0,1);
    context->addFunction("float",FloatFunction,1,1,"x");
    context->addFunction("single",FloatFunction,1,1,"x");
    context->addFunction("double",DoubleFunction,1,1,"x");
    context->addFunction("complex",ComplexFunction,1,1,"x");
    context->addFunction("dcomplex",DcomplexFunction,1,1,"x");
    context->addFunction("uint8",UInt8Function,1,1,"x");
    context->addFunction("string",StringFunction,1,1,"x");
    context->addFunction("uint16",UInt16Function,1,1,"x");
    context->addFunction("uint32",UInt32Function,1,1,"x");
    context->addFunction("int8",Int8Function,1,1,"x");
    context->addFunction("int16",Int16Function,1,1,"x");
    context->addFunction("int32",Int32Function,1,1,"x");  
    context->addFunction("logical",LogicalFunction,1,1,"x");  
    context->addFunction("svd",SVDFunction,2,3,"x","flag");
    context->addFunction("diag",DiagFunction,2,1,"x","n");
    context->addFunction("qr",QRDFunction,2,-1,"x","n");
    context->addFunction("int2bin",Int2BinFunction,2,1,"x","n");
    context->addFunction("bin2int",Bin2IntFunction,1,1,"x","n");
    context->addFunction("eig",EigFunction,2,2,"A","balanceflag");
    context->addFunction("eigs",EigsFunction,-1,-1,"A");
    context->addFunction("isempty",IsEmptyFunction,1,1,"x");
    context->addFunction("zeros",ZerosFunction,-1,1);
    context->addFunction("cell",CellFunction,-1,1);
    context->addFunction("reshape",ReshapeFunction,-1,1);
    context->addFunction("ones",OnesFunction,-1,1);
    context->addFunction("pwd",PrintWorkingDirectoryFunction,0,1);
    context->addFunction("error",ErrorFunction,1,0,"x");
    context->addSpecialFunction("warning",WarningFunction,1,0,"x");
    context->addSpecialFunction("lasterr",LasterrFunction,1,1,"x");
    context->addFunction("typeof",TypeOfFunction,1,1,"x");
    context->addFunction("strcomp",StrCmpFunction,2,1,"x","y");
    context->addFunction("struct",StructFunction,-1,1);
    context->addFunction("size",SizeFunction,-1,-1);
    context->addFunction("nan",NaNFunction,0,1);
    context->addFunction("NaN",NaNFunction,0,1);
    context->addFunction("inf",InfFunction,0,1);
    context->addFunction("Inf",InfFunction,0,1);
    context->addFunction("fieldnames",FieldNamesFunction,1,1,"x");
    context->addFunction("IsNaN",IsNaNFunction,1,1,"x");
    context->addFunction("isnan",IsNaNFunction,1,1,"x");
    context->addFunction("issparse",IsSparseFunction,1,1,"x");
    context->addFunction("nnz",NNZFunction,1,1,"x");
    context->addFunction("spones",SponesFunction,1,1,"x");
    context->addFunction("IsInf",IsInfFunction,1,1,"x");
    context->addFunction("isinf",IsInfFunction,1,1,"x");
    context->addSpecialFunction("cd",ChangeDirFunction,1,0,"dirname");
    context->addSpecialFunction("setpath",SetPathFunction,1,0,"pathlist");
    context->addSpecialFunction("getpath",GetPathFunction,0,1);
    context->addSpecialFunction("ls",ListFilesFunction,-1,0);
    context->addSpecialFunction("dir",ListFilesFunction,-1,0);
    context->addFunction("system",SystemFunction,1,1,"command");
    context->addSpecialFunction("feval",FevalFunction,-1,-1);
    context->addSpecialFunction("builtin",BuiltinFunction,-1,-1);
    context->addSpecialFunction("fdump",FdumpFunction,1,0,"function");
    context->addSpecialFunction("eval",EvalFunction,-1,-1);
    context->addSpecialFunction("evalin",EvalInFunction,-1,-1);
    context->addSpecialFunction("source",SourceFunction,1,0,"filename");
    context->addSpecialFunction("who",WhoFunction,-1,0);
    context->addSpecialFunction("exist",ExistFunction,2,1,"name","type");
    context->addSpecialFunction("isset",IsSetFunction,1,1,"name");
    context->addSpecialFunction("which",WhichFunction,1,0,"fname");
    context->addSpecialFunction("where",WhereFunction,0,0);
    context->addFunction("fopen",FopenFunction,3,1,"fname","mode","byteorder");
    context->addFunction("fclose",FcloseFunction,1,1,"handle");
    context->addFunction("fwrite",FwriteFunction,2,1,"handle","A");
    context->addFunction("fread",FreadFunction,3,1,"handle","size","precision");
    context->addFunction("fseek",FseekFunction,3,0,"handle","offset","style");
    context->addFunction("ftell",FtellFunction,1,1,"handle");
    context->addFunction("fgetline",FgetlineFunction,1,1,"handle");
    context->addFunction("feof",FeofFunction,1,1,"handle");
    context->addSpecialFunction("printf",PrintfFunction,-1,0);
    context->addFunction("sprintf",SprintfFunction,-1,1);
    context->addFunction("fprintf",FprintfFunction,-1,0);
    context->addFunction("fscanf",FscanfFunction,2,-1,"handle","format");
    context->addFunction("str2num",Str2NumFunction,1,1,"string");
    context->addFunction("rand",RandFunction,-1,1);
    context->addFunction("randn",RandnFunction,-1,1);
    context->addFunction("seed",SeedFunction,2,0,"s1","s2");
    context->addFunction("randbeta",RandBetaFunction,2,1,"p1","p2");
    context->addFunction("randf",RandFFunction,2,1,"p1","p2");
    context->addFunction("randi",RandIFunction,2,1,"p1","p2");
    context->addFunction("randgamma",RandGammaFunction,2,1,"p1","p2");
    context->addFunction("randmulti",RandMultiFunction,2,1,"p1","p2");
    context->addFunction("randnchi",RandGammaFunction,2,1,"p1","p2");
    context->addFunction("randbin",RandBinFunction,2,1,"p1","p2");
    context->addFunction("randnbin",RandNBinFunction,2,1,"p1","p2");
    context->addFunction("randnf",RandNFFunction,3,1,"p1","p2","p3");
    context->addFunction("randchi",RandChiFunction,1,1,"dof");
    context->addFunction("randexp",RandExpFunction,1,1,"means");
    context->addFunction("randp",RandPoissonFunction,1,1,"means");
    context->addFunction("find",FindFunction,1,-1,"x");
    context->addFunction("conj",ConjFunction,1,1,"x");
    context->addFunction("real",RealFunction,1,1,"x");
    context->addFunction("imag",ImagFunction,1,1,"x");
    context->addFunction("abs",AbsFunction,1,1,"x");
    context->addSpecialFunction("save",SaveFunction,-1,0);
    context->addSpecialFunction("load",LoadFunction,1,0,"filename");
    context->addSpecialFunction("clear",ClearFunction,-1,0);
    context->addSpecialFunction("setprintlimit",SetPrintLimitFunction,1,0,"n");
    context->addSpecialFunction("getprintlimit",GetPrintLimitFunction,0,1);
    context->addFunction("sleep",SleepFunction,1,0,"n");
    context->addFunction("eps",EpsFunction,0,1);
    context->addFunction("feps",FepsFunction,0,1);
    context->addFunction("repmat",RepMatFunction,3,1,"x","rows","cols");
    context->addFunction("tic",TicFunction,0,0);
    context->addFunction("toc",TocFunction,0,1);
    context->addFunction("xnrm2",XNrm2Function,1,1,"x");
    context->addFunction("clock",ClockFunction,0,1);
    context->addFunction("clocktotime",ClockToTimeFunction,1,1,"x");
    context->addSpecialFunction("help",HelpFunction,1,0,"function");
    context->addSpecialFunction("pcode",PCodeFunction,-1,0);
    context->addSpecialFunction("getline",GetLineFunction,1,1,"prompt");
    context->addSpecialFunction("sparse",SparseFunction,-1,1,"x");
    context->addFunction("full",FullFunction,1,1,"x");
    context->addFunction("lu",LUFunction,1,-1,"x");
    context->addFunction("sort",SortFunction,3,2,"A","dim","mode");
    context->addFunction("unique",UniqueFunction,2,3,"x","mode");
    context->addSpecialFunction("dbauto",DbAutoFunction,1,1,"x");
    context->addSpecialFunction("mfilename",MFilenameFunction,-1,1);
    context->addFunction("computer",ComputerFunction,0,1);
    context->addFunction("strstr",StrStrFunction,2,1,"x","y");
    context->addFunction("strrep_string",StrRepStringFunction,3,1,"source","pattern","replace");
    context->addFunction("conv2",Conv2Function,4,1,"A","B","C","keep");
    context->addFunction("inv",InvFunction,1,1,"A");
    context->addFunction("rcond",RcondFunction,1,1,"A");
    InitializeFileSubsystem();
#ifdef USE_MPI
    LoadMPIFunctions(context);
#endif
   }
}
