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

void LoadGUICoreFunctions(Context* context) {
  context->addGfxSpecialFunction("helpwin",HelpWinFunction,0,0,NULL);
  context->addGfxSpecialFunction("editor",EditorFunction,0,0,NULL);
  context->addGfxSpecialFunction("pathtool",PathToolFunction,0,0,NULL);
}

void LoadCoreFunctions(Context* context) {
  context->addFunction("cos",CosFunction,1,1,"x",NULL);
  context->addFunction("acos",ArccosFunction,1,1,"x",NULL);
  context->addFunction("csc",CscFunction,1,1,"x",NULL);
  context->addFunction("sec",SecFunction,1,1,"x",NULL);
  context->addFunction("sin",SinFunction,1,1,"x",NULL);
  context->addFunction("asin",ArcsinFunction,1,1,"x",NULL);
  context->addFunction("tan",TanFunction,1,1,"x",NULL);
  context->addFunction("atan",ArctanFunction,1,1,"x",NULL);
  context->addFunction("atan2",Arctan2Function,2,1,"y","x",NULL);
  context->addFunction("cot",CotFunction,1,1,"x",NULL);
  context->addFunction("exp",ExpFunction,1,1,"x",NULL);
  context->addFunction("expm1",ExpM1Function,1,1,"x",NULL);
  context->addFunction("log",LogFunction,1,1,"x",NULL);
  context->addFunction("log1p",Log1PFunction,1,1,"x",NULL);
  context->addFunction("min",MinFunction,3,2,"x","z","n",NULL);
  context->addFunction("max",MaxFunction,3,2,"x","z","n",NULL);
  context->addFunction("sum",SumFunction,2,1,"x","d",NULL);
  context->addFunction("cumsum",CumsumFunction,2,1,"x","d",NULL);
  context->addFunction("mean",MeanFunction,2,1,"x","d",NULL);
  context->addFunction("var",VarFunction,2,1,"x","d",NULL);
  context->addFunction("prod",ProdFunction,2,1,"x","d",NULL);
  context->addFunction("ceil",CeilFunction,1,1,"x",NULL);
  context->addFunction("floor",FloorFunction,1,1,"x",NULL);
  context->addFunction("round",RoundFunction,1,1,"x",NULL);
  context->addFunction("fft",FFTFunction,3,1,"x","n","d",NULL);
  context->addFunction("ifft",IFFTFunction,3,1,"x","n","d",NULL);
  context->addSpecialFunction("disp",DispFunction,-1,0,"x",NULL);
  context->addSpecialFunction("dbstop",DbStopFunction,2,0,"file","line",NULL);
  //  context->addSpecialFunction("dbstep",DbStepFunction,1,0,"line",NULL);
  context->addSpecialFunction("dblist",DbListFunction,0,0,"dblist",NULL);
  context->addSpecialFunction("dbdelete",DbDeleteFunction,1,0,"number",NULL);
  context->addFunction("i",IFunction,0,1,NULL);
  context->addFunction("j",IFunction,0,1,NULL);
  context->addFunction("pi",PiFunction,0,1,NULL);
  context->addFunction("e",EFunction,0,1,NULL);
  context->addFunction("float",FloatFunction,1,1,"x",NULL);
  context->addFunction("single",FloatFunction,1,1,"x",NULL);
  context->addFunction("double",DoubleFunction,1,1,"x",NULL);
  context->addFunction("complex",ComplexFunction,1,1,"x",NULL);
  context->addFunction("dcomplex",DcomplexFunction,1,1,"x",NULL);
  context->addFunction("uint8",UInt8Function,1,1,"x",NULL);
  context->addFunction("string",StringFunction,1,1,"x",NULL);
  context->addFunction("uint16",UInt16Function,1,1,"x",NULL);
  context->addFunction("uint32",UInt32Function,1,1,"x",NULL);
  context->addFunction("uint64",UInt64Function,1,1,"x",NULL);
  context->addFunction("int8",Int8Function,1,1,"x",NULL);
  context->addFunction("int16",Int16Function,1,1,"x",NULL);
  context->addFunction("int32",Int32Function,1,1,"x",NULL);  
  context->addFunction("int64",Int64Function,1,1,"x",NULL);  
  context->addFunction("logical",LogicalFunction,1,1,"x",NULL);  
  context->addFunction("svd",SVDFunction,2,3,"x","flag",NULL);
  context->addFunction("diag",DiagFunction,2,1,"x","n",NULL);
  context->addFunction("qr",QRDFunction,2,-1,"x","n",NULL);
  context->addFunction("int2bin",Int2BinFunction,2,1,"x","n",NULL);
  context->addFunction("bin2int",Bin2IntFunction,2,1,"x","flags",NULL);
  context->addFunction("eig",EigFunction,2,2,"A","balanceflag",NULL);
  context->addFunction("eigs",EigsFunction,-1,-1,"A",NULL);
  context->addFunction("isempty",IsEmptyFunction,1,1,"x",NULL);
  context->addFunction("zeros",ZerosFunction,-1,1,NULL);
  context->addFunction("cell",CellFunction,-1,1,NULL);
  context->addFunction("reshape",ReshapeFunction,-1,1,NULL);
  context->addFunction("ones",OnesFunction,-1,1,NULL);
  context->addFunction("pwd",PrintWorkingDirectoryFunction,0,1,NULL);
  context->addFunction("dirsep",DirSepFunction,0,1,NULL);
  context->addFunction("error",ErrorFunction,1,0,"x",NULL);
  context->addSpecialFunction("warning",WarningFunction,1,0,"x",NULL);
  context->addSpecialFunction("lasterr",LasterrFunction,1,1,"x",NULL);
  context->addFunction("typeof",TypeOfFunction,1,1,"x",NULL);
  context->addFunction("strcomp",StrCmpFunction,2,1,"x","y",NULL);
  context->addFunction("struct",StructFunction,-1,1,NULL);
  context->addFunction("size",SizeFunction,-1,-1,NULL);
  context->addFunction("nan",NaNFunction,0,1,NULL);
  context->addFunction("NaN",NaNFunction,0,1,NULL);
  context->addFunction("inf",InfFunction,0,1,NULL);
  context->addFunction("Inf",InfFunction,0,1,NULL);
  context->addFunction("fieldnames",FieldNamesFunction,1,1,"x",NULL);
  context->addFunction("IsNaN",IsNaNFunction,1,1,"x",NULL);
  context->addFunction("isnan",IsNaNFunction,1,1,"x",NULL);
  context->addFunction("issparse",IsSparseFunction,1,1,"x",NULL);
  context->addFunction("nnz",NNZFunction,1,1,"x",NULL);
  context->addFunction("spones",SponesFunction,1,1,"x",NULL);
  context->addFunction("IsInf",IsInfFunction,1,1,"x",NULL);
  context->addFunction("isinf",IsInfFunction,1,1,"x",NULL);
  context->addSpecialFunction("cd",ChangeDirFunction,1,0,"dirname",NULL);
  context->addSpecialFunction("setpath",SetPathFunction,1,0,"pathlist",NULL);
  context->addSpecialFunction("getpath",GetPathFunction,0,1,NULL);
  context->addSpecialFunction("ls",ListFilesFunction,-1,0,NULL);
  context->addSpecialFunction("dir",ListFilesFunction,-1,0,NULL);
  context->addFunction("system",SystemFunction,1,1,"command",NULL);
  context->addSpecialFunction("feval",FevalFunction,-1,-1,NULL);
  context->addSpecialFunction("builtin",BuiltinFunction,-1,-1,NULL);
  context->addSpecialFunction("fdump",FdumpFunction,1,0,"function",NULL);
  context->addSpecialFunction("eval",EvalFunction,-1,-1,NULL);
  context->addSpecialFunction("evalin",EvalInFunction,-1,-1,NULL);
  context->addSpecialFunction("assignin",AssignInFunction,3,0,"workspace","name","value",NULL);
  context->addSpecialFunction("source",SourceFunction,1,0,"filename",NULL);
  context->addSpecialFunction("who",WhoFunction,-1,0,NULL);
  context->addSpecialFunction("exist",ExistFunction,2,1,"name","type",NULL);
  context->addSpecialFunction("isset",IsSetFunction,1,1,"name",NULL);
  context->addSpecialFunction("which",WhichFunction,1,1,"fname",NULL);
  context->addSpecialFunction("where",WhereFunction,0,0,NULL);
  context->addFunction("fopen",FopenFunction,3,1,"fname","mode","byteorder",NULL);
  context->addFunction("fclose",FcloseFunction,1,1,"handle",NULL);
  context->addFunction("fwrite",FwriteFunction,2,1,"handle","A",NULL);
  context->addFunction("fread",FreadFunction,3,1,"handle","size","precision",NULL);
  context->addFunction("fseek",FseekFunction,3,0,"handle","offset","style",NULL);
  context->addFunction("ftell",FtellFunction,1,1,"handle",NULL);
  context->addFunction("fgetline",FgetlineFunction,1,1,"handle",NULL);
  context->addFunction("feof",FeofFunction,1,1,"handle",NULL);
  context->addSpecialFunction("printf",PrintfFunction,-1,0,NULL);
  context->addFunction("sprintf",SprintfFunction,-1,1,NULL);
  context->addFunction("fprintf",FprintfFunction,-1,0,NULL);
  context->addFunction("fscanf",FscanfFunction,2,-1,"handle","format",NULL);
  context->addFunction("sscanf",SscanfFunction,2,-1,"string","format",NULL);
  context->addFunction("str2num",Str2NumFunction,1,1,"string",NULL);
  context->addFunction("rand",RandFunction,-1,1,NULL);
  context->addFunction("randn",RandnFunction,-1,1,NULL);
  context->addFunction("seed",SeedFunction,2,0,"s1","s2",NULL);
  context->addFunction("randbeta",RandBetaFunction,2,1,"p1","p2",NULL);
  context->addFunction("randf",RandFFunction,2,1,"p1","p2",NULL);
  context->addFunction("randi",RandIFunction,2,1,"p1","p2",NULL);
  context->addFunction("randgamma",RandGammaFunction,2,1,"p1","p2",NULL);
  context->addFunction("randmulti",RandMultiFunction,2,1,"p1","p2",NULL);
  context->addFunction("randnchi",RandGammaFunction,2,1,"p1","p2",NULL);
  context->addFunction("randbin",RandBinFunction,2,1,"p1","p2",NULL);
  context->addFunction("randnbin",RandNBinFunction,2,1,"p1","p2",NULL);
  context->addFunction("randnf",RandNFFunction,3,1,"p1","p2","p3",NULL);
  context->addFunction("randchi",RandChiFunction,1,1,"dof",NULL);
  context->addFunction("randexp",RandExpFunction,1,1,"means",NULL);
  context->addFunction("randp",RandPoissonFunction,1,1,"means",NULL);
  context->addFunction("find",FindFunction,3,-1,"x","k","flags",NULL);
  context->addFunction("conj",ConjFunction,1,1,"x",NULL);
  context->addFunction("real",RealFunction,1,1,"x",NULL);
  context->addFunction("imag",ImagFunction,1,1,"x",NULL);
  context->addFunction("abs",AbsFunction,1,1,"x",NULL);
  context->addSpecialFunction("save",SaveFunction,-1,0,NULL);
  context->addSpecialFunction("load",LoadFunction,1,0,"filename",NULL);
  context->addSpecialFunction("clear",ClearFunction,-1,0,NULL);
  context->addSpecialFunction("setprintlimit",SetPrintLimitFunction,1,0,"n",NULL);
  context->addSpecialFunction("getprintlimit",GetPrintLimitFunction,0,1,NULL);
  context->addFunction("sleep",SleepFunction,1,0,"n",NULL);
  context->addFunction("eps",EpsFunction,0,1,NULL);
  context->addFunction("feps",FepsFunction,0,1,NULL);
  context->addFunction("repmat",RepMatFunction,3,1,"x","rows","cols",NULL);
  context->addSpecialFunction("docli",DoCLIFunction,0,0,NULL);
  context->addFunction("tic",TicFunction,0,0,NULL);
  context->addFunction("toc",TocFunction,0,1,NULL);
  context->addFunction("xnrm2",XNrm2Function,1,1,"x",NULL);
  context->addFunction("clock",ClockFunction,0,1,NULL);
  context->addFunction("clocktotime",ClockToTimeFunction,1,1,"x",NULL);
  context->addSpecialFunction("help",HelpFunction,1,0,"function",NULL);
  context->addSpecialFunction("pcode",PCodeFunction,-1,0,NULL);
  context->addSpecialFunction("getline",GetLineFunction,1,1,"prompt",NULL);
  context->addSpecialFunction("sparse",SparseFunction,-1,1,"x",NULL);
  context->addFunction("full",FullFunction,1,1,"x",NULL);
  context->addFunction("lu",LUFunction,1,-1,"x",NULL);
  context->addFunction("sort",SortFunction,3,2,"A","dim","mode",NULL);
  context->addFunction("unique",UniqueFunction,2,3,"x","mode",NULL);
  context->addSpecialFunction("dbauto",DbAutoFunction,1,1,"x",NULL);
  context->addSpecialFunction("mfilename",MFilenameFunction,-1,1,NULL);
  context->addFunction("computer",ComputerFunction,0,1,NULL);
  context->addFunction("strstr",StrStrFunction,2,1,"x","y",NULL);
  context->addFunction("strrep_string",StrRepStringFunction,3,1,"source","pattern","replace",NULL);
  context->addFunction("conv2",Conv2Function,4,1,"A","B","C","keep",NULL);
  context->addFunction("inv",InvFunction,1,1,"A",NULL);
  context->addFunction("rcond",RcondFunction,1,1,"A",NULL);
  context->addFunction("end",EndFunction,3,1,"x","n","dims",NULL);
  context->addFunction("xmlread",XMLReadFunction,1,1,"filename",NULL);
  context->addFunction("htmlread",HTMLReadFunction,1,1,"filename",NULL);
  context->addFunction("urlwrite",URLWriteFunction,3,1,"url","filename","timeout",NULL);
  context->addFunction("p_end",EndFunction,3,1,"x","n","dims",NULL);
  context->addSpecialFunction("matload",MatLoadFunction,-1,0,NULL);
  context->addSpecialFunction("matsave",MatSaveFunction,-1,0,NULL);
  InitializeFileSubsystem();
#ifdef USE_MPI
  LoadMPIFunctions(context);
#endif
}
