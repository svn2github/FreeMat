#include "FN.hpp"

namespace FreeMat {
  void LoadFNFunctions(Context* context) {
    stringVector args;
    args.push_back("fcn");
    args.push_back("xinit");
    args.push_back("y");
    args.push_back("tol");
    args.push_back("params");
    context->addSpecialFunction("fitfun",FitFunFunction,-1,2,args);
    args.clear();
    args.push_back("x1");
    args.push_back("y1");
    args.push_back("xi");
    args.push_back("extrap");
    context->addFunction("interplin1",Interplin1Function,4,1,args);
    args.clear();
    args.push_back("x");
    context->addFunction("expei",ExpeiFunction,1,1,args);
    context->addFunction("eone",EoneFunction,1,1,args);
    context->addFunction("ei",EiFunction,1,1,args);
    context->addFunction("erfcx",ErfcxFunction,1,1,args);
    context->addFunction("erfc",ErfcFunction,1,1,args);
    context->addFunction("erf",ErfFunction,1,1,args);
    context->addFunction("dawson",DawsonFunction,1,1,args);
    context->addFunction("psi",PsiFunction,1,1,args);
    context->addFunction("gamma",GammaFunction,1,1,args);
    context->addFunction("gammaln",GammaLnFunction,1,1,args);
  }
}
