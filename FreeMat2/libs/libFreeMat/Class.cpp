/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
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
// To track precedence...
// 1.  Assume that inheritance and precedence do not interact (only the
// outermost class determines precedence).
// 2.  For each class, a list of superior classes is provided.
// 3.  A inf B <--> B sup A
// Precedence is then a simple question of testing interactions.

UserClass::UserClass() {
}

UserClass::UserClass(rvstring fields, rvstring parents) :
  fieldNames(fields), parentClasses(parents) {
}

bool UserClass::matchClass(UserClass test) {
  return (((fieldNames) == (test.fieldNames)) &&
	  ((parentClasses) == (test.parentClasses)));
}

UserClass::~UserClass() {
}

rvstring UserClass::getParentClasses() {
  return parentClasses;
}

Array ClassAux(Array s, std::string classname, rvstring parentNames, 
	       ArrayVector parents, Interpreter* eval) {
  UserClass newclass(s.getFieldNames(),parentNames);
  if (s.getDataClass() != FM_STRUCT_ARRAY) 
    throw Exception("first argument to 'class' function must be a structure");
  // Check to see if this class has already been registered
  if (!eval->isUserClassDefined(classname)) {
    // new class... register it
    eval->registerUserClass(classname,newclass);
  } else {
    // existing class...  make sure we match it
    UserClass eclass(eval->lookupUserClass(classname));
    if (!eclass.matchClass(newclass))
      throw Exception("fieldnames, and parent objects must match registered class.  Use 'clear classes' to reset this information.");
  }
  // Set up the new structure array.  We do this by constructing a set of fieldnames
  // that includes fields for the parent classes...  To resolve - what happens
  // if the parent arrays are different sizes than the current class.
  rvstring newfields(s.getFieldNames());
  // We should check for duplicates!
  for (int i=0;i<parentNames.size();i++)
    newfields.push_back(parentNames.at(i));
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
  rvstring cp;
  cp.push_back(classname);
  retval.setClassName(cp);
  return retval;
}

Array ClassOneArgFunction(Array x) {
  if (x.isUserClass()) {
    std::string p(x.getClassName().back());
    return Array::stringConstructor(x.getClassName().back());
  } else {
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
    case FM_UINT64:
      return Array::stringConstructor("uint64");
    case FM_INT64:
      return Array::stringConstructor("int64");
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
    case FM_FUNCPTR_ARRAY:
      return Array::stringConstructor("function handle");
    default:
      throw Exception("Unrecognized class!");
    }
  }
}

//!
//@Module CLASS Class Support Function
//@@Section CLASS
//@@Usage
//There are several uses for the @|class| function.  The first
//version takes a single argument, and returns the class of
//that variable.  The syntax for this form is
//@[
//  classname = class(variable)
//@]
//and it returns a string containing the name of the class for
//@|variable|.  The second form of the class function is used
//to construct an object of a specific type based on a structure
//which contains data elements for the class.  The syntax for
//this version is
//@[
//  classvar = class(template, classname, parent1, parent2,...)
//@]
//This should be called inside the constructor for the class.
//The resulting class will be of the type @|classname|, and will
//be derived from @|parent1|, @|parent2|, etc.  The @|template|
//argument should be a structure array that contains the members
//of the class.  See the @|constructors| help for some details
//on how to use the @|class| function.  Note that if the
//@|template| argument is an empty structure matrix, then the
//resulting variable has no fields beyond those inherited from
//the parent classes.
//!

//!
//@Module CONSTRUCTORS Class Constructors
//@@Section CLASS
//@@Usage
//When designing a constructor for a FreeMat class, you should
//design the constructor to take a certain form.  The following
//is the code for the sample @|mat| object 
//@[
//function p = mat(a)
//  if (nargin == 0)
//    p.c = [];
//    p = class(p,'mat');
//  elseif isa(a,'mat')
//    p = a;
//  else
//    p.c = a;
//    p = class(p,'mat');
//  end
//@]
//Generally speaking when it is provided with zero arguments, the
//constructor returns a default version of the class using a template
//structure with the right fields populated with default values.  If
//the constructor is given a single argument that matches the class
//we are trying to construct, the constructor passes through the
//argument.  This form of the constructor is used for type conversion.
//In particular, 
//@[
//   p = mat(a)
//@] 
//guarantees that @|p| is an array of class @|mat|.  The last form
//of the constructor builds a class object given the input.  The
//meaning of this form depends on what makes sense for your class.
//For example, for a polynomial class, you may want to pass in the
//coefficients of the polynomial.
//!

