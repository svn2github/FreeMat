#include "Class.hpp"
#include "Context.hpp"

// some behavioral observations on classes.
//  The first call to "class" is the definitive one.
//  The exact order of the structure fieldnames must be the same 
//     for all objects
//  The list of parent objects must also be the same for all objects
//  So, classes are stored as the following:
//     class UserClass {
//         stringVector fieldNames;
//         stringVector parentClasses;
//     }
//  Also, somewhere we require a table that
//  tracks the hierarchy relationship of the classes.
// 
// To Do:
//   change grepping code to look for classes
//   change function eval code to handle classes

namespace FreeMat {
  UserClass::UserClass() {
  }

  UserClass::UserClass(stringVector fields, stringVector parents) :
    fieldNames(fields), parentClasses(parents) {
  }

  bool UserClass::matchClass(UserClass test) {
    return ((fieldNames == test.fieldNames) &&
	    (parentClasses == test.parentClasses));
  }

  UserClass::~UserClass() {
  }

  Array ClassAux(Array s, std::string classname, stringVector parents, WalkTree* eval) {
    UserClass newclass(s.getFieldNames(),parents);
    if (s.getDataClass() != FM_STRUCT_ARRAY) 
      throw Exception("first argument to 'class' function must be a structure");
    // Check to see if this class has already been registered
    if (!eval->isUserClassDefined(classname)) {
      // new class... register it
      eval->registerUserClass(classname,newclass);
      eval->getInterface()->outputMessage("registered class ");
      eval->getInterface()->outputMessage(classname.c_str());
      eval->getInterface()->outputMessage("\n");
    } else {
      // existing class...  make sure we match it
      UserClass eclass(eval->lookupUserClass(classname));
      if (!eclass.matchClass(newclass))
	throw Exception("fieldnames, and parent objects must match registered class.  Use 'clear classes' to reset this information.");
    }
    s.setClassName(classname);
    return s;
  }

  Array ClassOneArgFunction(Array x) {
    if (x.isUserClass())
      return Array::stringConstructor(x.getClassName());
    else {
      switch (x.getDataClass()) {
      case FM_CELL_ARRAY:
	return Array::stringConstructor("cell");
      case FM_STRUCT_ARRAY:
	return Array::stringConstructor("struct");
      case FM_LOGICAL:
	return Array::stringConstructor("logical");
      case FM_UINT8:
	return Array::stringConstructor("uint8");
      case FM_INT8:
	return Array::stringConstructor("int8");
      case FM_UINT16:
	return Array::stringConstructor("uint16");
      case FM_INT16:
	return Array::stringConstructor("int16");
      case FM_UINT32:
	return Array::stringConstructor("uint32");
      case FM_INT32:
	return Array::stringConstructor("int32");
      case FM_FLOAT:
	return Array::stringConstructor("float");
      case FM_DOUBLE:
	return Array::stringConstructor("double");
      case FM_COMPLEX:
	return Array::stringConstructor("complex");
      case FM_DCOMPLEX:
	return Array::stringConstructor("dcomplex");
      case FM_STRING:
	return Array::stringConstructor("string");
      }
    }
  }
  
  ArrayVector ClassFunction(int nargout, const ArrayVector& arg,
			    WalkTree* eval) {
    if (arg.size() == 0)
      throw Exception("class function requires at least one argument");
    if (arg.size() == 1)
      return singleArrayVector(ClassOneArgFunction(arg[0]));
    stringVector parentNames;
    for (int i=2;i<arg.size();i++) {
      Array parent(arg[i]);
      if (!parent.isUserClass())
	throw Exception("parent objects must be user defined classes");
      parentNames.push_back(parent.getClassName());
    }
    Array sval(arg[0]);
    Array classname(arg[1]);
    return singleArrayVector(ClassAux(sval,classname.getContentsAsCString(),parentNames,eval));
  }

  void LoadClassFunction(Context* context) {
    SpecialFunctionDef *sfdef = new SpecialFunctionDef;
    sfdef->retCount = 1;
    sfdef->argCount = -1;
    sfdef->name = "class";
    sfdef->fptr = ClassFunction;
    context->insertFunctionGlobally(sfdef,false);
  }
}
