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
    stringVector getParentClasses();
    ~UserClass();
  };

  Array ClassTrinaryOperator(Array a, Array b, Array c, std::string funcname, WalkTree* eval);

  Array ClassBinaryOperator(Array a, Array b, std::string funcname,
			    WalkTree* eval);

  Array ClassUnaryOperator(Array a, std::string funcname,
			   WalkTree* eval);

  Array ClassMatrixConstructor(ArrayMatrix m, WalkTree* eval);

  ArrayVector ClassFunction(int nargout, const ArrayVector& arg,
			    WalkTree* eval);

  ArrayVector ClassRHSExpression(Array a, ASTPtr t, WalkTree* eval);

  void ClassAssignExpression(Array* r, ASTPtr t, ArrayVector& value, WalkTree* eval);
  
  bool ClassResolveFunction(WalkTree* eval, Array &args, std::string funcName, FuncPtr& val);
  
  std::string ClassMangleName(std::string className, std::string funcName);

  void LoadClassFunction(Context* context);
}
#endif
