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

#include "Core.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include "Malloc.hpp"
#include <math.h>

namespace FreeMat {
   void LoadCoreFunctions(Context* context) {
     stringVector args;

     args.push_back("x");
     context->addFunction("cos",CosFunction,1,1,args);
     context->addFunction("acos",ArccosFunction,1,1,args);
     context->addFunction("csc",CscFunction,1,1,args);
     context->addFunction("sec",SecFunction,1,1,args);
     context->addFunction("sin",SinFunction,1,1,args);
     context->addFunction("asin",ArcsinFunction,1,1,args);
     context->addFunction("tan",TanFunction,1,1,args);
     context->addFunction("atan",ArctanFunction,1,1,args);
     args.clear();
     args.push_back("y");
     args.push_back("x");
     context->addFunction("atan2",Arctan2Function,2,1,args);
     args.clear();
     args.push_back("x");
     context->addFunction("cot",CotFunction,1,1,args);
     context->addFunction("exp",ExpFunction,1,1,args);
     context->addFunction("log",LogFunction,1,1,args);
     args.clear();
     args.push_back("x");
     args.push_back("z");
     args.push_back("n");
     context->addFunction("min",MinFunction,3,2,args);
     context->addFunction("max",MaxFunction,3,2,args);
     args.clear();
     args.push_back("x");
     args.push_back("d");
     context->addFunction("sum",SumFunction,2,1,args);
     context->addFunction("cumsum",CumsumFunction,2,1,args);
     context->addFunction("mean",MeanFunction,2,1,args);
     context->addFunction("var",VarFunction,2,1,args);
     context->addFunction("prod",ProdFunction,2,1,args);
     args.clear();
     args.push_back("x");
     context->addFunction("ceil",CeilFunction,1,1,args);
     context->addFunction("floor",FloorFunction,1,1,args);
     args.clear();
     args.push_back("x");
     args.push_back("n");
     args.push_back("d");
     context->addFunction("fft",FFTFunction,3,1,args);
     context->addFunction("ifft",IFFTFunction,3,1,args);
     args.clear();
     args.push_back("x");
     context->addSpecialFunction("disp",DispFunction,1,0,args);
     args.clear();
     context->addFunction("i",IFunction,0,1,args);
     context->addFunction("j",IFunction,0,1,args);
     context->addFunction("pi",PiFunction,0,1,args);
     context->addFunction("e",EFunction,0,1,args);
     //     context->addFunction("length",LengthFunction,1,1);
     args.clear();
     args.push_back("x");     
     context->addFunction("float",FloatFunction,1,1,args);
     context->addFunction("double",DoubleFunction,1,1,args);
     context->addFunction("complex",ComplexFunction,1,1,args);
     context->addFunction("dcomplex",DcomplexFunction,1,1,args);
     context->addFunction("uint8",UInt8Function,1,1,args);
     context->addFunction("uint16",UInt16Function,1,1,args);
     context->addFunction("uint32",UInt32Function,1,1,args);
     context->addFunction("int8",Int8Function,1,1,args);
     context->addFunction("int16",Int16Function,1,1,args);
     context->addFunction("int32",Int32Function,1,1,args);  
     context->addFunction("eig",EigFunction,1,2,args);
     context->addFunction("svd",SVDFunction,1,3,args);
     args.clear();
     args.push_back("x");
     args.push_back("n");
     context->addFunction("diag",DiagFunction,2,1,args);
     args.clear();
     args.push_back("x");     
     context->addFunction("isempty",IsEmptyFunction,1,1,args);
     args.clear();
     context->addFunction("zeros",ZerosFunction,-1,1,args);
     context->addFunction("reshape",ReshapeFunction,-1,1,args);
     context->addFunction("ones",OnesFunction,-1,1,args);
     context->addFunction("pwd",PrintWorkingDirectoryFunction,0,1,args);
     args.clear();
     args.push_back("x");     
     context->addFunction("test",TestFunction,1,1,args);
     context->addFunction("error",ErrorFunction,1,0,args);
     context->addSpecialFunction("lasterr",LasterrFunction,1,1,args);
     context->addFunction("typeof",TypeOfFunction,1,1,args);
     args.clear();
     args.push_back("x");
     args.push_back("y");
     context->addFunction("strcmp",StrCmpFunction,2,1,args);
     args.clear();
     context->addFunction("struct",StructFunction,-1,1,args);
     context->addFunction("size",SizeFunction,-1,-1,args);
     context->addFunction("nan",NaNFunction,0,1,args);
     context->addFunction("NaN",NaNFunction,0,1,args);
     context->addFunction("inf",InfFunction,0,1,args);
     context->addFunction("Inf",InfFunction,0,1,args);
     args.clear();
     args.push_back("dirname");     
     context->addSpecialFunction("cd",ChangeDirFunction,1,0,args);
     args.clear();
     context->addSpecialFunction("ls",ListFilesFunction,-1,0,args);
     context->addSpecialFunction("dir",ListFilesFunction,-1,0,args);
     args.clear();
     args.push_back("command");
     context->addFunction("system",SystemFunction,1,1,args);
//      context->addFunction("rinit",RInitFunction,0,0,args);
//      context->addFunction("rstart",RStartFunction,1,1,args);
//      context->addFunction("rcall",RCallFunction,-1,1,args);
//      context->addFunction("rret",RRetFunction,1,-1,args);
     args.clear();
     context->addSpecialFunction("feval",FevalFunction,-1,-1,args);
     args.clear();
     args.push_back("function");
     context->addSpecialFunction("fdump",FdumpFunction,1,0,args);
     args.clear();
     context->addSpecialFunction("eval",EvalFunction,-1,-1,args);
     args.clear();
     args.push_back("filename");
     context->addSpecialFunction("source",SourceFunction,1,0,args);
     args.clear();
     context->addSpecialFunction("who",WhoFunction,-1,0,args);
     args.push_back("varname");
     context->addSpecialFunction("exist",ExistFunction,1,1,args);
     args.clear();
     args.push_back("fname");
     args.push_back("mode");
     args.push_back("byteorder");
     context->addFunction("fopen",FopenFunction,3,1,args);
     args.clear();
     args.push_back("handle");
     context->addFunction("fclose",FcloseFunction,1,0,args);
     args.clear();
     args.push_back("handle");
     args.push_back("A");
     context->addFunction("fwrite",FwriteFunction,2,1,args);
     args.clear();
     args.push_back("handle");
     args.push_back("size");
     args.push_back("precision");
     context->addFunction("fread",FreadFunction,3,1,args);
     args.clear();
     args.push_back("handle");
     args.push_back("offset");
     args.push_back("style");
     context->addFunction("fseek",FseekFunction,3,0,args);
     args.clear();
     args.push_back("handle");     
     context->addFunction("ftell",FtellFunction,1,1,args);
     context->addFunction("fgetline",FgetlineFunction,1,1,args);
     args.clear();
     args.push_back("handle");     
     context->addFunction("feof",FeofFunction,1,1,args);
     args.clear();
     context->addSpecialFunction("printf",PrintfFunction,-1,0,args);
     args.clear();
     context->addFunction("sprintf",SprintfFunction,-1,1,args);
     args.clear();
     context->addFunction("fprintf",FprintfFunction,-1,0,args);
     args.clear();
     args.push_back("handle");
     args.push_back("format");
     context->addFunction("fscanf",FscanfFunction,2,-1,args);
//      args.clear();
//      args.push_back("address");
//      args.push_back("portnumber");
//      context->addFunction("connect",ConnectFunction,2,1,args);
//      args.clear();
//      args.push_back("serverHandle");
//      context->addFunction("accept",AcceptFunction,1,1,args);
//      args.clear();
//      args.push_back("portnumber");
//      context->addFunction("server",ServerFunction,1,2,args);
//      args.clear();
//      args.push_back("handle");
//      args.push_back("x");
//      context->addFunction("send",SendFunction,2,0,args);
//      args.clear();
//      args.push_back("handle");
//      context->addFunction("receive",ReceiveFunction,1,1,args);
     args.clear();
     context->addFunction("rand",RandFunction,-1,1,args);
     context->addFunction("randn",RandnFunction,-1,1,args);
     args.clear();
     args.push_back("s");     
     context->addFunction("seed",SeedFunction,1,0,args);
     args.clear();
     args.push_back("x");          
     context->addFunction("find",FindFunction,1,1,args);
     context->addFunction("conj",ConjFunction,1,1,args);
     context->addFunction("real",RealFunction,1,1,args);
     context->addFunction("imag",ImagFunction,1,1,args);
     context->addFunction("abs",AbsFunction,1,1,args);
     args.clear();
     context->addSpecialFunction("save",SaveFunction,-1,0,args);
     args.clear();
     args.push_back("filename");
     context->addSpecialFunction("load",LoadFunction,1,0,args);
     args.clear();
     context->addSpecialFunction("clear",ClearFunction,-1,0,args);
     args.clear();
     args.push_back("n");
     context->addSpecialFunction("setprintlimit",SetPrintLimitFunction,1,0,args);
     args.clear();
     context->addSpecialFunction("getprintlimit",GetPrintLimitFunction,0,1,args);
     args.clear();
     args.push_back("n");
     context->addFunction("sleep",SleepFunction,1,0,args);
     //     context->addFunction("clock",ClockFunction,0,1,args);
     //      context->addSpecialFunction("whoglobals",WhoGlobalsFunction,0,0,args);
     args.clear();
     args.push_back("function");
     context->addSpecialFunction("help",HelpFunction,1,0,args);
     InitializeFileSubsystem();
   }
}
