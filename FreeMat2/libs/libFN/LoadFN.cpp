#include "FN.hpp"

namespace FreeMat {
  void LoadFNFunctions(Context* context) {
    context->addSpecialFunction("fitfun",FitFunFunction,-1,2);
    context->addFunction("interplin1",Interplin1Function,4,1,"x1","y1","xi","extrap");
    context->addFunction("expei",ExpeiFunction,1,1,"x");
    context->addFunction("eone",EoneFunction,1,1,"x");
    context->addFunction("ei",EiFunction,1,1,"x");
    context->addFunction("erfcx",ErfcxFunction,1,1,"x");
    context->addFunction("erfc",ErfcFunction,1,1,"x");
    context->addFunction("erf",ErfFunction,1,1,"x");
    context->addFunction("dawson",DawsonFunction,1,1,"x");
    context->addFunction("psi",PsiFunction,1,1,"x");
    context->addFunction("gamma",GammaFunction,1,1,"x");
    context->addFunction("gammaln",GammaLnFunction,1,1,"x");
  }
}
