#ifndef __Class_hpp__
#define __Class_hpp__

#include "Array.hpp"
#include "WalkTree.hpp"
#include "Types.hpp"

namespace FreeMat {
  class UserClass {
    stringVector fieldNames;
    stringVector parentClasses;
  public:
    UserClass();
    UserClass(stringVector fields, stringVector parents);
    bool matchClass(UserClass test);
    ~UserClass();
  };

  ArrayVector ClassFunction(int nargout, const ArrayVector& arg,
			    WalkTree* eval);
  void LoadClassFunction(Context* context);
}
#endif
