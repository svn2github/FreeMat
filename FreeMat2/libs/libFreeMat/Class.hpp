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

  Array ClassBinaryOperator(Array a, Array b, std::string funcname,
			    WalkTree* eval);

  Array ClassUnaryOperator(Array a, std::string funcname,
			   WalkTree* eval);

  ArrayVector ClassFunction(int nargout, const ArrayVector& arg,
			    WalkTree* eval);

  ArrayVector ClassRHSExpression(Array a, ASTPtr t, WalkTree* eval);

  void LoadClassFunction(Context* context);
}
#endif