//!
//@Module HORZCAT Overloaded Horizontal Concatenation
//@@Section CLASS
//@@Usage
//This is a method for a class that is invoked to concatenate two or more
//variables of the same class type together.  Besides being called
//when you invoke
//@[
//   c = horzcat(a,b,c)
//@]
//when @|a| is a class, it is also called for 
//@[
//   c = [a,b,c]
//@]
//when one of these variables is a class.  The exact meaning of
//horizontal concatenation depends on the class you have designed.
//!

//!
//@Module VERTCAT Overloaded Vertical Concatenation
//@@Section CLASS
//@@Usage
//This is a method for a class that is invoked to concatenate two or more
//variables of the same class type together.  Besides being called when
//you invoke
//@[
//   c = vertcat(a,b,c)
//@]
//when @|a| is a class, it is also called for
//@[
//   c = [a;b;c]
//@]
//when one of the variables is a class.  The exact meaning of 
//vertical concatenation depends on the class you have designed.
//!
  
//!
//@Module OR Overloaded Logical Or Operator
//@@Section CLASS
//@@Usage
//This is a method that is invoked to combine two variables using a
//logical or operator, and is invoked when you call
//@[
//   c = or(a,b)
//@]
//or for 
//@[
//   c = a | b
//@]
//!

//!
//@Module AND Overloaded Logical And Operator
//@@Section CLASS
//@@Usage
//This is a method that is invoked to combine two variables using a
//logical and operator, and is invoked when you call
//@[
//   c = and(a,b)
//@]
//or for 
//@[
//   c = a & b
//@]
//!

//!
//@Module LT Overloaded Less Than Comparison Operator
//@@Section CLASS
//@@Usage
//This is a method that is invoked to compare two variables using a
//less than comparison operator, and is invoked when you call
//@[
//   c = lt(a,b)
//@]
//or for 
//@[
//   c = a < b
//@]
//!

//!
//@Module GT Overloaded Greater Than Comparison Operator
//@@Section CLASS
//@@Usage
//This is a method that is invoked to combine two variables using a
//greater than comparison operator, and is invoked when you call
//@[
//   c = gt(a,b)
//@]
//or for 
//@[
//   c = a > b
//@]
//!
  
//!
//@Module LE Overloaded Less-Than-Equals Comparison Operator
//@@Section CLASS
//@@Usage
//This is a method that is invoked to compare two variables using a
//less than or equals comparison operator, and is invoked when you call
//@[
//   c = le(a,b)
//@]
//or for 
//@[
//   c = a <= b
//@]
//!

//!
//@Module GE Overloaded Greater-Than-Equals Comparison Operator
//@@Section CLASS
//@@Usage
//This is a method that is invoked to combine two variables using a
//greater than or equals comparison operator, and is invoked when you call
//@[
//   c = ge(a,b)
//@]
//or for 
//@[
//   c = a >= b
//@]
//!
  
//!
//@Module EQ Overloaded Equals Comparison Operator
//@@Section CLASS
//@@Usage
//This is a method that is invoked to combine two variables using an
//equals comparison operator, and is invoked when you call
//@[
//   c = eq(a,b)
//@]
//or for 
//@[
//   c = a == b
//@]
//!

//!
//@Module NE Overloaded Not-Equals Comparison Operator
//@@Section CLASS
//@@Usage
//This is a method that is invoked to combine two variables using a
//not-equals comparison operator, and is invoked when you call
//@[
//   c = ne(a,b)
//@]
//or for 
//@[
//   c = a != b
//@]
//!

//!
//@Module PLUS Overloaded Addition Operator
//@@Section CLASS
//@@Usage
//This is a method that is invoked when two variables are added
//and is invoked when you call
//@[
//   c = plus(a,b)
//@]
//or for
//@[
//   c = a + b
//@]
//!

