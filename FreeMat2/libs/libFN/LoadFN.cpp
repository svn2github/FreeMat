#include "FN.hpp"

namespace FreeMat {
  void LoadFNFunctions(Context* context) {
    stringVector args;
    args.push_back("fcn");
    args.push_back("xinit");
    args.push_back("y");
    args.push_back("tol");
    args.push_back("params");
    context->addSpecialFunction("fitfun",FitFunFunction,-1,1,args);
  }
}
