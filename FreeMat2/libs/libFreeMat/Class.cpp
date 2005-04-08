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
// What happens when the parent classes are different sizes - resolved
//    force parent classes to be the same size as the created object
//
// In c++, polymorphism is done through the notion of a pointer and
// type casting.  But we can't do exactly the same thing... Because
// when we type-cast, only methods and fields from the type-cast
// object are present... 
//
// What we want is 
//   a.class1.class2.foo = 32
// In this case, a is of some class (e.g., class3).  But we want to
// call some method on a that belongs to class2.  now, inside the
// method, we want something like
//    x.foo = 32
// but _x_ has to be tagged with prefix information, because _x_ is
// really of class class3.  The tag has to be on the object because
// if there are multiple arguments to the function, they can be
// typecast at different levels.  Also, it tracks only the _instance_
// of the array, not the core array itself.  So the information has
// to be tagged on the array somehow.
//
// One idea is to replace the class name with the class path.  So if
// a is of type class3, but we want to access it as a type class2,
// we "cast" it to type class3:class1:class2.  Then, when accessing
// members of "a", we use the class list to determine the indexing
// sequence.  This casting operation can be done at the dispatch
// level.  Because the "struct" operation simply strips the class name
// from the object, it will still return the intact data array.
//
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
	stringVector cp;
	cp.push_back(classname);
    retval.setClassName(cp);
    return retval;
  }

  Array ClassOneArgFunction(Array x) {
    if (x.isUserClass())
      return Array::stringConstructor(x.getClassName().back());
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
      parentNames.push_back(parent.getClassName().back());
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
    if (eval->getContext()->lookupFunction(std::string("@") + a.getClassName().back() + "_" + funcname,val)) {
      val->updateCode();
      m.push_back(a);
      n = val->evaluateFunction(eval,m,1);
      if (!n.empty())
	return n[0];
      else
	return Array::emptyConstructor();
    }
    throw Exception("Unable to find a definition of " + funcname + " for arguments of class " + a.getClassName().back());
  }

  bool ClassResolveFunction(WalkTree* eval, Array& args, std::string funcName, FuncPtr& val) {
    Context *context = eval->getContext();
    // First try to resolve to a method of the base class
    if (context->lookupFunction("@" + args.getClassName().back() + "_" + funcName,val)) {
      return true;
    } 
    UserClass eclass(eval->lookupUserClass(args.getClassName().back()));
    stringVector parentClasses(eclass.getParentClasses());
    // Now check the parent classes
    for (int i=0;i<parentClasses.size();i++) {
      if (context->lookupFunction("@" + parentClasses[i] + "_" + funcName,val)) {
	stringVector argClass(args.getClassName());
	argClass.push_back(parentClasses[i]);
	args.setClassName(argClass);
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
      if (eval->getContext()->lookupFunction(std::string("@") + a.getClassName().back() + "_" + funcname,val)) {
	val->updateCode();
	m.push_back(a); m.push_back(b);
	n = val->evaluateFunction(eval,m,1);
	if (!n.empty())
	  return n[0];
	else
	  return Array::emptyConstructor();
      }
    } else if (b.isUserClass()) {
      if (eval->getContext()->lookupFunction(std::string("@") + b.getClassName().back() + "_" + funcname,val)) {
	val->updateCode();
	m.push_back(a); m.push_back(b);
	n = val->evaluateFunction(eval,m,1);
	if (!n.empty())
	  return n[0];
	else
	  return Array::emptyConstructor();
      }
    }
    throw Exception("Unable to find a definition of " + funcname + " for arguments of class " + a.getClassName().back() + " and " + b.getClassName().back());
  }

  ArrayVector ClassSubsrefCall(WalkTree* eval, ASTPtr t, Array r, FuncPtr val) {
    stringVector fNames;
    fNames.push_back("type");
    fNames.push_back("subs");
    Dimensions rhsDimensions;
    while (t != NULL) {
      rhsDimensions = r.getDimensions();
      if (!rv.empty()) 
	throw Exception("Cannot reindex an expression that returns multiple values.");
      if (t->opNum ==(OP_PARENS)) {
	m = eval->expressionList(t->down,&rhsDimensions);
	if (m.size() == 0) 
	  throw Exception("Expected indexing expression!");
	// Take the arguments and push them into a cell array...
	ArrayMatrix q;	q.push_back(m);
	Array args(Array::cellConstructor(q));
	ArrayVector tmp;
	tmp.push_back(Array::stringConstructor("()"));
	tmp.push_back(args);
	Array strct(Array::structConstructor(fNames,tmp));
      }
      if (t->opNum ==(OP_BRACES)) {
	m = eval->expressionList(t->down,&rhsDimensions);
	if (m.size() == 0) 
	  throw Exception("Expected indexing expression!");
	// Take the arguments and push them into a cell array...
	ArrayMatrix q;	q.push_back(m);
	Array args(Array::cellConstructor(q));
	ArrayVector tmp;
	tmp.push_back(Array::stringConstructor("{}"));
	tmp.push_back(args);
	Array strct(Array::structConstructor(fNames,tmp));
      }
      if (t->opNum ==(OP_DOT)) {
	ArrayVector tmp;
	tmp.push_back(Array::stringConstructor("."));
	tmp.push_back(t->down->text);
	Array strct(Array::structConstructor(fNames,tmp));
      }
    }
  }

  // What is special here...  Need to be able to do field indexing
  // 
  ArrayVector ClassRHSExpression(Array r, ASTPtr t, WalkTree* eval) {
    ASTPtr s;
    Array q;
    Array n, p;
    ArrayVector m;
    int peerCnt;
    int dims;
    bool isVar;
    bool isFun;
    FuncPtr val;

    // Try and look up subsref...
    if (ClassResolveFunction(eval,r,"subsref",val)) {
      // Overloaded subsref case
      return ClassSubsrefCall(eval,t,r,val);
    }

    ArrayVector rv;
    Dimensions rhsDimensions;
    while (t != NULL) {
      rhsDimensions = r.getDimensions();
      if (!rv.empty()) 
	throw Exception("Cannot reindex an expression that returns multiple values.");
      if (t->opNum ==(OP_PARENS)) {
	m = eval->expressionList(t->down,&rhsDimensions);
	if (m.size() == 0) 
	  throw Exception("Expected indexing expression!");
	else if (m.size() == 1) {
	  q = r.getVectorSubset(m[0]);
	  r = q;
	} else {
	  q = r.getNDimSubset(m);
	  r = q;
	}
      }
      if (t->opNum ==(OP_BRACES)) {
	m = eval->expressionList(t->down,&rhsDimensions);
	if (m.size() == 0) 
	  throw Exception("Expected indexing expression!");
	else if (m.size() == 1)
	  rv = r.getVectorContentsAsList(m[0]);
	else
	  rv = r.getNDimContentsAsList(m);
	if (rv.size() == 1) {
	  r = rv[0];
	  rv = ArrayVector();
	} else if (rv.size() == 0) {
	  throw Exception("Empty expression!");
	  r = Array::emptyConstructor();
	}
      }
      if (t->opNum ==(OP_DOT)) {
	// This is where the classname chain comes into being.
	stringVector className = r.getClassName();
	for (int i=1;i<className.size();i++) {
	  rv = r.getFieldAsList(className[i]);
	  r = rv[0];
	}
	rv = r.getFieldAsList(t->down->text);
	if (rv.size() <= 1) {
	  r = rv[0];
	  rv = ArrayVector();
	}
      }
      if (t->opNum == (OP_DOTDYN)) {
	char *field;
	try {
	  Array fname(eval->expression(t->down));
	  field = fname.getContentsAsCString();
	} catch (Exception &e) {
	  throw Exception("dynamic field reference to structure requires a string argument");
	}
	rv = r.getFieldAsList(field);
	if (rv.size() <= 1) {
	  r = rv[0];
	  rv = ArrayVector();
	}      
      }
      t = t->right;
    }
    if (rv.empty())
      rv.push_back(r);
    return rv;
  }
}