//!
//@Module MINUS Overloaded Addition Operator
//@@Section CLASS
//@@Usage
//This is a method that is invoked when two variables are subtracted
//and is invoked when you call
//@[
//   c = minus(a,b)
//@]
//or for
//@[
//   c = a - b
//@]
//!

//!
//@Module MTIMES Overloaded Matrix Multiplication Operator
//@@Section CLASS
//@@Usage
//This is a method that is invoked when two variables are multiplied
//using the matrix operator and is invoked when you call
//@[
//   c = mtimes(a,b)
//@]
//or for
//@[
//   c = a * b
//@]
//!

//!
//@Module TIMES Overloaded Multiplication Operator
//@@Section CLASS
//@@Usage
//This is a method that is invoked when two variables are multiplied
//and is invoked when you call
//@[
//   c = times(a,b)
//@]
//or for
//@[
//   c = a .* b
//@]
//!
  
//!
//@Module RDIVIDE Overloaded Right Divide Operator
//@@Section CLASS
//@@Usage
//This is a method that is invoked when two variables are divided
//and is invoked when you call
//@[
//   c = rdivide(a,b)
//@]
//or for
//@[
//   c = a ./ b
//@]
//!

//!
//@Module LDIVIDE Overloaded Left Divide Operator
//@@Section CLASS
//@@Usage
//This is a method that is invoked when two variables are divided
//and is invoked when you call
//@[
//   c = ldivide(a,b)
//@]
//or for
//@[
//   c = a .\ b
//@]
//!

//!
//@Module MRDIVIDE Overloaded Matrix Right Divide Operator
//@@Section CLASS
//@@Usage
//This is a method that is invoked when two variables are divided
//using the matrix divide operator, and is invoked when you call
//@[
//   c = mrdivide(a,b)
//@]
//or for
//@[
//   c = a / b
//@]
//!

//!
//@Module MLDIVIDE Overloaded Matrix Left Divide Operator
//@@Section CLASS
//@@Usage
//This is a method that is invoked when two variables are divided
//using the matrix (left) divide operator, and is invoked when
//you call
//@[
//   c = mldivide(a,b)
//@]
//or for
//@[
//   c = a \ b
//@]
//!


//!
//@Module UMINUS Overloaded Unary Minus Operator
//@@Section CLASS
//@@Usage
//This is a method that is invoked when a variable is negated,
//and is invoked when you call
//@[
//   c = uminus(a)
//@]
//or for 
//@[
//   c = -a
//@]
//!

//!
//@Module UPLUS Overloaded Unary Plus Operator
//@@Section CLASS
//@@Usage
//This is a method that is invoked when a variable is preceeded by a "+",
//and is invoked when you call
//@[
//   c = uplus(a)
//@]
//or for 
//@[
//   c = +a
//@]
//!

//!
//@Module NOT Overloaded Logical Not Operator
//@@Section CLASS
//@@Usage
//This is a method that is invoked when a variable is logically
//inverted, and is invoked when you call
//@[
//   c = not(a)
//@]
//or for
//@[
//   c = ~a
//@]
//!

//!
//@Module MPOWER Overloaded Matrix Power Operator
//@@Section CLASS
//@@Usage
//This is a method that is invoked when one variable is raised
//to another variable using the matrix power operator, and
//is invoked when you call
//@[
//  c = mpower(a,b)
//@]
//or
//@[
//  c = a^b
//@]
//!

//!
//@Module POWER Overloaded Power Operator
//@@Section CLASS
//@@Usage
//This is a method that is invoked when one variable is raised
//to another variable using the dot-power operator, and is
//invoked when you call
//@[
//   c = power(a,b)
//@]
//or
//@[
//   c = a.^b
//@]
//!

//!
//@Module CTRANSPOSE Overloaded Conjugate Transpose Operator
//@@Section CLASS
//@@Usage
//This is a method that is invoked when a variable has the
//conjugate transpose operator method applied, and is invoked
//when you call
//@[
//   c = ctranspose(a)
//@]
//or
//@[
///  c = a'
//@]
//!

//!
//@Module TRANSPOSE Overloaded Transpose Operator
//@@Section CLASS
//@@Usage
//This is a method that is invoked when a variable has the
//transpose operator method applied, and is invoked
//when you call
//@[
//   c = transpose(a)
//@]
//or
//@[
///  c = a.'
//@]
//!

