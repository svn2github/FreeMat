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
//
// These are both done.  Next is the issue of parent classes.
// What does it mean when we have one or more parent classes?
// The structure is simple enough (simply add a new field with
// the name of the parent class).  But what about methods?
// When we call a method of the parent class on the current class
// what does it get passed?
//
//  The answer:
//   Suppose g is of class1, and inherits class2, and class3.
//   Then g has fields class2 and class3.
//   When we call 
//     method(g), where method is a member function of class2, then
//   effectively, the output is the same as
//     method(g.class2)
//     p = g
//     p.class2 = method(g.class2)
//   Odd - that's not quite right...  it must be more complicated than that
// Class related issues
//    rhs subscripting
//    assignment
//
// What about function pointers?   - done
//
// Need overload capability for
//   And
//   Or
//   Uplus
//   a(s1,s2,...,sn) - subsref
//   a(s1, ..., sn) - subsasgn
//   b(a) - subsindex
//   [a b] - horzcat
//   [a; b] - vertcat
//   Colon
//   end (!)
//
// More ideas on overloading classes...
//
// What happens when the parent classes are different sizes
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

  stringVector UserClass::getParentClasses() {
    return parentClasses;
  }

  Array ClassAux(Array s, std::string classname, stringVector parentNames, 
		 ArrayVector parents, WalkTree* eval) {
    UserClass newclass(s.getFieldNames(),parentNames);
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
    // Set up the new structure array.  We do this by constructing a set of fieldnames
    // that includes fields for the parent classes...  To resolve - what happens
    // if the parent arrays are different sizes than the current class.
    stringVector newfields(s.getFieldNames());
    // We should check for duplicates!
    for (int i=0;i<parentNames.size();i++)
      newfields.push_back(parentNames[i]);
    // Now check to make sure all of the parent objects are the same size
    // as the source object
    for (int i=0;i<parents.size();i++) 
      if (!s.getDimensions().equals(parents[i].getDimensions()))
	throw Exception("parent object much match dimensions of the structure used to make the object");
    // Finally, we can construct the new structure object.
    Array* dp = (Array *) Array::allocateArray(FM_STRUCT_ARRAY,s.getLength(),newfields);
    const Array* sp = (const Array*) s.getDataPointer();
    // Now we copy in the data from the original structure
    int oldFieldCount(s.getFieldNames().size());
    int newFieldCount(newfields.size());
    int arrayLength(s.getLength());
    for (int i=0;i<arrayLength;i++)
      for (int j=0;j<oldFieldCount;j++) {
	dp[i*newFieldCount+j] = sp[i*oldFieldCount+j];
      }
    // Now we copy in the data from the parent objects
    for (int j=0;j<parents.size();j++) 
      for (int i=0;i<arrayLength;i++) {
	Array ndx(Array::int32Constructor(i+1));
	dp[i*newFieldCount+oldFieldCount+j] = parents[j].getVectorSubset(ndx);
      }
    // return a new object with the specified properties
    Array retval(FM_STRUCT_ARRAY,s.getDimensions(),dp,false,newfields);
    retval.setClassName(classname);
    return retval;
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
    ArrayVector parents;
    stringVector parentNames;
    for (int i=2;i<arg.size();i++) {
      Array parent(arg[i]);
      if (!parent.isUserClass())
	throw Exception("parent objects must be user defined classes");
      parents.push_back(parent);
      parentNames.push_back(parent.getClassName());
    }
    Array sval(arg[0]);
    Array classname(arg[1]);
    return singleArrayVector(ClassAux(sval,classname.getContentsAsCString(),
				      parentNames,parents,eval));
  }

  void LoadClassFunction(Context* context) {
    SpecialFunctionDef *sfdef = new SpecialFunctionDef;
    sfdef->retCount = 1;
    sfdef->argCount = -1;
    sfdef->name = "class";
    sfdef->fptr = ClassFunction;
    context->insertFunctionGlobally(sfdef,false);
  }

  Array ClassUnaryOperator(Array a, std::string funcname,
			   WalkTree* eval) {
    FuncPtr val;
    ArrayVector m, n;
    if (eval->getContext()->lookupFunction(std::string("@") + a.getClassName() + "_" + funcname,val)) {
      val->updateCode();
      m.push_back(a);
      n = val->evaluateFunction(eval,m,1);
      if (!n.empty())
	return n[0];
      else
	return Array::emptyConstructor();
    }
    throw Exception("Unable to find a definition of " + funcname + " for arguments of class " + a.getClassName());
  }

  bool ClassResolveFunction(WalkTree* eval, std::string classname,
			    std::string funcName, FuncPtr& val) {
    Context *context = eval->getContext();
    // First try to resolve to a method of the base class
    if (context->lookupFunction("@" + classname + "_" + funcName,val)) {
      eval->setClassPrefix("");
      return true;
    } 
    UserClass eclass(eval->lookupUserClass(classname));
    stringVector parentClasses(eclass.getParentClasses());
    // Now check the parent classes
    for (int i=0;i<parentClasses.size();i++) {
      if (context->lookupFunction("@" + parentClasses[i] + "_" + funcName,val)) {
	eval->setClassPrefix(parentClasses[i]);
	return true;
      }
    }
    // Nothing matched, return
    return false;
  }

  Array ClassBinaryOperator(Array a, Array b, std::string funcname,
			    WalkTree* eval) {
    FuncPtr val;
    ArrayVector m, n;
    if (a.isUserClass()) {
      if (eval->getContext()->lookupFunction(std::string("@") + a.getClassName() + "_" + funcname,val)) {
	val->updateCode();
	m.push_back(a); m.push_back(b);
	n = val->evaluateFunction(eval,m,1);
	if (!n.empty())
	  return n[0];
	else
	  return Array::emptyConstructor();
      }
    } else if (b.isUserClass()) {
      if (eval->getContext()->lookupFunction(std::string("@") + b.getClassName() + "_" + funcname,val)) {
	val->updateCode();
	m.push_back(a); m.push_back(b);
	n = val->evaluateFunction(eval,m,1);
	if (!n.empty())
	  return n[0];
	else
	  return Array::emptyConstructor();
      }
    }
    throw Exception("Unable to find a definition of " + funcname + " for arguments of class " + a.getClassName() + " and " + b.getClassName());
  }

}