//!
//@Module COLON Overloaded Colon Operator
//@@Section CLASS
//@@Usage
//This is a method that is invoked in one of two forms, either
//the two argument version
//@[
//  c = colon(a,b)
//@]
//which is also called using the notation
//@[
//  c = a:b
//@]
//and the three argument version
//@[
//  d = colon(a,b,c)
//@]
//which is also called using the notation
//@[
//  d = a:b:c
//@]
//!

//!
//@Module SUBSREF Overloaded Class Indexing
//@@Section CLASS
//@@Usage
//This method is called for expressions of the form
//@[
//  c = a(b), c = a{b}, c = a.b
//@]
//and overloading the @|subsref| method allows you
//to define the meaning of these expressions for
//objects of class @|a|.  These expressions are
//mapped to a call of the form
//@[
//  b = subsref(a,s)
//@]
//where @|s| is a structure array with two fields. The
//first field is
//\begin{itemize}
//\item @|type|  is a string containing either @|'()'| or
// @|'{}'| or @|'.'| depending on the form of the call.
//\item @|subs| is a cell array or string containing the
// the subscript information.
//\end{itemize}
//When multiple indexing experssions are combined together
//such as @|b = a(5).foo{:}|, the @|s| array contains
//the following entries
//@[
//  s(1).type = '()'  s(1).subs = {5}
//  s(2).type = '.'   s(2).subs = 'foo'
//  s(3).type = '{}'  s(3).subs = ':'
//@]
//!

//!
//@Module SUBSASGN Overloaded Class Assignment
//@@Section CLASS
//@@Usage
//This method is called for expressions of the form
//@[
//  a(b) = c, a{b} = c, a.b = c
//@]
//and overloading the @|subsasgn| method can allow you
//to define the meaning of these expressions for
//objects of class @|a|.  These expressions are mapped
//to a call of the form
//@[
//  a = subsasgn(a,s,b)
//@]
//where @|s| is a structure array with two fields.  The
//first field is
//\begin{itemize}
//\item @|type|  is a string containing either @|'()'| or
// @|'{}'| or @|'.'| depending on the form of the call.
//\item @|subs| is a cell array or string containing the
// the subscript information.
//\end{itemize}
//When multiple indexing experssions are combined together
//such as @|a(5).foo{:} = b|, the @|s| array contains
//the following entries
//@[
//  s(1).type = '()'  s(1).subs = {5}
//  s(2).type = '.'   s(2).subs = 'foo'
//  s(3).type = '{}'  s(3).subs = ':'
//@]
//!

//!
//@Module SUBSINDEX Overloaded Class Indexing
//@@Section CLASS
//@@Usage
//This method is called for classes in the expressions
//of the form
//@[
//  c = subsindex(a)
//@]
//where @|a| is an object, and @|c| is an index vector.
//It is also called for
//@[
//  c = b(a)
//@]
//in which case @|subsindex(a)| must return a vector containing
//integers between @|0| and @|N-1| where @|N| is the number
//of elements in the vector @|b|.
//!

ArrayVector ClassFunction(int nargout, const ArrayVector& arg,
			  Interpreter* eval) {
  if (arg.size() == 0)
    throw Exception("class function requires at least one argument");
  if (arg.size() == 1) 
    return singleArrayVector(ClassOneArgFunction(arg[0]));
  ArrayVector parents;
  rvstring parentNames;
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

std::vector<int> MarkUserClasses(ArrayVector t) {
  std::vector<int> set;
  for (int j=0;j<t.size();j++) 
    if (t[j].isUserClass()) set.push_back(j);
  return set;
}
 
bool ClassSearchOverload(Interpreter* eval, ArrayVector t, 
			 std::vector<int> userset, FuncPtr &val,
			 std::string name) {
  bool overload = false;
  int k = 0;
  while (k<userset.size() && !overload) {
    overload = eval->getContext()->lookupFunction(ClassMangleName(t[userset[k]].getClassName().back(),name),val);
    if (!overload) k++;
  }
  if (!overload)
    throw Exception(std::string("Unable to find overloaded '") + name + "' for user defined classes");
}

Array ClassMatrixConstructor(ArrayMatrix m, Interpreter* eval) {
  // Process the rows...
  // If a row has no user defined classes, then
  // use the standard matrixConstructor
  ArrayVector rows;
  for (int i=0;i<m.size();i++) {
    // check this row - make a list of columns that are
    // user classes
    std::vector<int> userset(MarkUserClasses(m[i]));
    if (userset.empty()) {
      ArrayMatrix n;
      n.push_back(m[i]);
      rows.push_back(Array::matrixConstructor(n));
    } else {
      FuncPtr val;
      bool horzcat_overload = ClassSearchOverload(eval,m[i],userset,
						  val,"horzcat");
      // scan through the list of user defined classes - look
      // for one that has "horzcat" overloaded
      val->updateCode();
      ArrayVector p;
      p = val->evaluateFunction(eval,m[i],1);
      if (!p.empty())
	rows.push_back(p[0]);
      else {
	eval->warningMessage("'horzcat' called for user defined class and it returned nothing.  Substituting empty array for result.");
	rows.push_back(Array::emptyConstructor());
      }
    }
  }
  // Check for a singleton - handle with special case
  if (rows.size() == 1)
    return rows[0];
  // At this point we have a vector arrays that have to vertically
  // concatenated.  There may not be any objects in it, so we have 
  // to rescan.
  std::vector<int> userset(MarkUserClasses(rows));
  if (userset.empty()) {
    // OK - we don't have any user-defined classes anymore,
    // so we want to call matrix constructor, which needs
    // an ArrayMatrix instead of an ArrayVector.
    ArrayMatrix ref;
    for (int i=0;i<rows.size();i++)
      ref.push_back(singleArrayVector(rows[i]));
    return Array::matrixConstructor(ref);
  } else {
    // We do have a user defined class - look for a vertcat defined
    FuncPtr val;
    bool vertcat_overload = ClassSearchOverload(eval,rows,userset,
						val,"vertcat");
    val->updateCode();
    ArrayVector p;
    p = val->evaluateFunction(eval,rows,1);
    if (!p.empty())
      return p[0];
    else
      return Array::emptyConstructor();
  }
  return Array::emptyConstructor();
}

Array ClassUnaryOperator(Array a, std::string funcname,
			 Interpreter* eval) {
  FuncPtr val;
  ArrayVector m, n;
  if (eval->getContext()->lookupFunction(ClassMangleName(a.getClassName().back(),funcname),val)) {
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

bool ClassResolveFunction(Interpreter* eval, Array& args, std::string funcName, FuncPtr& val) {
  Context *context = eval->getContext();
  // First try to resolve to a method of the base class
  if (context->lookupFunction(ClassMangleName(args.getClassName().back(),funcName),val)) {
    return true;
  } 
  UserClass eclass(eval->lookupUserClass(args.getClassName().back()));
  rvstring parentClasses(eclass.getParentClasses());
  // Now check the parent classes
  for (int i=0;i<parentClasses.size();i++) {
    if (context->lookupFunction(ClassMangleName(parentClasses.at(i),funcName),val)) {
      rvstring argClass(args.getClassName());
      argClass.push_back(parentClasses.at(i));
      args.setClassName(argClass);
      return true;
    }
  }
  // Nothing matched, return
  return false;
}

void printClassNames(Array a) {
  rvstring classname(a.getClassName());
  for (int i=0;i<classname.size();i++)
    std::cout << "class " << classname.at(i) << "\r\n";
}


// TODO - add "inferiorto", etc and class precedence

Array ClassBiOp(Array a, Array b, FuncPtr val, Interpreter *eval) {
  val->updateCode();
  ArrayVector m, n;
  m.push_back(a); m.push_back(b);
  n = val->evaluateFunction(eval,m,1);
  if (!n.empty())
    return n[0];
  else
    return Array::emptyConstructor();
}

Array ClassTriOp(Array a, Array b, Array c, FuncPtr val, Interpreter *eval) {
  val->updateCode();
  ArrayVector m, n;
  m.push_back(a); m.push_back(b); m.push_back(c);
  n = val->evaluateFunction(eval,m,1);
  if (!n.empty())
    return n[0];
  else
    return Array::emptyConstructor();
}

Array ClassTrinaryOperator(Array a, Array b, Array c, std::string funcname,
			   Interpreter* eval) {
  FuncPtr val;
  if (a.isUserClass()) {
    if (eval->getContext()->lookupFunction(ClassMangleName(a.getClassName().back(),funcname),val)) 
      return ClassTriOp(a,b,c,val,eval);
    throw Exception("Unable to find a definition of " + funcname + " for arguments of class " + a.getClassName().back());
  } else if (b.isUserClass()) {
    if (eval->getContext()->lookupFunction(ClassMangleName(b.getClassName().back(),funcname),val)) 
      return ClassTriOp(a,b,c,val,eval);
    throw Exception("Unable to find a definition of " + funcname + " for arguments of class " + b.getClassName().back());
  } else if (c.isUserClass()) {
    if (eval->getContext()->lookupFunction(ClassMangleName(c.getClassName().back(),funcname),val)) 
      return ClassTriOp(a,b,c,val,eval);
    throw Exception("Unable to find a definition of " + funcname + " for arguments of class " + b.getClassName().back());
  }
}

Array ClassBinaryOperator(Array a, Array b, std::string funcname,
			  Interpreter* eval) {
  FuncPtr val;
  if (a.isUserClass()) {
    if (eval->getContext()->lookupFunction(ClassMangleName(a.getClassName().back(),funcname),val)) 
      return ClassBiOp(a,b,val,eval);
    throw Exception("Unable to find a definition of " + funcname + " for arguments of class " + a.getClassName().back());
  } else if (b.isUserClass()) {
    if (eval->getContext()->lookupFunction(ClassMangleName(b.getClassName().back(),funcname),val)) 
      return ClassBiOp(a,b,val,eval);
    throw Exception("Unable to find a definition of " + funcname + " for arguments of class " + b.getClassName().back());
  }
}

void AdjustColonCalls(ArrayVector& m, treeVector t) {
  for (unsigned index=0;index < t.size();index++) 
    if (t[index].is(':'))
      m[index] = Array::stringConstructor(":");
}

Array IndexExpressionToStruct(Interpreter* eval, treeVector t, Array r) {
#error FIXME
//   ArrayVector struct_args, m;
//   ArrayVector rv;
//   Array rsave(r);
//   rvstring fNames;
//   fNames.push_back("type");
//   fNames.push_back("subs");
//   for (unsigned index=0;index < t.size();index++) {
//     if (!rv.empty()) 
//       throw Exception("Cannot reindex an expression that returns multiple values.");
//     if (t[index].is(TOK_PARENS)) {
//       m = eval->varExpressionList(t[index].children(),r);
//       // Scan through the expressions... adjust for "colon" calls
//       AdjustColonCalls(m,t[index].children());
//       if (m.size() == 0) 
// 	throw Exception("Expected indexing expression!");
//       // Take the arguments and push them into a cell array...
//       ArrayMatrix q;	q.push_back(m);
//       struct_args.push_back(Array::stringConstructor("()"));
//       struct_args.push_back(Array::cellConstructor(q));
//     }
//     if (t[index].is(TOK_BRACES)) {
//       m = eval->varExpressionList(t[index].children(),r);
//       AdjustColonCalls(m,t[index].children());
//       if (m.size() == 0) 
// 	throw Exception("Expected indexing expression!");
//       // Take the arguments and push them into a cell array...
//       ArrayMatrix q;	q.push_back(m);
//       struct_args.push_back(Array::stringConstructor("{}"));
//       struct_args.push_back(Array::cellConstructor(q));
//     }
//     if (t[index].is('.')) {
//       struct_args.push_back(Array::stringConstructor("."));
//       struct_args.push_back(Array::stringConstructor(t[index].first().text()));
//     }
//   }
//   int cnt = struct_args.size()/2;
//   Array *cp = (Array*) Array::allocateArray(FM_STRUCT_ARRAY,cnt,fNames);
//   for (int i=0;i<2*cnt;i++) 
//     cp[i] = struct_args[i];
//   return Array(FM_STRUCT_ARRAY,Dimensions(cnt,1),cp,false,fNames);
}
  
ArrayVector ClassSubsrefCall(Interpreter* eval, treeVector t, Array r, FuncPtr val) {
#error FIXME
  ArrayVector p;
  p.push_back(r);
  p.push_back(IndexExpressionToStruct(eval,t, r));
  val->updateCode();
  ArrayVector n = val->evaluateFunction(eval,p,1);
  return n;
}

// What is special here...  Need to be able to do field indexing
// 
ArrayVector ClassRHSExpression(Array r, treeVector t, Interpreter* eval) {
#error FIXME
//  tree s;
//  Array q;
//  Array n, p;
//  ArrayVector m;
//  int peerCnt;
//  int dims;
//  bool isVar;
//  bool isFun;
//  FuncPtr val;
//    
//  // Try and look up subsref, _unless_ we are already in a method 
//  // of this class
//  if (!eval->inMethodCall(r.getClassName().back()))
//    if (ClassResolveFunction(eval,r,"subsref",val)) {
//      // Overloaded subsref case
//      return ClassSubsrefCall(eval,t,r,val);
//    }
//  ArrayVector rv;
//  for (unsigned index=0;index < t.size();index++) {
//    if (!rv.empty()) 
//      throw Exception("Cannot reindex an expression that returns multiple values.");
//    if (t[index].is(TOK_PARENS)) {
//      m = eval->varExpressionList(t[index].children(),r);
//      if (m.size() == 0) 
//	throw Exception("Expected indexing expression!");
//      else if (m.size() == 1) {
//	q = r.getVectorSubset(m[0]);
//	r = q;
//      } else {
//	q = r.getNDimSubset(m);
//	r = q;
//      }
//    }
//    if (t[index].is(TOK_BRACES)) {
//      m = eval->varExpressionList(t[index].children(),r);
//      if (m.size() == 0) 
//	throw Exception("Expected indexing expression!");
//      else if (m.size() == 1)
//	rv = r.getVectorContentsAsList(m[0]);
//      else
//	rv = r.getNDimContentsAsList(m);
//      if (rv.size() == 1) {
//	r = rv[0];
//	rv = ArrayVector();
//      } else if (rv.size() == 0) {
//	throw Exception("Empty expression!");
//	r = Array::emptyConstructor();
//      }
//    }
//    if (t[index].is('.')) {
//      // This is where the classname chain comes into being.
//      rvstring className(r.getClassName());
//      for (int i=1;i<className.size();i++) {
//	rv = r.getFieldAsList(className.at(i));
//	r = rv[0];
//      }
//      rv = r.getFieldAsList(t[index].first().text());
//      if (rv.size() <= 1) {
//	r = rv[0];
//	rv = ArrayVector();
//      }
//    }
//    if (t[index].is(TOK_DYN)) {
//      char *field;
//      try {
//	Array fname(eval->expression(t[index].first()));
//	field = fname.getContentsAsCString();
//      } catch (Exception &e) {
//	throw Exception("dynamic field reference to structure requires a string argument");
//      }
//      rv = r.getFieldAsList(field);
//      if (rv.size() <= 1) {
//	r = rv[0];
//	rv = ArrayVector();
//      }      
//    }
//  }
//  if (rv.empty())
//    rv.push_back(r);
//  return rv;
}

void ClassAssignExpression(Array *dst, tree t, const Array& value, Interpreter* eval) {
#error FIXME
  FuncPtr val;
  if (!ClassResolveFunction(eval,*dst,"subsasgn",val))
    throw Exception("The method 'subsasgn' is not defined for objects of class " + 
		    dst->getClassName().back());
  ArrayVector p;
  p.push_back(*dst);
  p.push_back(IndexExpressionToStruct(eval,t.children(), *dst));
  p.push_back(value);
  val->updateCode();
  bool overload(eval->getStopOverload());
  eval->setStopOverload(true);
  ArrayVector n = val->evaluateFunction(eval,p,1);
  eval->setStopOverload(overload);
  if (!n.empty())
    *dst = n[0];
  else
    eval->warningMessage(std::string("'subsasgn' for class ") + dst->getClassName().back() + std::string(" did not return a value... operation has no effect."));
}

// Ideally, this would be the only place where the class name is mangled.
// However, currently, the same op is repeated in the Interface implementation
// code.
std::string ClassMangleName(std::string className, std::string funcName) {
  return "@" + className + ":" + funcName;
}

