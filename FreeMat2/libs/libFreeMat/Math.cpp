// Copyright (c) 2002, 2003 Samit Basu
// 
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.

#include "Array.hpp"
#include "Exception.hpp"
#include "Math.hpp"
#include "VectorOps.hpp"
#include "MatrixMultiply.hpp"
#include "LinearEqSolver.hpp"
#include "LeastSquaresSolver.hpp"
#include "EigenDecompose.hpp"
#include "Malloc.hpp"

namespace FreeMat {

  /**
   * This is the generic function interface into calculations
   * that can be performed on some type.
   */
  typedef void (*vvfun) (const int length, 
			 void* result,
			 const void* arg1,
			 const int stride1,
			 const void* arg2,
			 const int stride2);

  /**
   * This structure holds pointers to functions when
   * all types can be handled.
   */

  typedef struct {
    bool  promoteInt32ToDouble;
    vvfun int32func;
    vvfun floatfunc;
    vvfun doublefunc;
    vvfun complexfunc;
    vvfun dcomplexfunc;
  } packVectorVector;

  static packVectorVector addFuncs = {false,
				      (vvfun) int32Add,
				      (vvfun) floatAdd,
				      (vvfun) doubleAdd,
				      (vvfun) complexAdd,
				      (vvfun) dcomplexAdd};

  static packVectorVector subtractFuncs = {false,
					   (vvfun) int32Subtract,
					   (vvfun) floatSubtract,
					   (vvfun) doubleSubtract,
					   (vvfun) complexSubtract,
					   (vvfun) dcomplexSubtract};

  static packVectorVector dotMultiplyFuncs = {false,
					      (vvfun) int32DotMultiply,
					      (vvfun) floatDotMultiply,
					      (vvfun) doubleDotMultiply,
					      (vvfun) complexDotMultiply,
					      (vvfun) dcomplexDotMultiply};

  static packVectorVector dotRightDivideFuncs = {true,
						 (vvfun) int32DotRightDivide,
						 (vvfun) floatDotRightDivide,
						 (vvfun) doubleDotRightDivide,
						 (vvfun) complexDotRightDivide,
						 (vvfun) dcomplexDotRightDivide};

  static packVectorVector dotLeftDivideFuncs = {true,
						(vvfun) int32DotLeftDivide,
						(vvfun) floatDotLeftDivide,
						(vvfun) doubleDotLeftDivide,
						(vvfun) complexDotLeftDivide,
						(vvfun) dcomplexDotLeftDivide};

  static packVectorVector lessThanFuncs = {false,
					   (vvfun) int32LessThan,
					   (vvfun) floatLessThan,
					   (vvfun) doubleLessThan,
					   (vvfun) complexLessThan,
					   (vvfun) dcomplexLessThan};

  static packVectorVector lessEqualsFuncs = {false,
					     (vvfun) int32LessEquals,
					     (vvfun) floatLessEquals,
					     (vvfun) doubleLessEquals,
					     (vvfun) complexLessEquals,
					     (vvfun) dcomplexLessEquals};

  static packVectorVector greaterThanFuncs = {false,
					      (vvfun) int32GreaterThan,
					      (vvfun) floatGreaterThan,
					      (vvfun) doubleGreaterThan,
					      (vvfun) complexGreaterThan,
					      (vvfun) dcomplexGreaterThan};

  static packVectorVector greaterEqualsFuncs = {false,
						(vvfun) int32GreaterEquals,
						(vvfun) floatGreaterEquals,
						(vvfun) doubleGreaterEquals,
						(vvfun) complexGreaterEquals,
						(vvfun) dcomplexGreaterEquals};

  static packVectorVector equalsFuncs = {false,
					 (vvfun) int32Equals,
					 (vvfun) floatEquals,
					 (vvfun) doubleEquals,
					 (vvfun) complexEquals,
					 (vvfun) dcomplexEquals};

  static packVectorVector notEqualsFuncs = {false,
					    (vvfun) int32NotEquals,
					    (vvfun) floatNotEquals,
					    (vvfun) doubleNotEquals,
					    (vvfun) complexNotEquals,
					    (vvfun) dcomplexNotEquals};

  inline vvfun mapTypeToFunction(packVectorVector F, Class clss) {
    switch (clss) {
    case FM_INT32:
      return F.int32func;
    case FM_FLOAT:
      return F.floatfunc;
    case FM_DOUBLE:
      return F.doublefunc;
    case FM_COMPLEX:
      return F.complexfunc;
    case FM_DCOMPLEX:
      return F.dcomplexfunc;
    default:
      throw Exception("Illegal type mapped to function type.");
    }
  }

  /**
   * Check that both of the argument objects are numeric.
   */
  inline void CheckNumeric(Array &A, Array &B) throw(Exception){
    bool Anumeric, Bnumeric;

    Anumeric = !A.isReferenceType();
    Bnumeric = !B.isReferenceType();
    if (!(Anumeric && Bnumeric))
      throw Exception("Cannot apply numeric operations to reference types.");
  }

  /**
   * Check that both of the argument objects are of the
   * same type.  If not, apply conversion.  Here are the
   * rules:
   *   Float operations produce float results.
   *   Double operations produce double results.
   *   Mixtures of float and double operations produce double results.
   *   Integer operations (of any kind) produce int results - 
   *     the question is what type do these objects get promoted too?  The
   *     answer is a signed int (32 bits).
   *   Mixtures of real and complex operations produce complex results.
   *   Integer constants are 32 bit by default
   *   Float constants are 64 bit by default
   *   Division operations lead to promotion 
   *   Character types are automatically (demoted) to 32 bit integers.
   *
   *   The way to accomplish this is as follows: compute the larger
   *   of the types of A and B.  Call this type t_max.  If t_max
   *   is not an integer type, promote both types to this type.
   *   If t_max is an integer type, promote both objects to an FM_INT32.
   *   If this is a division operation or a matrix operation, promote both 
   *    objects to an FM_DOUBLE64!
   *
   */
  inline void TypeCheck(Array &A, Array &B, bool isDivOrMatrix) {
    Class Aclass, Bclass, Cclass;

    Aclass = A.getDataClass();
    Bclass = B.getDataClass();
  
    if (Aclass == FM_STRING) Aclass = FM_INT32;
    if (Bclass == FM_STRING) Bclass = FM_INT32;

    if ((Aclass == FM_COMPLEX && Bclass == FM_DOUBLE) ||
	(Bclass == FM_COMPLEX && Aclass == FM_DOUBLE))
      Cclass = FM_DCOMPLEX;
    else
      Cclass = (Aclass > Bclass) ? Aclass : Bclass;

    if (Cclass >= FM_FLOAT) {
      A.promoteType(Cclass);
      B.promoteType(Cclass);
    } else if (!isDivOrMatrix) {
      A.promoteType(FM_INT32);
      B.promoteType(FM_INT32);
    } else {
      A.promoteType(FM_DOUBLE);
      B.promoteType(FM_DOUBLE);
    }
  }

  /**
   * We want to perform a matrix-matrix operation between two data objects.
   * The following checks are required:
   *  1. If A or B is a scalar, then return false - this is really a 
   *     vector operation, and the arguments should be passed to a 
   *     vector checker (like VectorCheck).
   *  2. Both A & B must be numeric
   *  3. Both A & B must be the same type (if not, the lesser type is
   *     automatically promoted).
   *  4. Both A & B must be 2-Dimensional.
   *  5. A & B must be conformant, i.e. the number of columns in A must
   *     match the number of rows in B.
   */
  inline bool MatrixCheck(Array &A, Array &B) throw(Exception){
    // Test for either a scalar (test 1)
    if (A.isScalar() || B.isScalar())
      return false;

    // Test for A & B numeric
    CheckNumeric(A,B);

    // Test for 2D
    if (!A.is2D() || !B.is2D()) 
      throw Exception("Cannot apply matrix operations to N-Dimensional arrays.");
  
    // Test the types
    TypeCheck(A,B,true);
    return true;
  }
 
  /*
   * Check to see if two dimensions (when treated as vectors) are equivalent in size.
   */
  bool SameSizeCheck(Dimensions Adim, Dimensions Bdim) {
    Adim.simplify();
    Bdim.simplify();
    return (Adim.equals(Bdim));
  }

  /**
   * We want to perform a vector operation between two data objects.
   * The following checks are required:
   *  1. Both A & B must be numeric
   *  2. Either A & B are the same size or
   *      A is a scalar or B is a scalar.
   */
  inline void VectorCheck(Array& A, Array& B, bool promote) throw(Exception){
    stringVector dummySV;

    // Check for numeric types
    CheckNumeric(A,B);
  
    if (!(SameSizeCheck(A.getDimensions(),B.getDimensions()) || A.isScalar() || B.isScalar()))
      throw Exception("Size mismatch on arguments to arithmetic operator.");
  
    // Test the types.
    TypeCheck(A,B,promote);
  }

  /**
   * We want to perform a vector operator between two logical data objects.
   * The following operations are performed:
   *  1. Both A & B are converted to logical types.
   *  2. Either A & B must be the same size, or A is a
   *     scalar or B is a scalar.
   */
  inline void BoolVectorCheck(Array& A, Array& B) throw(Exception){
    A.promoteType(FM_LOGICAL);
    B.promoteType(FM_LOGICAL);

    if (!(SameSizeCheck(A.getDimensions(),B.getDimensions()) || A.isScalar() || B.isScalar()))
      throw Exception("Size mismatch on arguments.");
  }

  /**
   * This is for vector-vector operations that are closed under their type.
   */
  inline Array DoGenericTwoArgFunction(Array A, Array B, 
				packVectorVector F) {
    vvfun exec;
  
    // Process the two arguments through the type check and dimension checks...
    VectorCheck(A,B,F.promoteInt32ToDouble);
    // Get a pointer to the function we ultimately need to execute
    exec = mapTypeToFunction(F,A.getDataClass());
    if (A.isScalar()) {
      int Blen(B.getLength());
      void *Cp = Malloc(Blen*B.getElementSize());
      exec(Blen,Cp,A.getDataPointer(),0,B.getDataPointer(),1);
      return Array(B.getDataClass(),B.getDimensions(),Cp);
    } else if (B.isScalar()) {
      int Alen(A.getLength());
      void *Cp = Malloc(Alen*A.getElementSize());
      exec(Alen,Cp,A.getDataPointer(),1,B.getDataPointer(),0);
      return Array(B.getDataClass(),A.getDimensions(),Cp);
    } else {
      int Alen(A.getLength());
      void *Cp = Malloc(Alen*A.getElementSize());
      exec(Alen,Cp,A.getDataPointer(),1,B.getDataPointer(),1);
      return Array(A.getDataClass(),A.getDimensions(),Cp);
    }
    throw Exception("Fatal error: fell through DoGenericTwoArgFunction cases");
  }


  /**
   * Handle a particular type case in the power operator.
   */
  inline Array doPowerAssist(Array A, Class AClass, 
		      Array B, Class BClass,
		      Class CClass, vvfun exec) {
    Array C;
    A.promoteType(AClass);
    B.promoteType(BClass);
    if (A.isScalar()) {
      int Blen(B.getLength());
      C = Array(CClass,B.getDimensions(),NULL);
      void *Cp = Malloc(Blen*C.getElementSize());
      exec(Blen,Cp,A.getDataPointer(),0,B.getDataPointer(),1);
      C.setDataPointer(Cp);
    } else if (B.isScalar()) {
      int Alen(A.getLength());
      C = Array(CClass,A.getDimensions(),NULL);
      void *Cp = Malloc(Alen*C.getElementSize());
      exec(Alen,Cp,A.getDataPointer(),1,B.getDataPointer(),0);
      C.setDataPointer(Cp);
    } else {
      int Alen(A.getLength());
      C = Array(CClass,A.getDimensions(),NULL);
      void *Cp = Malloc(Alen*C.getElementSize());
      exec(Alen,Cp,A.getDataPointer(),1,B.getDataPointer(),1);
      C.setDataPointer(Cp);
    }
    return C;
  }

  /**
   * The power function is a special case of the vector-vector operations.  
   * In this case, if the real vector contains any negative elements
   * A real vector can be written as (r*e^(j\theta))^p = r^p*e^(j*\theta*p)
   * If there are any components of A negative raised to a non-integer power,
   * then A and B are promoted to complex types.  If A is a double, then they
   * are promoted to dcomplex types.  So the rules are as follows:
   * A complex, B arbitrary -> output is complex, unless B is double or dcomplex
   * A dcomplex, B arbitrary -> output is dcomplex
   * A double, arbitrary, B integer -> output is double
   * A double, negative, B noninteger -> output is dcomplex
   * A float, arbitrary, B integer -> output is float
   * A float, negative, B noninteger -> output is complex, unless B is double or dcomplex
   *
   * Some simplification can take place - the following type combinations
   * are possible - with the unique combinations numbered
   * 1. complex, integer   -> complex
   * 2. complex, float     -> complex
   * 3. complex, double    >> dcomplex, double   -> dcomplex
   * 4. complex, complex   -> complex
   * 5. complex, dcomplex  >> dcomplex, dcomplex -> dcomplex
   * 6. dcomplex, integer  -> dcomplex
   *    dcomplex, float    >> dcomplex, double   -> dcomplex (3)
   *    dcomplex, double   >> dcomplex, double   -> dcomplex (3)
   *    dcomplex, complex  >> dcomplex, dcomplex -> dcomplex (5)
   *    dcomplex, dcomplex >> dcomplex, dcomplex -> dcomplex (5)
   * 7. double, integer    -> double
   *    double-, float     >> dcomplex, dcomplex -> dcomplex (5)
   *    double-, double    >> dcomplex, dcomplex -> dcomplex (5)
   *    double-, complex   >> dcomplex, dcomplex -> dcomplex (5)
   *    double-, dcomplex  >> dcomplex, dcomplex -> dcomplex (5)
   * 8. double+, float     >> double, double -> double
   *    double+, double    >> double, double -> double (8)
   *    double+, complex   >> dcomplex, dcomplex -> dcomplex (5)
   *    double+, dcomplex  >> dcomplex, dcomplex -> dcomplex (5)
   * 9. float, integer     -> float
   *    float-, float      >> complex, complex -> complex (4)
   *    float-, double     >> dcomplex, dcomplex -> dcomplex (5)
   *    float-, complex    >> complex, complex -> complex (4)
   *    float-, dcomplex   >> dcomplex, dcomplex -> dcomplex (5)
   *10. float+, float      >> float, float -> float
   *    float+, double     >> double, double -> double (8)
   *    float+, complex    >> complex, complex -> complex (4)
   *    float+, dcomplex   >> dcomplex, dcomplex -> dcomplex (5)
   *
   * This type scheme is complicated, but workable.  If A is an 
   * integer type, it is first promoted to double, before the
   * rules are applied.  Then, we note that the rules can be cleaved
   * along the lines of B being integer or not.  If B is integer,
   * we apply the appropriate version of 1., 6., 7., 9..  This
   * covers 4 of the 6 cases.  Next, we determine if A is complex.
   * If A is complex, we check if B is double or dcomplex. If either
   * is the case, we promote A to dcomplex.  If A is dcomplex,
   * we promote B to double or dcomplex.  Next, we test for
   * rules 2., 3., 4., 5.  
   */

#define OPCASE(t,o) case t: opType = o; break;
#define MAPOP(o,a,b,c,f) case o: return doPowerAssist(A,a,B,b,c,f);
  inline Array DoPowerTwoArgFunction(Array A, Array B) throw(Exception){
    Array C;
    double *Cp;
    bool Anegative;
    bool Bnoninteger;
    bool complexResult;
    stringVector dummySV;
    Class AClass, BClass;
    int opType;

    CheckNumeric(A,B);
    if (!(SameSizeCheck(A.getDimensions(),B.getDimensions()) || A.isScalar() || B.isScalar()))
      throw Exception("Size mismatch on arguments to arithmetic operator.");
    // If A is not at least a float type, promote it to double
    AClass = A.getDataClass();
    BClass = B.getDataClass();
    if (AClass < FM_FLOAT) AClass = FM_DOUBLE;
    if (BClass < FM_INT32) BClass = FM_INT32;
    // Get a read on if A is positive
    Anegative = !(A.isPositive());
    // Check through the different type cases...
    opType = 0;
    if (AClass == FM_COMPLEX) {
      switch (BClass) {
	OPCASE(FM_INT32,1);
	OPCASE(FM_FLOAT,2);
	OPCASE(FM_DOUBLE,3);
	OPCASE(FM_COMPLEX,4);
	OPCASE(FM_DCOMPLEX,5);
      }
    } else if (AClass == FM_DCOMPLEX) {
      switch(BClass) {
	OPCASE(FM_INT32,6);
	OPCASE(FM_FLOAT,3);
	OPCASE(FM_DOUBLE,3);
	OPCASE(FM_COMPLEX,5);
	OPCASE(FM_DCOMPLEX,5);
      }
    } else if (AClass == FM_DOUBLE && Anegative) {
      switch(BClass) {
	OPCASE(FM_INT32,7);
	OPCASE(FM_FLOAT,5);
	OPCASE(FM_DOUBLE,5);
	OPCASE(FM_COMPLEX,5);
	OPCASE(FM_DCOMPLEX,5);
      }      
    } else if (AClass == FM_DOUBLE && (!Anegative)){
      switch(BClass) {
	OPCASE(FM_INT32,7);
	OPCASE(FM_FLOAT,8);
	OPCASE(FM_DOUBLE,8);
	OPCASE(FM_COMPLEX,5);
	OPCASE(FM_DCOMPLEX,5);
      }      
    } else if (AClass == FM_FLOAT && Anegative) {
      switch(BClass) {
	OPCASE(FM_INT32,9);
	OPCASE(FM_FLOAT,4);
	OPCASE(FM_DOUBLE,5);
	OPCASE(FM_COMPLEX,4);
	OPCASE(FM_DCOMPLEX,5);
      }      
    } else if (AClass == FM_FLOAT && (!Anegative)){
      switch(BClass) {
	OPCASE(FM_INT32,9);
	OPCASE(FM_FLOAT,10);
	OPCASE(FM_DOUBLE,8);
	OPCASE(FM_COMPLEX,4);
	OPCASE(FM_DCOMPLEX,5);
      }
    }
    // Invoke the appropriate case
    switch(opType) {
      MAPOP(1,FM_COMPLEX,FM_INT32,FM_COMPLEX,cicPower);
      MAPOP(2,FM_COMPLEX,FM_FLOAT,FM_COMPLEX,cfcPower);
      MAPOP(3,FM_DCOMPLEX,FM_DOUBLE,FM_DCOMPLEX,zdzPower);
      MAPOP(4,FM_COMPLEX,FM_COMPLEX,FM_COMPLEX,cccPower);
      MAPOP(5,FM_DCOMPLEX,FM_DCOMPLEX,FM_DCOMPLEX,zzzPower);
      MAPOP(6,FM_DCOMPLEX,FM_INT32,FM_DCOMPLEX,zizPower);
      MAPOP(7,FM_DOUBLE,FM_INT32,FM_DOUBLE,didPower);
      MAPOP(8,FM_DOUBLE,FM_DOUBLE,FM_DOUBLE,dddPower);
      MAPOP(9,FM_FLOAT,FM_INT32,FM_FLOAT,fifPower);
      MAPOP(10,FM_FLOAT,FM_FLOAT,FM_FLOAT,fffPower);
    }
  }

  /**
   * For all of the Vector-Vector operations that have vector and scalar versions,
   * the general behavior is the same.  We require 6 functions to handle the 6
   * cases that generally arise:
   *   real    vector vector
   *   real    vector scalar
   *   real    scalar vector
   *   complex vector vector
   *   complex vector scalar
   *   complex scalar vector
   */
  inline Array DoComparativeTwoArgFunction(Array A, Array B, packVectorVector F) {
    Array C;
    vvfun exec;

    // Process the two arguments through the type check and dimension checks...
    VectorCheck(A,B,F.promoteInt32ToDouble);
    // Get a pointer to the function we ultimately need to execute
    exec = mapTypeToFunction(F,A.getDataClass());
    if (A.isScalar()) {
      int Blen(B.getLength());
      C = Array(FM_LOGICAL,B.getDimensions(),NULL);
      void *Cp = Malloc(Blen*C.getElementSize());
      exec(Blen,Cp,A.getDataPointer(),0,B.getDataPointer(),1);
      C.setDataPointer(Cp);
    } else if (B.isScalar()) {
      int Alen(A.getLength());
      C = Array(FM_LOGICAL,A.getDimensions(),NULL);
      void *Cp = Malloc(Alen*C.getElementSize());
      exec(Alen,Cp,A.getDataPointer(),1,B.getDataPointer(),0);
      C.setDataPointer(Cp);
    } else {
      int Alen(A.getLength());
      C = Array(FM_LOGICAL,A.getDimensions(),NULL);
      void *Cp = Malloc(Alen*C.getElementSize());
      exec(Alen,Cp,A.getDataPointer(),1,B.getDataPointer(),1);
      C.setDataPointer(Cp);
    }
    return C;
  }

  /**
   * For all of the Vector-Vector operations that have vector and scalar versions,
   * the general behavior is the same.  We require 3 functions to handle the 3
   * cases that generally arise:
   *   logical vector vector
   *   logical vector scalar
   *   logical scalar vector
   * The remaining 3 function placeholders in the packVectorVector are unused.
   */
  inline Array DoBoolTwoArgFunction(Array A, Array B, vvfun exec) {
    Array C;
  
    BoolVectorCheck(A,B);
    if (A.isScalar()) {
      int Blen(B.getLength());
      C = Array(FM_LOGICAL,B.getDimensions(),NULL);
      void *Cp = Malloc(Blen*C.getElementSize());
      exec(Blen,Cp,A.getDataPointer(),0,B.getDataPointer(),1);
      C.setDataPointer(Cp);
    } else if (B.isScalar()) {
      int Alen(A.getLength());
      C = Array(FM_LOGICAL,A.getDimensions(),NULL);
      void *Cp = Malloc(Alen*C.getElementSize());
      exec(Alen,Cp,A.getDataPointer(),1,B.getDataPointer(),0);
      C.setDataPointer(Cp);
    } else {
      int Alen(A.getLength());
      C = Array(FM_LOGICAL,A.getDimensions(),NULL);
      void *Cp = Malloc(Alen*C.getElementSize());
      exec(Alen,Cp,A.getDataPointer(),1,B.getDataPointer(),1);
      C.setDataPointer(Cp);
    }
    return C;
  }

  /**
   * Add two objects.
   */
  //!
  //@Module PLUS Addition Operator
  //@@Usage
  //Adds two numerical arrays (elementwise) together.  There are two forms
  //for its use, both with the same general syntax:
  //@[
  //  y = a + b
  //@]
  //where @|a| and @|b| are @|n|-dimensional arrays of numerical type.  In the
  //first case, the two arguments are the same size, in which case, the 
  //output @|y| is the same size as the inputs, and is the element-wise the sum 
  //of @|a| and @|b|.  In the second case, either @|a| or @|b| is a scalar, 
  //in which case @|y| is the same size as the larger argument,
  //and is the sum of the scalar added to each element of the other argument.
  //
  //The type of @|y| depends on the types of @|a| and @|b| using the type 
  //promotion rules.  The types are ordered as:
  //\begin{enumerate}
  //\item @|uint8| - unsigned, 8-bit integers range @|[0,255]|
  //\item @|int8| - signed, 8-bit integers @|[-127,128]|
  //\item @|uint16| - unsigned, 16-bit integers @|[0,65535]|
  //\item @|int16| - signed, 16-bit integers @|[-32768,32767]|
  //\item @|uint32| - unsigned, 32-bit integers @|[0,4294967295]|
  //\item @|int32| - signed, 32-bit integers @|[-2147483648,2147483647]|
  //\item @|float| - 32-bit floating point
  //\item @|double| - 64-bit floating point
  //\item @|complex| - 32-bit complex floating point
  //\item @|dcomplex| - 64-bit complex floating point
  //\end{enumerate}
  //Note that the type promotion and combination rules work similar to 
  //@|C|.  Numerical overflow rules are also the same as @|C|.
  //@@Function Internals
  //There are three formulae for the addition operator, depending on the
  //sizes of the three arguments.  In the most general case, in which 
  //the two arguments are the same size, the output is computed via:
  //\[
  //y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d) + b(m_1,\ldots,m_d)
  //\]
  //If @|a| is a scalar, then the output is computed via
  //\[
  //y(m_1,\ldots,m_d) = a + b(m_1,\ldots,m_d).
  //\]
  //On the other hand, if @|b| is a scalar, then the output is computed via
  //\[
  //y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d) + b.
  //\]
  //@@Examples
  //Here are some examples of using the addition operator.  First, a 
  //straight-forward usage of the plus operator.  The first example
  //is straightforward - the @|int32| is the default type used for
  //integer constants (same as in @|C|), hence the output is the
  //same type:
  //@<
  //3 + 8
  //@>
  //Next, we use the floating point syntax to force one of the arguments
  //to be a @|double|, which results in the output being @|double|:
  //@<
  //3.1 + 2
  //@>
  //Note that if one of the arguments is complex-valued, the output will be
  //complex also.
  //@<
  //a = 3 + 4*i
  //b = a + 2.0f
  //@>
  //If a @|complex| value is added to a @|double|, the result is 
  //promoted to @|dcomplex|.
  //@<
  //b = a + 2.0
  //@>
  //We can also demonstrate the three forms of the addition operator.  First
  //the element-wise version:
  //@<
  //a = [1,2;3,4]
  //b = [2,3;6,7]
  //c = a + b
  //@>
  //Then the scalar versions
  //@<
  //c = a + 1
  //c = 1 + b
  //@>
  //!
  Array Add(const Array& A, const Array& B) {
    return(DoGenericTwoArgFunction(A,B,addFuncs));
  }

  /**
   * Subtract two objects.
   */
  //!
  //@Module MINUS Subtraction Operator
  //@@Usage
  //Subtracts two numerical arrays (elementwise).  There are two forms
  //for its use, both with the same general syntax:
  //@[
  //  y = a - b
  //@]
  //where @|a| and @|b| are @|n|-dimensional arrays of numerical type.  In the
  //first case, the two arguments are the same size, in which case, the 
  //output @|y| is the same size as the inputs, and is the element-wise
  //difference of @|a| and @|b|.  In the second case, either @|a| or @|b| is a scalar, 
  //in which case @|y| is the same size as the larger argument,
  //and is the difference of the scalar to each element of the other argument.
  //
  //The type of @|y| depends on the types of @|a| and @|b| using the type 
  //promotion rules.  The types are ordered as:
  //\begin{enumerate}
  //\item @|uint8| - unsigned, 8-bit integers range @|[0,255]|
  //\item @|int8| - signed, 8-bit integers @|[-127,128]|
  //\item @|uint16| - unsigned, 16-bit integers @|[0,65535]|
  //\item @|int16| - signed, 16-bit integers @|[-32768,32767]|
  //\item @|uint32| - unsigned, 32-bit integers @|[0,4294967295]|
  //\item @|int32| - signed, 32-bit integers @|[-2147483648,2147483647]|
  //\item @|float| - 32-bit floating point
  //\item @|double| - 64-bit floating point
  //\item @|complex| - 32-bit complex floating point
  //\item @|dcomplex| - 64-bit complex floating point
  //\end{enumerate}
  //Note that the type promotion and combination rules work similar to 
  //@|C|.  Numerical overflow rules are also the same as @|C|.
  //@@Function Internals
  //There are three formulae for the subtraction operator, depending on the
  //sizes of the three arguments.  In the most general case, in which 
  //the two arguments are the same size, the output is computed via:
  //\[
  //y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d) - b(m_1,\ldots,m_d)
  //\]
  //If @|a| is a scalar, then the output is computed via
  //\[
  //y(m_1,\ldots,m_d) = a - b(m_1,\ldots,m_d).
  //\]
  //On the other hand, if @|b| is a scalar, then the output is computed via
  //\[
  //y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d) - b.
  //\]
  //@@Examples
  //Here are some examples of using the subtraction operator.  First, a 
  //straight-forward usage of the minus operator.  The first example
  //is straightforward - the @|int32| is the default type used for
  //integer constants (same as in @|C|), hence the output is the
  //same type:
  //@<
  //3 - 8
  //@>
  //Next, we use the floating point syntax to force one of the arguments
  //to be a @|double|, which results in the output being @|double|:
  //@<
  //3.1 - 2
  //@>
  //Note that if one of the arguments is complex-valued, the output will be
  //complex also.
  //@<
  //a = 3 + 4*i
  //b = a - 2.0f
  //@>
  //If a @|double| value is subtracted from a @|complex|, the result is 
  //promoted to @|dcomplex|.
  //@<
  //b = a - 2.0
  //@>
  //We can also demonstrate the three forms of the subtraction operator.  First
  //the element-wise version:
  //@<
  //a = [1,2;3,4]
  //b = [2,3;6,7]
  //c = a - b
  //@>
  //Then the scalar versions
  //@<
  //c = a - 1
  //c = 1 - b
  //@>
  //!
  Array Subtract(const Array& A, const Array& B) {
    return(DoGenericTwoArgFunction(A,B,subtractFuncs));
  }

  /**
   * Element-wise multiplication.
   */
  //!
  //@Module DOTTIMES Element-wise Multiplication Operator
  //@@Usage
  //Multiplies two numerical arrays (elementwise).  There are two forms
  //for its use, both with the same general syntax:
  //@[
  //  y = a .* b
  //@]
  //where @|a| and @|b| are @|n|-dimensional arrays of numerical type.  In the
  //first case, the two arguments are the same size, in which case, the 
  //output @|y| is the same size as the inputs, and is the element-wise
  //product of @|a| and @|b|.  In the second case, either @|a| or @|b| is a scalar, 
  //in which case @|y| is the same size as the larger argument,
  //and is the product of the scalar with each element of the other argument.
  //
  //The type of @|y| depends on the types of @|a| and @|b| using type 
  //promotion rules. All of the types are preserved under multiplication except
  // for integer types, which are promoted to @|int32| prior to 
  //multiplication (same as @|C|).
  //@@Function Internals
  //There are three formulae for the dot-times operator, depending on the
  //sizes of the three arguments.  In the most general case, in which 
  //the two arguments are the same size, the output is computed via:
  //\[
  //y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d) \times b(m_1,\ldots,m_d)
  //\]
  //If @|a| is a scalar, then the output is computed via
  //\[
  //y(m_1,\ldots,m_d) = a \times b(m_1,\ldots,m_d).
  //\]
  //On the other hand, if @|b| is a scalar, then the output is computed via
  //\[
  //y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d) \times b.
  //\]
  //@@Examples
  //Here are some examples of using the dottimes operator.  First, a 
  //straight-forward usage of the @|.*| operator.  The first example
  //is straightforward:
  //@<
  //3 .* 8
  //@>
  //Note, however, that because of the way that input is parsed, eliminating
  //the spaces @|3.*8| results in the input being parsed as @|3. * 8|,
  //which yields a @|double| result:
  //@<
  //3.*8
  //@>
  //This is really an invokation of the @|times| operator.
  //
  //Next, we use the floating point syntax to force one of the arguments
  //to be a @|double|, which results in the output being @|double|:
  //@<
  //3.1 .* 2
  //@>
  //Note that if one of the arguments is complex-valued, the output will be
  //complex also.
  //@<
  //a = 3 + 4*i
  //b = a .* 2.0f
  //@>
  //If a @|complex| value is multiplied by a @|double|, the result is 
  //promoted to @|dcomplex|.
  //@<
  //b = a .* 2.0
  //@>
  //We can also demonstrate the three forms of the dottimes operator.  First
  //the element-wise version:
  //@<
  //a = [1,2;3,4]
  //b = [2,3;6,7]
  //c = a .* b
  //@>
  //Then the scalar versions
  //@<
  //c = a .* 3
  //c = 3 .* a
  //@>
  //!
  Array DotMultiply(Array A, Array B) {
    return(DoGenericTwoArgFunction(A,B,dotMultiplyFuncs));
  }

  /**
   * Element-wise right divide.
   */
  //!
  //@Module DOTRIGHTDIVIDE Element-wise Right-Division Operator
  //@@Usage
  //Divides two numerical arrays (elementwise).  There are two forms
  //for its use, both with the same general syntax:
  //@[
  //  y = a ./ b
  //@]
  //where @|a| and @|b| are @|n|-dimensional arrays of numerical type.  In the
  //first case, the two arguments are the same size, in which case, the 
  //output @|y| is the same size as the inputs, and is the element-wise
  //division of @|b| by @|a|.  In the second case, either @|a| or @|b| is a scalar, 
  //in which case @|y| is the same size as the larger argument,
  //and is the division of the scalar with each element of the other argument.
  //
  //The type of @|y| depends on the types of @|a| and @|b| using type 
  //promotion rules, with one important exception: unlike @|C|, integer
  //types are promoted to @|double| prior to division.
  //@@Function Internals
  //There are three formulae for the dot-right-divide operator, depending on the
  //sizes of the three arguments.  In the most general case, in which 
  //the two arguments are the same size, the output is computed via:
  //\[
  //y(m_1,\ldots,m_d) = \frac{a(m_1,\ldots,m_d)}{b(m_1,\ldots,m_d)}
  //\]
  //If @|a| is a scalar, then the output is computed via
  //\[
  //y(m_1,\ldots,m_d) = \frac{a}{b(m_1,\ldots,m_d)}
  //\]
  //On the other hand, if @|b| is a scalar, then the output is computed via
  //\[
  //y(m_1,\ldots,m_d) = \frac{a(m_1,\ldots,m_d)}{b}.
  //\]
  //@@Examples
  //Here are some examples of using the dot-right-divide operator.  First, a 
  //straight-forward usage of the @|./| operator.  The first example
  //is straightforward:
  //@<
  //3 ./ 8
  //@>
  //Note that this is not the same as evaluating @|3/8| in @|C| - there,
  //the output would be @|0|, the result of the integer division.
  //
  //We can also divide complex arguments:
  //@<
  //a = 3 + 4*i
  //b = 5 + 8*i
  //c = a ./ b
  //@>
  //If a @|complex| value is divided by a @|double|, the result is 
  //promoted to @|dcomplex|.
  //@<
  //b = a ./ 2.0
  //@>
  //We can also demonstrate the three forms of the dot-right-divide operator.  First
  //the element-wise version:
  //@<
  //a = [1,2;3,4]
  //b = [2,3;6,7]
  //c = a ./ b
  //@>
  //Then the scalar versions
  //@<
  //c = a ./ 3
  //c = 3 ./ a
  //@>
  //!
  Array DotRightDivide(Array A, Array B) {
    return(DoGenericTwoArgFunction(A,B,dotRightDivideFuncs));
  }

  /**
   * Element-wise left divide.
   */
  //!
  //@Module DOTLEFTDIVIDE Element-wise Left-Division Operator
  //@@Usage
  //Divides two numerical arrays (elementwise) - gets its name from the 
  //fact that the divisor is on the left.  There are two forms
  //for its use, both with the same general syntax:
  //@[
  //  y = a .\ b
  //@]
  //where @|a| and @|b| are @|n|-dimensional arrays of numerical type.  In the
  //first case, the two arguments are the same size, in which case, the 
  //output @|y| is the same size as the inputs, and is the element-wise
  //division of @|b| by @|a|.  In the second case, either @|a| or @|b| is a scalar, 
  //in which case @|y| is the same size as the larger argument,
  //and is the division of the scalar with each element of the other argument.
  //
  //The type of @|y| depends on the types of @|a| and @|b| using type 
  //promotion rules, with one important exception: unlike @|C|, integer
  //types are promoted to @|double| prior to division.
  //@@Function Internals
  //There are three formulae for the dot-left-divide operator, depending on the
  //sizes of the three arguments.  In the most general case, in which 
  //the two arguments are the same size, the output is computed via:
  //\[
  //y(m_1,\ldots,m_d) = \frac{b(m_1,\ldots,m_d)}{a(m_1,\ldots,m_d)}
  //\]
  //If @|a| is a scalar, then the output is computed via
  //\[
  //y(m_1,\ldots,m_d) = \frac{b(m_1,\ldots,m_d)}{a}
  //\]
  //On the other hand, if @|b| is a scalar, then the output is computed via
  //\[
  //y(m_1,\ldots,m_d) = \frac{b}{a(m_1,\ldots,m_d)}.
  //\]
  //@@Examples
  //Here are some examples of using the dot-left-divide operator.  First, a 
  //straight-forward usage of the @|.\\| operator.  The first example
  //is straightforward:
  //@<
  //3 .\ 8
  //@>
  //Note that this is not the same as evaluating @|8/3| in @|C| - there,
  //the output would be @|2|, the result of the integer division.
  //
  //We can also divide complex arguments:
  //@<
  //a = 3 + 4*i
  //b = 5 + 8*i
  //c = b .\ a
  //@>
  //If a @|complex| value is divided by a @|double|, the result is 
  //promoted to @|dcomplex|.
  //@<
  //b = a .\ 2.0
  //@>
  //We can also demonstrate the three forms of the dot-left-divide operator.  First
  //the element-wise version:
  //@<
  //a = [1,2;3,4]
  //b = [2,3;6,7]
  //c = a .\ b
  //@>
  //Then the scalar versions
  //@<
  //c = a .\ 3
  //c = 3 .\ a
  //@>
  //!
  Array DotLeftDivide(Array A, Array B) {
    return(DoGenericTwoArgFunction(A,B,dotLeftDivideFuncs));
  }

  /**
   * Element-wise power.
   */
  //!
  //@Module DOTPOWER Element-wise Power Operator
  //@@Usage
  //Raises one numerical array to another array (elementwise).  There are three operators all with the same general syntax:
  //@[
  //  y = a .^ b
  //@]
  //The result @|y| depends on which of the following three situations applies to the arguments @|a| and @|b|:
  //\begin{enumerate}
  //  \item @|a| is a scalar, @|b| is an arbitrary @|n|-dimensional numerical array, in which case the output is @|a| raised to the power of each element of @|b|, and the output is the same size as @|b|.
  //  \item @|a| is an @|n|-dimensional numerical array, and @|b| is a scalar, then the output is the same size as @|a|, and is defined by each element of @|a| raised to the power @|b|.
  //  \item @|a| and @|b| are both @|n|-dimensional numerical arrays of \emph{the same size}.  In this case, each element of the output is the corresponding element of @|a| raised to the power defined by the corresponding element of @|b|.
  //\end{enumerate}
  //The output follows the standard type promotion rules, although types are not generally preserved under the power operation.  In particular, integers are automatically converted to @|double| type, and negative numbers raised to fractional powers can return complex values.
  //@@Function Internals
  //There are three formulae for this operator.  For the first form
  //\[
  //y(m_1,\ldots,m_d) = a^{b(m_1,\ldots,m_d)},
  //\]
  //and the second form
  //\[
  //y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d)^b,
  //\]
  //and in the third form
  //\[
  //y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d)^{b(m_1,\ldots,m_d)}.
  //\]
  //@@Examples
  //We demonstrate the three forms of the dot-power operator using some simple examples.  First, the case of a scalar raised to a series of values.
  //@<
  //a = 2
  //b = 1:4
  //c = a.^b
  //@>
  //The second case shows a vector raised to a scalar.
  //@<
  //c = b.^a
  //@>
  //The third case shows the most general use of the dot-power operator.
  //@<
  //A = [1,2;3,2]
  //B = [2,1.5;0.5,0.6]
  //C = A.^B
  //@>
  //!
  Array DotPower(Array A, Array B) {
    return(DoPowerTwoArgFunction(A,B));
  }

  /**
   * Element-wise less than.
   */
  //!
  //@Module COMPARISONOPS Array Comparison Operators
  //@@Usage
  //There are a total of six comparison operators available in FreeMat, all of which are binary operators with the following syntax
  //@[
  //  y = a < b
  //  y = a <= b
  //  y = a > b
  //  y = a >= b
  //  y = a ~= b
  //  y = a == b
  //@]
  //where @|a| and @|b| are numerical arrays or scalars, and @|y| is a @|logical| array of the appropriate size.  Each of the operators has three modes of operation, summarized in the following list:
  //\begin{enumerate}
  //  \item @|a| is a scalar, @|b| is an n-dimensional array - the output is then the same size as @|b|, and contains the result of comparing each element in @|b| to the scalar @|a|.
  //  \item @|a| is an n-dimensional array, @|b| is a scalar - the output is the same size as @|a|, and contains the result of comparing each element in @|a| to the scalar @|b|.
  //  \item @|a| and @|b| are both n-dimensional arrays of the same size - the output is then the same size as both @|a| and @|b|, and contains the result of an element-wise comparison between @|a| and @|b|.
  //\end{enumerate}
  //The operators behave the same way as in @|C|, with unequal types meing promoted using the standard type promotion rules prior to comparisons.  The only difference is that in FreeMat, the not-equals operator is @|~=| instead of @|!=|.
  //@@Examples
  //Some simple examples of comparison operations.  First a comparison with a scalar:
  //@<
  //a = randn(1,5)
  //a>0
  //@>
  //Next, we construct two vectors, and test for equality:
  //@<
  //a = [1,2,5,7,3]
  //b = [2,2,5,9,4]
  //c = a == b
  //@>
  //!
  Array LessThan(Array A, Array B) {
    return(DoComparativeTwoArgFunction(A,B,lessThanFuncs));
  }

  /**
   * Element-wise less equals.
   */
  Array LessEquals(Array A, Array B) {
    return(DoComparativeTwoArgFunction(A,B,lessEqualsFuncs));
  }

  /**
   * Element-wise greater than.
   */
  Array GreaterThan(Array A, Array B) {
    return(DoComparativeTwoArgFunction(A,B,greaterThanFuncs));
  }

  /**
   * Element-wise greater equals.
   */
  Array GreaterEquals(Array A, Array B) {
    return(DoComparativeTwoArgFunction(A,B,greaterEqualsFuncs));
  }

  /**
   * Element-wise equals.
   */
  Array Equals(Array A, Array B) {
    return(DoComparativeTwoArgFunction(A,B,equalsFuncs));
  }

  /**
   * Element-wise notEquals.
   */
  Array NotEquals(Array A, Array B) {
    return(DoComparativeTwoArgFunction(A,B,notEqualsFuncs));
  }

  /**
   * Element-wise and
   */
  //!
  //@Module LOGICALOPS Logical Array Operators
  //@@Usage
  //There are three Boolean operators available in FreeMat.  The syntax for their use is:
  //@[
  //  y = ~x
  //  y = a & b
  //  y = a | b
  //@]
  //where @|x|, @|a| and @|b| are @|logical| arrays.  The operators are
  //\begin{itemize}
  //\item NOT (@|~|) - output @|y| is true if the corresponding element of @|x| is false, and ouput @|y| is false if the corresponding element of @|x| is true.
  //\item OR (@|||) - output @|y| is true if corresponding element of @|a| is true or if corresponding element of @|b| is true (or if both are true).
  //\item AND (@|\&|) - output @|y| is true only if both the corresponding elements of @|a| and @|b| are both true.
  //\end{itemize}
  //The binary operators AND and OR can take scalar arguments as well as vector arguments, in which case, the scalar is operated on with each element of the vector.
  //@@Examples
  //Some simple examples of logical operators.  Suppose we want to calculate the exclusive-or (XOR) of two vectors of logical variables.  First, we create a pair of vectors to perform the XOR operation on:
  //@<
  //a = (randn(1,6)>0)
  //b = (randn(1,6)>0)
  //@>
  //Next, we can compute the OR of @|a| and @|b|:
  //@<
  //c = a | b
  //@>
  //However, the XOR and OR operations differ on the fifth entry - the XOR would be false, since it is true if and only if exactly one of the two inputs is true.  To isolate this case, we can AND the two vectors, to find exactly those entries that appear as true in both @|a| and @|b|:
  //@<
  //d = a & b
  //@>
  //At this point, we can modify the contents of @|c| in two ways -- the Boolean way is to AND @|\sim d| with @|c|, like so
  //@<
  //xor = c & (~d)
  //@>
  //The other way to do this is simply force @|c(d) = 0|, which uses the logical indexing mode of FreeMat (see the chapter on indexing for more details).  This, however, will cause @|c| to become an @|int32| type, as opposed to a logical type.
  //@<
  //c(d) = 0
  //@>
  //!
  Array And(Array A, Array B) {
    return(DoBoolTwoArgFunction(A,B,(vvfun)boolAnd));
  }

  /**
   * Element-wise or
   */
  Array Or(Array A, Array B) {
    return(DoBoolTwoArgFunction(A,B,(vvfun)boolOr));
  }

  /**
   * Element-wise not
   */
  Array Not(Array A) {
    Array C;

    A.promoteType(FM_LOGICAL);
    C = Array(FM_LOGICAL,A.getDimensions(),NULL);
    void *Cp = Malloc(A.getLength()*C.getElementSize());
    boolNot(A.getLength(),(logical*)Cp,(const logical*) A.getDataPointer());
    C.setDataPointer(Cp);
    return C;
  }

  /**
   * Element-wise negate - this one is a custom job, so to speak.
   * 
   */
  Array Negate(Array A) throw(Exception){
    Array C;
    Class Aclass;

    if (A.isReferenceType())
      throw Exception("Cannot negate non-numeric types.");
    Aclass = A.getDataClass();
  
    if (Aclass == FM_STRING)
      Aclass = FM_INT32;
    else if (Aclass < FM_INT32)
      Aclass = FM_INT32;

    A.promoteType(Aclass);

    C = Array(Aclass,A.getDimensions(),NULL);
    void *Cp = Malloc(A.getLength()*C.getElementSize());
    switch (Aclass) {
    case FM_INT32:
      int32Negate(A.getLength(),(int32*)Cp,(int32*)A.getDataPointer());
      break;
    case FM_FLOAT:
      floatNegate(A.getLength(),(float*)Cp,(float*)A.getDataPointer());
      break;
    case FM_DOUBLE:
      doubleNegate(A.getLength(),(double*)Cp,(double*)A.getDataPointer());
      break;
    case FM_COMPLEX:
      complexNegate(A.getLength(),(float*)Cp,(float*)A.getDataPointer());
      break;
    case FM_DCOMPLEX:
      dcomplexNegate(A.getLength(),(double*)Cp,(double*)A.getDataPointer());
      break;
    }
    C.setDataPointer(Cp);
    return C;
  }


  /**
   * Matrix-matrix multiply
   */
  //!
  //@Module TIMES Matrix Multiply Operator
  //@@Usage
  //Multiplies two numerical arrays.  This operator is really a combination
  //of three operators, all of which have the same general syntax:
  //@[
  //  y = a * b
  //@]
  //where @|a| and @|b| are arrays of numerical type.  The result @|y| depends
  //on which of the following three situations applies to the arguments
  //@|a| and @|b|:
  //\begin{enumerate}
  //  \item @|a| is a scalar, @|b| is an arbitrary @|n|-dimensional numerical array, in which case the output is the element-wise product of @|b| with the scalar @|a|.
  //  \item @|b| is a scalar, @|a| is an arbitrary @|n|-dimensional numerical array, in which case the output is the element-wise product of @|a| with the scalar @|b|.
  //  \item @|a,b| are conformant matrices, i.e., @|a| is of size @|M x K|, and @|b| is of size @|K x N|, in which case the output is of size @|M x N| and is the matrix product of @|a|, and @|b|.
  //\end{enumerate}
  //The output follows the standard type promotion rules, although in the first two cases, if @|a| and @|b| are integers, the output is an integer also, while in the third case if @|a| and @|b| are integers, ,the output is of type @|double|.
  //@@Function Internals
  //There are three formulae for the times operator.  For the first form
  //\[
  //y(m_1,\ldots,m_d) = a \times b(m_1,\ldots,m_d),
  //\]
  //and the second form
  //\[
  //y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d) \times b.
  //\]
  //In the third form, the output is the matrix product of the arguments
  //\[
  //y(m,n) = \sum_{k=1}^{K} a(m,k) b(k,n)
  //\]
  //@@Examples
  //Here are some examples of using the matrix multiplication operator.  First,
  //the scalar examples (types 1 and 2 from the list above):
  //@<
  //a = [1,3,4;0,2,1]
  //b = a * 2
  //@>
  //The matrix form, where the first argument is @|2 x 3|, and the
  //second argument is @|3 x 1|, so that the product is size 
  //@|2 x 1|.
  //@<
  //a = [1,2,0;4,2,3]
  //b = [5;3;1]
  //c = a*b
  //@>
  //Note that the output is double precision.
  //!
  Array Multiply(Array A, Array B) throw(Exception){
    // Process our arguments
    if (!MatrixCheck(A,B))
      // Its really a vector product, pass...
      return DotMultiply(A,B);
  
    // Test for conformancy
    if (A.getDimensionLength(1) != B.getDimensionLength(0)) 
      throw Exception("Requested matrix multiplication requires arguments to be conformant.");

    int Arows, Acols;
    int Brows, Bcols;
  
    Arows = A.getDimensionLength(0);
    Acols = A.getDimensionLength(1);
    Bcols = B.getDimensionLength(1);
  
    // Its really a matrix-matrix operation, and the arguments are
    // satisfactory.  Check for the type.
    void *Cp = Malloc(Arows*Bcols*A.getElementSize());
  
    if (A.getDataClass() == FM_FLOAT)
      floatMatrixMatrixMultiply(Arows,Bcols,Acols,(float*)Cp,
				(const float*)A.getDataPointer(),
				(const float*)B.getDataPointer());
    else if (A.getDataClass() == FM_DOUBLE)
      doubleMatrixMatrixMultiply(Arows,Bcols,Acols,(double*)Cp,
				 (const double*)A.getDataPointer(),
				 (const double*)B.getDataPointer());
    else if (A.getDataClass() == FM_COMPLEX)
      complexMatrixMatrixMultiply(Arows,Bcols,Acols,(float*)Cp,
				  (const float*)A.getDataPointer(),
				  (const float*)B.getDataPointer());
    else if (A.getDataClass() == FM_DCOMPLEX)
      dcomplexMatrixMatrixMultiply(Arows,Bcols,Acols,(double*)Cp,
				   (const double*)A.getDataPointer(),
				   (const double*)B.getDataPointer());
    Dimensions outDim(2);
    outDim[0] = Arows;
    outDim[1] = Bcols;
    return Array(A.getDataClass(),outDim,Cp);
  }
    
  /**
   * Matrix-matrix divide
   */
  //!
  //@Module LEFTDIVIDE Matrix Equation Solver/Divide Operator
  //@@Usage
  //The divide operator @|\| is really a combination of three
  //operators, all of which have the same general syntax:
  //@[
  //  Y = A \ B
  //@]
  //where @|A| and @|B| are arrays of numerical type.  The result @|Y| depends
  //on which of the following three situations applies to the arguments
  //@|A| and @|B|:
  //\begin{enumerate}
  //  \item @|A| is a scalar, @|B| is an arbitrary @|n|-dimensional numerical array, in which case the output is each element of @|B| divided by the scalar @|A|.
  //  \item @|B| is a scalar, @|A| is an arbitrary @|n|-dimensional numerical array, in which case the output is the scalar @|B| divided by each element of @|A|.
  //  \item @|A,B| are matrices with the same number of rows, i.e., @|A| is of size @|M x K|, and @|B| is of size @|M x L|, in which case the output is of size @|K x L|.
  //\end{enumerate}
  //The output follows the standard type promotion rules, although in the first two cases, if @|A| and @|B| are integers, the output is an integer also, while in the third case if @|A| and @|B| are integers, the output is of type @|double|.
  //
  //A few additional words about the third version, in which @|A| and @|B| are matrices.  Very loosely speaking, @|Y| is the matrix that satisfies @|A * Y = B|.  In cases where such a matrix exists.  If such a matrix does not exist, then a matrix @|Y| is returned that approximates @|A * Y \approx B|.
  //@@Function Internals
  //There are three formulae for the times operator.  For the first form
  //\[
  //Y(m_1,\ldots,m_d) = \frac{B(m_1,\ldots,m_d)}{A},
  //\]
  //and the second form
  //\[
  //Y(m_1,\ldots,m_d) = \frac{B}{A(m_1,\ldots,m_d)}.
  //\]
  //In the third form, the calculation of the output depends on the size of @|A|. Because each column of @|B| is treated independantly, we can rewrite the equation @|A Y = B| as
  //\[
  //  A [y_1, y_2,\ldots, y_l] = [b_1, b_2, \ldots, b_l]
  //\]
  //where @|y_i| are the columns of @|Y|, and @|b_i| are the columns of the matrix @|B|. If @|A| is a square matrix, then the LAPACK routine @|*gesvx| (where the @|*| is replaced with @|sdcz| depending on the type of the arguments) is used, which uses an LU decomposition of @|A| to solve the sequence of equations sequentially.  If @|A| is singular, then a warning is emitted. 
  //
  //On the other hand, if @|A| is rectangular, then the LAPACK routine @|*gelsy| is used.  Note that these routines are designed to work with matrices @|A| that are full rank - either full column rank or full row rank.  If @|A| fails to satisfy this assumption, a warning is emitted.  If @|A| has full column rank (and thus necessarily has more rows than columns), then theoretically, this operator finds the columns @|y_i| that satisfy:
  //\[
  //  y_i = \arg \min_y \| A y - b_i \|_2
  //\]
  //and each column is thus the Least Squares solution of @|A y = b_i|.  On the other hand, if @|A| has full row rank (and thus necessarily has more columns than rows), then theoretically, this operator finds the columns @|y_i| that satisfy
  //\[
  //  y_i = \arg \min_{A y = b_i} \| y \|_2
  //\]
  //and each column is thus the Minimum Norm vector @|y_i| that satisfies @|A y_i = b_i|.  
  //In the event that the matrix @|A| is neither full row rank nor full column rank, a solution is returned, that is the minimum norm least squares solution.  The solution is computed using an orthogonal factorization technique that is documented in the LAPACK User's Guide (see the References section for details).
  //@@Examples
  //Here are some simple examples of the divide operator.  We start with a simple example of a full rank, square matrix:
  //@<
  //A = [1,1;0,1]
  //@>
  //Suppose we wish to solve
  //\[
  //  \begin{bmatrix} 1 & 1 \\ 0 & 1 \end{bmatrix}
  //  \begin{bmatrix} y_1 \\ y_2 \end{bmatrix}
  // = 
  //  \begin{bmatrix} 3 \\ 2 \end{bmatrix}
  //\]
  //(which by inspection has the solution @|y_1 = 1|, @|y_2 = 2|).  Thus we compute:
  //@<
  //B = [3;2]
  //Y = A\B
  //@>
  //
  //Suppose we wish to solve a trivial Least Squares (LS) problem.  We want to find a simple scaling of the vector @|[1;1]| that is closest to the point @|[2,1]|.  This is equivalent to solving
  //\[
  //\begin{bmatrix} 1 \\ 1 \end{bmatrix} y = \begin{bmatrix} 2 \\ 1 \end{bmatrix}
  //\]
  //in a least squares sense.  For fun, we can calculate the solution using calculus by hand.  The error we wish to minimize is
  //\[
  //  \varepsilon(y) = (y - 2)^2 + (y-1)^2.
  //\]
  //Taking a derivative with respect to @|y|, and setting to zero (which we must have for an extrema when @|y| is unconstrained)
  //\[
  //  2 (y-2) + 2 (y-1) = 0
  //\]
  //which we can simplify to @|4y = 6| or @|y = 3/2| (we must, technically, check to make sure this is a minimum, and not a maximum or an inflection point).  Here is the same calculation performed using FreeMat:
  //@<
  //A = [1;1]
  //B = [2;1]
  //A\B
  //@>
  //which is the same solution.
  //!
  Array LeftDivide(Array A, Array B) throw(Exception) {
    stringVector dummySV;
    // Process our arguments
    if (!MatrixCheck(A,B))
      // Its really a vector product, pass...
      return DotLeftDivide(A,B);
  
    // Test for conformancy
    if (A.getDimensionLength(0) != B.getDimensionLength(0)) 
      throw Exception("Requested divide operation requires arguments to have correct dimensions.");

    int Arows, Acols;
    int Brows, Bcols;
  
    Arows = A.getDimensionLength(0);
    Acols = A.getDimensionLength(1);
    Brows = B.getDimensionLength(0);
    Bcols = B.getDimensionLength(1);
  
    // Its really a matrix-matrix operation, and the arguments are
    // satisfactory.  Check for the type.
    void *Cp;
    Dimensions outDim(2);
    if (Arows == Acols) {
      // Square matrix case - A is N x N, B is N x K - use 
      // linear equation solver.  Output is N x K.
      Cp = Malloc(Arows*Bcols*A.getElementSize());
      if (A.getDataClass() == FM_FLOAT)
	floatSolveLinEq(Array::getArrayIOInterface(),
			Arows,Bcols,(float*)Cp,
			(float*)A.getReadWriteDataPointer(),
			(float*)B.getReadWriteDataPointer());
      else if (A.getDataClass() == FM_COMPLEX)
	complexSolveLinEq(Array::getArrayIOInterface(),
			  Arows,Bcols,(float*)Cp,
			  (float*)A.getReadWriteDataPointer(),
			  (float*)B.getReadWriteDataPointer());
      else if (A.getDataClass() == FM_DOUBLE)
	doubleSolveLinEq(Array::getArrayIOInterface(),
			 Arows,Bcols,(double*)Cp,
			 (double*)A.getReadWriteDataPointer(),
			 (double*)B.getReadWriteDataPointer());
      else if (A.getDataClass() == FM_DCOMPLEX)
	dcomplexSolveLinEq(Array::getArrayIOInterface(),
			   Arows,Bcols,(double*)Cp,
			   (double*)A.getReadWriteDataPointer(),
			   (double*)B.getReadWriteDataPointer());
      outDim[0] = Arows;
      outDim[1] = Bcols;
    } else {
      // Rectangular matrix case - A is M x N, B must be M x K - use
      // lease squares equation solver.  Output is N x K.
      Cp = Malloc(Acols*Bcols*A.getElementSize());
      if (A.getDataClass() == FM_FLOAT)
	floatSolveLeastSq(Array::getArrayIOInterface(),
			  Arows,Acols,Bcols,(float*)Cp,
			  (float*)A.getReadWriteDataPointer(),
			  (float*)B.getReadWriteDataPointer());
      else if (A.getDataClass() == FM_COMPLEX)
	complexSolveLeastSq(Array::getArrayIOInterface(),
			    Arows,Acols,Bcols,(float*)Cp,
			    (float*)A.getReadWriteDataPointer(),
			    (float*)B.getReadWriteDataPointer());
      else if (A.getDataClass() == FM_DOUBLE)
	doubleSolveLeastSq(Array::getArrayIOInterface(),
			   Arows,Acols,Bcols,(double*)Cp,
			   (double*)A.getReadWriteDataPointer(),
			   (double*)B.getReadWriteDataPointer());
      else if (A.getDataClass() == FM_DCOMPLEX)
	dcomplexSolveLeastSq(Array::getArrayIOInterface(),
			     Arows,Acols,Bcols,(double*)Cp,
			     (double*)A.getReadWriteDataPointer(),
			     (double*)B.getReadWriteDataPointer());
      outDim[0] = Acols;
      outDim[1] = Bcols;
    }
    return Array(A.getDataClass(),outDim,Cp);
  }
    
  /**
   * Matrix-matrix divide
   */
  //!
  //@Module RIGHTDIVIDE Matrix Equation Solver/Divide Operator
  //@@Usage
  //The divide operator @|/| is really a combination of three
  //operators, all of which have the same general syntax:
  //@[
  //  Y = A / B
  //@]
  //where @|A| and @|B| are arrays of numerical type.  The result @|Y| depends
  //on which of the following three situations applies to the arguments
  //@|A| and @|B|:
  //\begin{enumerate}
  //  \item @|A| is a scalar, @|B| is an arbitrary @|n|-dimensional numerical array, in which case the output is the scalar @|A| divided into each element of @|B|.
  //  \item @|B| is a scalar, @|A| is an arbitrary @|n|-dimensional numerical array, in which case the output is each element of @|A| divided by the scalar @|B|.
  //  \item @|A,B| are matrices with the same number of columns, i.e., @|A| is of size @|K x M|, and @|B| is of size @|L x M|, in which case the output is of size @|K x L|.
  //\end{enumerate}
  //The output follows the standard type promotion rules, although in the first two cases, if @|A| and @|B| are integers, the output is an integer also, while in the third case if @|A| and @|B| are integers, the output is of type @|double|.
  //
  //@@Function Internals
  //There are three formulae for the times operator.  For the first form
  //\[
  //Y(m_1,\ldots,m_d) = \frac{A}{B(m_1,\ldots,m_d)},
  //\]
  //and the second form
  //\[
  //Y(m_1,\ldots,m_d) = \frac{A(m_1,\ldots,m_d)}{B}.
  //\]
  //In the third form, the output is defined as:
  //\[
  //  Y = (B' \backslash A')'
  //\]
  //and is used in the equation @|Y B = A|.
  //@@Examples
  //The right-divide operator is much less frequently used than the left-divide operator, but the concepts are similar.  It can be used to find least-squares and minimum norm solutions.  It can also be used to solve systems of equations in much the same way.  Here's a simple example:
  //@<
  //B = [1,1;0,1];
  //A = [4,5]
  //A/B
  //@>
  //!
  Array RightDivide(Array A, Array B) {
    Array C;

    // Process our arguments
    if (!MatrixCheck(A,B))
      // Its really a vector product, pass...
      return DotRightDivide(A,B);

    A.transpose();
    B.transpose();

    C = LeftDivide(B,A);
    C.transpose();

    return C;
  }

  /**
   * Eigen decomposition, symmetric matrix, compact decomposition case
   */
  void EigenDecomposeCompactSymmetric(Array A, Array& D) {
    Class Aclass;
    printf("ED-CS\r\n");

    // Test for numeric
    if (A.isReferenceType())
      throw Exception("Cannot apply eigendecomposition to reference types.");
  
    if (!A.is2D())
      throw Exception("Cannot apply matrix operations to N-Dimensional arrays.");

    if (A.getDimensionLength(0) != A.getDimensionLength(1))
      throw Exception("Cannot eigendecompose a non-square matrix.");

    int N = A.getDimensionLength(0);

    // Create one square matrix to store the eigenvectors
    Dimensions Vdims(2);
    Vdims[0] = N;
    Vdims[1] = 1;

    // Handle the type of A - if it is an integer type, then promote to double
    Aclass = A.getDataClass();
    if (Aclass < FM_FLOAT) {
      A.promoteType(FM_DOUBLE);
      Aclass = FM_DOUBLE;
    }

    // Select the eigenvector decomposition routine based on A's type
    Dimensions VDims(2);
    VDims[0] = N;
    VDims[1] = 1;
    switch (Aclass) {
    case FM_FLOAT: 
      {
	// A temporary vector to store the eigenvalues
	float *eigenvals = (float*) Malloc(N*sizeof(float));
	floatEigenDecomposeSymmetric(N, NULL, eigenvals, 
				     (float*)A.getReadWriteDataPointer(),
				     false);
	// Copy the eigenvalues into a diagonal (float) matrix
	D = Array(FM_FLOAT,Vdims,eigenvals);
      }
      break;
    case FM_DOUBLE: 
      {
	// A temporary vector to store the eigenvalues
	double *eigenvals = (double*) Malloc(N*sizeof(double));
	doubleEigenDecomposeSymmetric(N, NULL, eigenvals, 
				      (double*)A.getReadWriteDataPointer(),
				     false);
	// Copy the eigenvalues into a diagonal (double) matrix
	D = Array(FM_DOUBLE,Vdims,eigenvals);
      }
      break;
    case FM_COMPLEX:
      {
	float *eigenvals = (float*) Malloc(N*sizeof(float));
	complexEigenDecomposeSymmetric(N, NULL, eigenvals, 
				       (float*)A.getReadWriteDataPointer(),
				       false);
	// Copy the eigenvalues into a diagonal (complex) matrix
	D = Array(FM_FLOAT,Vdims,eigenvals);
      }
      break;
    case FM_DCOMPLEX:
      {
	double *eigenvals = (double*) Malloc(N*sizeof(double));
	dcomplexEigenDecomposeSymmetric(N, NULL, eigenvals, 
					(double*)A.getReadWriteDataPointer(),
					false);
	// Copy the eigenvalues into a diagonaal (complex) matrix
	D = Array(FM_DOUBLE,Vdims,eigenvals);
      }
      break;
    }
  }

  /**
   * Eigen decomposition, symmetric matrix, full decomposition case
   */
  void EigenDecomposeFullSymmetric(Array A, Array& V, Array& D) {
    int i;
    Class Aclass;

    printf("ED-FS\r\n");
    // Test for numeric
    if (A.isReferenceType())
      throw Exception("Cannot apply eigendecomposition to reference types.");
  
    if (!A.is2D())
      throw Exception("Cannot apply matrix operations to N-Dimensional arrays.");

    if (A.getDimensionLength(0) != A.getDimensionLength(1))
      throw Exception("Cannot eigendecompose a non-square matrix.");

    int N = A.getDimensionLength(0);

    // Create one square matrix to store the eigenvectors
    Dimensions Vdims(2);
    Vdims[0] = N;
    Vdims[1] = N;

    // Handle the type of A - if it is an integer type, then promote to double
    Aclass = A.getDataClass();
    if (Aclass < FM_FLOAT) {
      A.promoteType(FM_DOUBLE);
      Aclass = FM_DOUBLE;
    }

    // Select the eigenvector decomposition routine based on A's type
    switch (Aclass) {
    case FM_FLOAT: 
      {
	// A temporary vector to store the eigenvalues
	float *eigenvals = (float*) Malloc(N*sizeof(float));
	float *Vp = (float*) Malloc(N*N*A.getElementSize());
	floatEigenDecomposeSymmetric(N, Vp, eigenvals, (float*)A.getReadWriteDataPointer(),
				     true);
	// Copy the eigenvalues into a diagonal (float) matrix
	D = Array(FM_FLOAT,Vdims,NULL);
	float *Dp = (float*) Malloc(N*N*D.getElementSize());
	for (i=0;i<N;i++)
	  Dp[i+N*i] = eigenvals[i];
	D.setDataPointer(Dp);
	V = Array(FM_FLOAT,Vdims,Vp);
      }
      break;
    case FM_DOUBLE: 
      {
	// A temporary vector to store the eigenvalues
	double *eigenvals = (double*) Malloc(N*sizeof(double));
	double *Vp = (double*) Malloc(N*N*A.getElementSize());
	doubleEigenDecomposeSymmetric(N, Vp, eigenvals, (double*)A.getReadWriteDataPointer(),
				     true);
	// Copy the eigenvalues into a diagonal (double) matrix
	D = Array(FM_DOUBLE,Vdims,NULL);
	double *Dp = (double*) Malloc(N*N*D.getElementSize());
	for (i=0;i<N;i++)
	  Dp[i+N*i] = eigenvals[i];
	D.setDataPointer(Dp);
	V = Array(FM_DOUBLE,Vdims,Vp);
      }
      break;
    case FM_COMPLEX:
      {
	float *eigenvals = (float*) Malloc(N*sizeof(float));
	float *Vp = (float*) Malloc(N*N*A.getElementSize());
	complexEigenDecomposeSymmetric(N, Vp, eigenvals, 
				       (float*)A.getReadWriteDataPointer(),
				       true);
	// Copy the eigenvalues into a diagonal (real) matrix
	D = Array(FM_FLOAT,Vdims,NULL);
	float *Dp = (float*) Malloc(N*N*sizeof(float));
	for (i=0;i<N;i++) 
	  Dp[i+N*i] = eigenvals[i];
	D.setDataPointer(Dp);
	V = Array(FM_COMPLEX,Vdims,Vp);
	Free(eigenvals);
      }
      break;
    case FM_DCOMPLEX:
      {
	double *eigenvals = (double*) Malloc(N*sizeof(double));
	double *Vp = (double*) Malloc(N*N*A.getElementSize());
	dcomplexEigenDecomposeSymmetric(N, Vp, eigenvals, 
					(double*)A.getReadWriteDataPointer(),
					true);
	// Copy the eigenvalues into a diagonal (complex) matrix
	D = Array(FM_DOUBLE,Vdims,NULL);
	double *Dp = (double*) Malloc(N*N*sizeof(double));
	for (i=0;i<N;i++) 
	  Dp[i+N*i] = eigenvals[i];
	D.setDataPointer(Dp);
	V = Array(FM_DCOMPLEX,Vdims,Vp);
	Free(eigenvals);
      }
      break;
    }
  }

  /**
   * Perform an eigen decomposition of the matrix A - This version computes the 
   * eigenvectors, and returns the eigenvalues in a diagonal matrix
   */
  void EigenDecomposeFullGeneral(Array A, Array& V, Array& D, bool balanceFlag) {
    int i, j;
    Class Aclass;

    printf("ED-FG - balance = %d\r\n",balanceFlag);
    // Test for numeric
    if (A.isReferenceType())
      throw Exception("Cannot apply eigendecomposition to reference types.");
  
    if (!A.is2D())
      throw Exception("Cannot apply matrix operations to N-Dimensional arrays.");

    if (A.getDimensionLength(0) != A.getDimensionLength(1))
      throw Exception("Cannot eigendecompose a non-square matrix.");

    int N = A.getDimensionLength(0);

    // Create one square matrix to store the eigenvectors
    Dimensions Vdims(2);
    Vdims[0] = N;
    Vdims[1] = N;

    // Handle the type of A - if it is an integer type, then promote to double
    Aclass = A.getDataClass();
    if (Aclass < FM_FLOAT) {
      A.promoteType(FM_DOUBLE);
      Aclass = FM_DOUBLE;
    }

    // Select the eigenvector decomposition routine based on A's type
    switch (Aclass) {
    case FM_FLOAT: 
      {
	// A temporary vector to store the eigenvalues
	float *eigenvals = (float*) Malloc(2*N*sizeof(float));
	// For a real matrix, the eigenvectors are stored in a packed
	// format - complex eigenvectors are stored as two successive 
	// columns, corresponding to the real and imaginary parts of
	// the vector.  Successive columns can be used because the 
	// eigenvalues occur in conjugate pairs.
	float *Vp = (float*) Malloc(N*N*A.getElementSize());
	floatEigenDecompose(N, Vp, eigenvals, (float*)A.getReadWriteDataPointer(),
			    true, balanceFlag);
	// Make a pass through the eigenvals, and look for complex eigenvalues.
	bool complexEigenvalues = false;
	for (i=0;(i<N) && !complexEigenvalues;i++)
	  complexEigenvalues = (eigenvals[2*i+1] != 0);
	if (!complexEigenvalues) {
	  // Copy the eigenvalues into a diagonal (float) matrix
	  D = Array(FM_FLOAT,Vdims,NULL);
	  float *Dp = (float*) Malloc(N*N*D.getElementSize());
	  for (i=0;i<N;i++)
	    Dp[i+N*i] = eigenvals[2*i];
	  D.setDataPointer(Dp);
	  V = Array(FM_FLOAT,Vdims,Vp);
	} else {
	  // Copy the eigenvalues into a diagonal (complex) matrix
	  D = Array(FM_COMPLEX,Vdims,NULL);
	  float *Dp = (float*) Malloc(N*N*D.getElementSize());
	  for (i=0;i<N;i++) {
	    Dp[2*(i+N*i)] = eigenvals[2*i];
	    Dp[2*(i+N*i)+1] = eigenvals[2*i+1];
	  }
	  D.setDataPointer(Dp);
	  V = Array(FM_COMPLEX,Vdims,NULL);
	  // We have one or more complex eigenvalues.  Allocate
	  // space for a complex eigenvector matrix
	  float *Vpc = (float*) Malloc(N*N*V.getElementSize());
	  // Loop through the columns of Vpc
	  i = 0;
	  while (i<N) {
	    // Is this eigenvalue complex?
	    if (eigenvals[2*i+1] != 0) {
	      // Yes - Vpc[:,i] = V[:,i] + I*V[:,i+1]
	      //       Vpc[:,i+1] = V[:,i] - I*V[:,i+1]
	      for (j=0;j<N;j++) {
		Vpc[2*(j+N*i)] = Vp[j+N*i];
		Vpc[2*(j+N*i)+1] = Vp[j+N*(i+1)];
		Vpc[2*(j+N*(i+1))] = Vp[j+N*i];
		Vpc[2*(j+N*(i+1))+1] = -Vp[j+N*(i+1)];
	      }
	      i += 2;
	    } else {
	      for (j=0;j<N;j++)
		Vpc[2*(j+N*i)] = Vp[j+N*i];
	      i++;
	    }
	  }
	  Free(Vp);
	  V.setDataPointer(Vpc);
	}
	Free(eigenvals);
      }
      break;
    case FM_DOUBLE: 
      {
	// A temporary vector to store the eigenvalues
	double *eigenvals = (double*) Malloc(2*N*sizeof(double));
	// For a real matrix, the eigenvectors are stored in a packed
	// format - complex eigenvectors are stored as two successive 
	// columns, corresponding to the real and imaginary parts of
	// the vector.  Successive columns can be used because the 
	// eigenvalues occur in conjugate pairs.
	double *Vp = (double*) Malloc(N*N*A.getElementSize());
	doubleEigenDecompose(N, Vp, eigenvals, (double*)A.getReadWriteDataPointer(),
			     true, balanceFlag);
	// Make a pass through the eigenvals, and look for complex eigenvalues.
	bool complexEigenvalues = false;
	for (i=0;(i<N) && !complexEigenvalues;i++)
	  complexEigenvalues = (eigenvals[2*i+1] != 0);
	if (!complexEigenvalues) {
	  // Copy the eigenvalues into a diagonal (double) matrix
	  D = Array(FM_DOUBLE,Vdims,NULL);
	  double *Dp = (double*) Malloc(N*N*D.getElementSize());
	  for (i=0;i<N;i++) 
	    Dp[i+N*i] = eigenvals[2*i];
	  D.setDataPointer(Dp);
	  V = Array(FM_DOUBLE,Vdims,Vp);
	} else {
	  // Copy the eigenvalues into a diagonal (complex) matrix
	  D = Array(FM_DCOMPLEX,Vdims,NULL);
	  double *Dp = (double*) Malloc(N*N*D.getElementSize());
	  for (i=0;i<N;i++) {
	    Dp[2*(i+N*i)] = eigenvals[2*i];
	    Dp[2*(i+N*i)+1] = eigenvals[2*i+1];
	  }
	  D.setDataPointer(Dp);
	  V = Array(FM_DCOMPLEX,Vdims,NULL);
	  // We have one or more complex eigenvalues.  Allocate
	  // space for a complex eigenvector matrix
	  double *Vpc = (double*) Malloc(N*N*V.getElementSize());
	  // Loop through the columns of Vpc
	  i = 0;
	  while (i<N) {
	    // Is this eigenvalue complex?
	    if (eigenvals[2*i+1] != 0) {
	      // Yes - Vpc[:,i] = V[:,i] + I*V[:,i+1]
	      //       Vpc[:,i+1] = V[:,i] - I*V[:,i+1]
	      for (j=0;j<N;j++) {
		Vpc[2*(j+N*i)] = Vp[j+N*i];
		Vpc[2*(j+N*i)+1] = Vp[j+N*(i+1)];
		Vpc[2*(j+N*(i+1))] = Vp[j+N*i];
		Vpc[2*(j+N*(i+1))+1] = -Vp[j+N*(i+1)];
	      }
	      i += 2;
	    } else {
	      for (j=0;j<N;j++)
		Vpc[2*(j+N*i)] = Vp[j+N*i];
	      i++;
	    }
	  }
	  Free(Vp);
	  V.setDataPointer(Vpc);
	}
	Free(eigenvals);
      }
      break;
    case FM_COMPLEX:
      {
	float *eigenvals = (float*) Malloc(2*N*sizeof(float));
	float *Vp = (float*) Malloc(N*N*A.getElementSize());
	complexEigenDecompose(N, Vp, eigenvals, (float*)A.getReadWriteDataPointer(),
			      true, balanceFlag);
	// Copy the eigenvalues into a diagonal (complex) matrix
	D = Array(FM_COMPLEX,Vdims,NULL);
	float *Dp = (float*) Malloc(N*N*D.getElementSize());
	for (i=0;i<N;i++) {
	  Dp[2*(i+N*i)] = eigenvals[2*i];
	  Dp[2*(i+N*i)+1] = eigenvals[2*i+1];
	}
	D.setDataPointer(Dp);
	V = Array(FM_COMPLEX,Vdims,Vp);
	Free(eigenvals);
      }
      break;
    case FM_DCOMPLEX:
      {
	double *eigenvals = (double*) Malloc(2*N*sizeof(double));
	double *Vp = (double*) Malloc(N*N*A.getElementSize());
	dcomplexEigenDecompose(N, Vp, eigenvals, (double*)A.getReadWriteDataPointer(),
			       true, balanceFlag);
	// Copy the eigenvalues into a diagonal (complex) matrix
	D = Array(FM_DCOMPLEX,Vdims,NULL);
	double *Dp = (double*) Malloc(N*N*D.getElementSize());
	for (i=0;i<N;i++) {
	  Dp[2*(i+N*i)] = eigenvals[2*i];
	  Dp[2*(i+N*i)+1] = eigenvals[2*i+1];
	}
	D.setDataPointer(Dp);
	V = Array(FM_DCOMPLEX,Vdims,Vp);
	Free(eigenvals);
      }
      break;
    }
  }

  /**
   * Perform an eigen decomposition of the matrix A - This version computes the 
   * eigenvalues only in a vector
   */
  void EigenDecomposeCompactGeneral(Array A, Array& D, bool balanceFlag) {
    int i, j;
    Class Aclass;

    printf("ED-CG : balance = %d\r\n",balanceFlag);
    // Test for numeric
    if (A.isReferenceType())
      throw Exception("Cannot apply eigendecomposition to reference types.");
  
    if (!A.is2D())
      throw Exception("Cannot apply matrix operations to N-Dimensional arrays.");

    if (A.getDimensionLength(0) != A.getDimensionLength(1))
      throw Exception("Cannot eigendecompose a non-square matrix.");

    int N = A.getDimensionLength(0);

    // Create one square matrix to store the eigenvectors
    Dimensions Vdims(2);
    Vdims[0] = N;
    Vdims[1] = 1;

    // Handle the type of A - if it is an integer type, then promote to double
    Aclass = A.getDataClass();
    if (Aclass < FM_FLOAT) {
      A.promoteType(FM_DOUBLE);
      Aclass = FM_DOUBLE;
    }

    // Select the eigenvector decomposition routine based on A's type
    switch (Aclass) {
    case FM_FLOAT: 
      {
	// A temporary vector to store the eigenvalues
	float *eigenvals = (float*) Malloc(2*N*sizeof(float));
	floatEigenDecompose(N, NULL, eigenvals, 
			    (float*)A.getReadWriteDataPointer(),
			    false, balanceFlag);
	// Make a pass through the eigenvals, and look for complex eigenvalues.
	bool complexEigenvalues = false;
	for (i=0;(i<N) && !complexEigenvalues;i++)
	  complexEigenvalues = (eigenvals[2*i+1] != 0);
	if (!complexEigenvalues) {
	  // Copy the eigenvalues into a real (float) vector
	  D = Array(FM_FLOAT,Vdims,NULL);
	  float *Dp = (float*) Malloc(N*D.getElementSize());
	  for (i=0;i<N;i++)
	    Dp[i] = eigenvals[2*i];
	  D.setDataPointer(Dp);
	} else {
	  // Copy the eigenvalues into a complex vector
	  D = Array(FM_COMPLEX,Vdims,NULL);
	  float *Dp = (float*) Malloc(N*D.getElementSize());
	  for (i=0;i<N;i++) {
	    Dp[2*i] = eigenvals[2*i];
	    Dp[2*i+1] = eigenvals[2*i+1];
	  }
	  D.setDataPointer(Dp);
	}
	Free(eigenvals);
      }
      break;
    case FM_DOUBLE: 
      {
	// A temporary vector to store the eigenvalues
	double *eigenvals = (double*) Malloc(2*N*sizeof(double));
	doubleEigenDecompose(N, NULL, eigenvals, 
			     (double*)A.getReadWriteDataPointer(),
			     false, balanceFlag);
	// Make a pass through the eigenvals, and look for complex eigenvalues.
	bool complexEigenvalues = false;
	for (i=0;(i<N) && !complexEigenvalues;i++)
	  complexEigenvalues = (eigenvals[2*i+1] != 0);
	if (!complexEigenvalues) {
	  // Copy the eigenvalues into a real (double) vector
	  D = Array(FM_DOUBLE,Vdims,NULL);
	  double *Dp = (double*) Malloc(N*D.getElementSize());
	  for (i=0;i<N;i++)
	    Dp[i] = eigenvals[2*i];
	  D.setDataPointer(Dp);
	} else {
	  // Copy the eigenvalues into a complex vector
	  D = Array(FM_DCOMPLEX,Vdims,NULL);
	  double *Dp = (double*) Malloc(N*D.getElementSize());
	  for (i=0;i<N;i++) {
	    Dp[2*i] = eigenvals[2*i];
	    Dp[2*i+1] = eigenvals[2*i+1];
	  }
	  D.setDataPointer(Dp);
	}
	Free(eigenvals);
      }
      break;
    case FM_COMPLEX:
      {
	float *eigenvals = (float*) Malloc(2*N*sizeof(float));
	complexEigenDecompose(N, NULL, eigenvals, 
			      (float*)A.getReadWriteDataPointer(),
			      false, balanceFlag);
	// Copy the eigenvalues into a diagonal (complex) matrix
	D = Array(FM_COMPLEX,Vdims,eigenvals);
      }
      break;
    case FM_DCOMPLEX:
      {
	double *eigenvals = (double*) Malloc(2*N*sizeof(double));
	dcomplexEigenDecompose(N, NULL, eigenvals, 
			       (double*)A.getReadWriteDataPointer(),
			       false, balanceFlag);
	// Copy the eigenvalues into a diagonal (complex) matrix
	D = Array(FM_DCOMPLEX,Vdims,eigenvals);
      }
      break;
    }
  }

  bool GeneralizedEigenDecomposeCompactSymmetric(Array A, Array B, Array& D) {
    Class Aclass, Bclass;

    printf("GD-CS\r\n");
    // Test for numeric
    if (A.isReferenceType() || B.isReferenceType())
      throw Exception("Cannot apply eigendecomposition to reference types.");
    if (!A.is2D() || !B.is2D())
      throw Exception("Cannot apply matrix operations to N-Dimensional arrays.");
    if (A.getDimensionLength(0) != A.getDimensionLength(1))
      throw Exception("Cannot eigendecompose a non-square matrix.");
    if (B.getDimensionLength(0) != B.getDimensionLength(1))
      throw Exception("Cannot eigendecompose a non-square matrix.");
    if (A.getDimensionLength(0) != B.getDimensionLength(0))
      throw Exception("B and A must be the same size when computing a generalized eigendecomposition");
    int N = A.getDimensionLength(0);

    // Create one square matrix to store the eigenvectors
    Dimensions Vdims(2);
    Vdims[0] = N;
    Vdims[1] = 1;

    // Handle the type of A - if it is an integer type, then promote to double
    Aclass = A.getDataClass();
    if (Aclass < FM_FLOAT) {
      A.promoteType(FM_DOUBLE);
      Aclass = FM_DOUBLE;
    }
    Bclass = B.getDataClass();
    if (Bclass < Aclass) {
      B.promoteType(Aclass);
      Bclass = Aclass;
    } else {
      A.promoteType(Bclass);
      Aclass = Bclass;
    }

    // Select the eigenvector decomposition routine based on A's type
    Dimensions VDims(2);
    VDims[0] = N;
    VDims[1] = 1;
    switch (Aclass) {
    case FM_FLOAT: 
      {
	// A temporary vector to store the eigenvalues
	float *eigenvals = (float*) Malloc(N*sizeof(float));
	if (!floatGenEigenDecomposeSymmetric(N, NULL, eigenvals, 
					     (float*)A.getReadWriteDataPointer(),
					     (float*)B.getReadWriteDataPointer(),
					     false)) {
	  Free(eigenvals);
	  return false;
	}
	// Copy the eigenvalues into a diagonal (float) matrix
	D = Array(FM_FLOAT,Vdims,eigenvals);
      }
      break;
    case FM_DOUBLE: 
      {
	// A temporary vector to store the eigenvalues
	double *eigenvals = (double*) Malloc(N*sizeof(double));
	if (!doubleGenEigenDecomposeSymmetric(N, NULL, eigenvals, 
					      (double*)A.getReadWriteDataPointer(),
					      (double*)B.getReadWriteDataPointer(),
					      false)) {
	  Free(eigenvals);
	  return false;
	}
	// Copy the eigenvalues into a diagonal (double) matrix
	D = Array(FM_DOUBLE,Vdims,eigenvals);
      }
      break;
    case FM_COMPLEX:
      {
	float *eigenvals = (float*) Malloc(N*sizeof(float));
	if (!complexGenEigenDecomposeSymmetric(N, NULL, eigenvals, 
					       (float*)A.getReadWriteDataPointer(),
					       (float*)B.getReadWriteDataPointer(),
					       false)) {
	  Free(eigenvals);
	  return false;
	}
	// Copy the eigenvalues into a diagonal (complex) matrix
	D = Array(FM_FLOAT,Vdims,eigenvals);
      }
      break;
    case FM_DCOMPLEX:
      {
	double *eigenvals = (double*) Malloc(N*sizeof(double));
	if (!dcomplexGenEigenDecomposeSymmetric(N, NULL, eigenvals, 
						(double*)A.getReadWriteDataPointer(),
						(double*)B.getReadWriteDataPointer(),
						false)) {
	  Free(eigenvals);
	  return false;
	}
	// Copy the eigenvalues into a diagonaal (complex) matrix
	D = Array(FM_DOUBLE,Vdims,eigenvals);
      }
      break;
    }
    return true;
  }

  /**
   * Eigen decomposition, symmetric matrix, full decomposition case
   */
  bool GeneralizedEigenDecomposeFullSymmetric(Array A, Array B, Array& V, Array& D) {
    int i;
    Class Aclass, Bclass;

    printf("GD-FS\r\n");
    // Test for numeric
    if (A.isReferenceType() || B.isReferenceType())
      throw Exception("Cannot apply eigendecomposition to reference types.");
    if (!A.is2D() || !B.is2D())
      throw Exception("Cannot apply matrix operations to N-Dimensional arrays.");
    if (A.getDimensionLength(0) != A.getDimensionLength(1))
      throw Exception("Cannot eigendecompose a non-square matrix.");
    if (B.getDimensionLength(0) != B.getDimensionLength(1))
      throw Exception("Cannot eigendecompose a non-square matrix.");
    if (A.getDimensionLength(0) != B.getDimensionLength(0))
      throw Exception("B and A must be the same size when computing a generalized eigendecomposition");
    int N = A.getDimensionLength(0);

    // Create one square matrix to store the eigenvectors
    Dimensions Vdims(2);
    Vdims[0] = N;
    Vdims[1] = N;

    // Handle the type of A - if it is an integer type, then promote to double
    Aclass = A.getDataClass();
    if (Aclass < FM_FLOAT) {
      A.promoteType(FM_DOUBLE);
      Aclass = FM_DOUBLE;
    }
    Bclass = B.getDataClass();
    if (Bclass < Aclass) {
      B.promoteType(Aclass);
      Bclass = Aclass;
    } else {
      A.promoteType(Bclass);
      Aclass = Bclass;
    }
    // Select the eigenvector decomposition routine based on A's type
    switch (Aclass) {
    case FM_FLOAT: 
      {
	// A temporary vector to store the eigenvalues
	float *eigenvals = (float*) Malloc(N*sizeof(float));
	float *Vp = (float*) Malloc(N*N*A.getElementSize());
	if (!floatGenEigenDecomposeSymmetric(N, Vp, eigenvals, 
					     (float*)A.getReadWriteDataPointer(),
					     (float*)B.getReadWriteDataPointer(),
					     true)) {
	  Free(eigenvals);
	  Free(Vp);
	  return false;
	}
	// Copy the eigenvalues into a diagonal (float) matrix
	D = Array(FM_FLOAT,Vdims,NULL);
	float *Dp = (float*) Malloc(N*N*D.getElementSize());
	for (i=0;i<N;i++)
	  Dp[i+N*i] = eigenvals[i];
	D.setDataPointer(Dp);
	V = Array(FM_FLOAT,Vdims,Vp);
      }
      break;
    case FM_DOUBLE: 
      {
	// A temporary vector to store the eigenvalues
	double *eigenvals = (double*) Malloc(N*sizeof(double));
	double *Vp = (double*) Malloc(N*N*A.getElementSize());
	if (!doubleGenEigenDecomposeSymmetric(N, Vp, eigenvals, 
					      (double*)A.getReadWriteDataPointer(),
					      (double*)B.getReadWriteDataPointer(),
					      true)) {
	  Free(eigenvals);
	  Free(Vp);	  
	}
	// Copy the eigenvalues into a diagonal (double) matrix
	D = Array(FM_DOUBLE,Vdims,NULL);
	double *Dp = (double*) Malloc(N*N*D.getElementSize());
	for (i=0;i<N;i++)
	  Dp[i+N*i] = eigenvals[i];
	D.setDataPointer(Dp);
	V = Array(FM_DOUBLE,Vdims,Vp);
      }
      break;
    case FM_COMPLEX:
      {
	float *eigenvals = (float*) Malloc(N*sizeof(float));
	float *Vp = (float*) Malloc(N*N*A.getElementSize());
	if (!complexGenEigenDecomposeSymmetric(N, Vp, eigenvals, 
					       (float*)A.getReadWriteDataPointer(),
					       (float*)B.getReadWriteDataPointer(),
					       true)) {
	  Free(eigenvals);
	  Free(Vp);
	  return false;
	}
	// Copy the eigenvalues into a diagonal (real) matrix
	D = Array(FM_FLOAT,Vdims,NULL);
	float *Dp = (float*) Malloc(N*N*sizeof(float));
	for (i=0;i<N;i++) 
	  Dp[i+N*i] = eigenvals[i];
	D.setDataPointer(Dp);
	V = Array(FM_COMPLEX,Vdims,Vp);
	Free(eigenvals);
      }
      break;
    case FM_DCOMPLEX:
      {
	double *eigenvals = (double*) Malloc(N*sizeof(double));
	double *Vp = (double*) Malloc(N*N*A.getElementSize());
	if (!dcomplexGenEigenDecomposeSymmetric(N, Vp, eigenvals, 
						(double*)A.getReadWriteDataPointer(),
						(double*)B.getReadWriteDataPointer(),
						true)) {
	  Free(eigenvals);
	  Free(Vp);
	  return false;
	}
	// Copy the eigenvalues into a diagonal (complex) matrix
	D = Array(FM_DOUBLE,Vdims,NULL);
	double *Dp = (double*) Malloc(N*N*sizeof(double));
	for (i=0;i<N;i++) 
	  Dp[i+N*i] = eigenvals[i];
	D.setDataPointer(Dp);
	V = Array(FM_DCOMPLEX,Vdims,Vp);
	Free(eigenvals);
      }
      break;
    }
    return true;
  }

  /**
   * Perform an eigen decomposition of the matrix A - This version computes the 
   * eigenvectors, and returns the eigenvalues in a diagonal matrix
   */
  void GeneralizedEigenDecomposeFullGeneral(Array A, Array B, Array& V, Array& D) {
    int i, j;
    Class Aclass;

    // Test for numeric
    printf("GD-FG\r\n");
    if (A.isReferenceType())
      throw Exception("Cannot apply eigendecomposition to reference types.");
    if (!A.is2D())
      throw Exception("Cannot apply matrix operations to N-Dimensional arrays.");
    if (A.getDimensionLength(0) != A.getDimensionLength(1))
      throw Exception("Cannot eigendecompose a non-square matrix.");
    int N = A.getDimensionLength(0);
    // Create one square matrix to store the eigenvectors
    Dimensions Vdims(2);
    Vdims[0] = N;
    Vdims[1] = N;
    // Handle the type of A - if it is an integer type, then promote to double
    Aclass = A.getDataClass();
    if (Aclass < FM_FLOAT) {
      A.promoteType(FM_DOUBLE);
      Aclass = FM_DOUBLE;
    }
    // Select the eigenvector decomposition routine based on A's type
    switch (Aclass) {
    case FM_FLOAT: 
      {
	// A temporary vector to store the eigenvalues
	float *eigenvals = (float*) Malloc(2*N*sizeof(float));
	// For a real matrix, the eigenvectors are stored in a packed
	// format - complex eigenvectors are stored as two successive 
	// columns, corresponding to the real and imaginary parts of
	// the vector.  Successive columns can be used because the 
	// eigenvalues occur in conjugate pairs.
	float *Vp = (float*) Malloc(N*N*A.getElementSize());
	floatGenEigenDecompose(N, Vp, eigenvals, (float*)A.getReadWriteDataPointer(),
			       (float*)B.getReadWriteDataPointer(), true);
	// Make a pass through the eigenvals, and look for complex eigenvalues.
	bool complexEigenvalues = false;
	for (i=0;(i<N) && !complexEigenvalues;i++)
	  complexEigenvalues = (eigenvals[2*i+1] != 0);
	if (!complexEigenvalues) {
	  // Copy the eigenvalues into a diagonal (float) matrix
	  D = Array(FM_FLOAT,Vdims,NULL);
	  float *Dp = (float*) Malloc(N*N*D.getElementSize());
	  for (i=0;i<N;i++)
	    Dp[i+N*i] = eigenvals[2*i];
	  D.setDataPointer(Dp);
	  V = Array(FM_FLOAT,Vdims,Vp);
	} else {
	  // Copy the eigenvalues into a diagonal (complex) matrix
	  D = Array(FM_COMPLEX,Vdims,NULL);
	  float *Dp = (float*) Malloc(N*N*D.getElementSize());
	  for (i=0;i<N;i++) {
	    Dp[2*(i+N*i)] = eigenvals[2*i];
	    Dp[2*(i+N*i)+1] = eigenvals[2*i+1];
	  }
	  D.setDataPointer(Dp);
	  V = Array(FM_COMPLEX,Vdims,NULL);
	  // We have one or more complex eigenvalues.  Allocate
	  // space for a complex eigenvector matrix
	  float *Vpc = (float*) Malloc(N*N*V.getElementSize());
	  // Loop through the columns of Vpc
	  i = 0;
	  while (i<N) {
	    // Is this eigenvalue complex?
	    if (eigenvals[2*i+1] != 0) {
	      // Yes - Vpc[:,i] = V[:,i] + I*V[:,i+1]
	      //       Vpc[:,i+1] = V[:,i] - I*V[:,i+1]
	      for (j=0;j<N;j++) {
		Vpc[2*(j+N*i)] = Vp[j+N*i];
		Vpc[2*(j+N*i)+1] = Vp[j+N*(i+1)];
		Vpc[2*(j+N*(i+1))] = Vp[j+N*i];
		Vpc[2*(j+N*(i+1))+1] = -Vp[j+N*(i+1)];
	      }
	      i += 2;
	    } else {
	      for (j=0;j<N;j++)
		Vpc[2*(j+N*i)] = Vp[j+N*i];
	      i++;
	    }
	  }
	  Free(Vp);
	  V.setDataPointer(Vpc);
	}
	Free(eigenvals);
      }
      break;
    case FM_DOUBLE: 
      {
	// A temporary vector to store the eigenvalues
	double *eigenvals = (double*) Malloc(2*N*sizeof(double));
	// For a real matrix, the eigenvectors are stored in a packed
	// format - complex eigenvectors are stored as two successive 
	// columns, corresponding to the real and imaginary parts of
	// the vector.  Successive columns can be used because the 
	// eigenvalues occur in conjugate pairs.
	double *Vp = (double*) Malloc(N*N*A.getElementSize());
	doubleGenEigenDecompose(N, Vp, eigenvals, (double*)A.getReadWriteDataPointer(),
				(double*)B.getReadWriteDataPointer(), true);
	// Make a pass through the eigenvals, and look for complex eigenvalues.
	bool complexEigenvalues = false;
	for (i=0;(i<N) && !complexEigenvalues;i++)
	  complexEigenvalues = (eigenvals[2*i+1] != 0);
	if (!complexEigenvalues) {
	  // Copy the eigenvalues into a diagonal (double) matrix
	  D = Array(FM_DOUBLE,Vdims,NULL);
	  double *Dp = (double*) Malloc(N*N*D.getElementSize());
	  for (i=0;i<N;i++) 
	    Dp[i+N*i] = eigenvals[2*i];
	  D.setDataPointer(Dp);
	  V = Array(FM_DOUBLE,Vdims,Vp);
	} else {
	  // Copy the eigenvalues into a diagonal (complex) matrix
	  D = Array(FM_DCOMPLEX,Vdims,NULL);
	  double *Dp = (double*) Malloc(N*N*D.getElementSize());
	  for (i=0;i<N;i++) {
	    Dp[2*(i+N*i)] = eigenvals[2*i];
	    Dp[2*(i+N*i)+1] = eigenvals[2*i+1];
	  }
	  D.setDataPointer(Dp);
	  V = Array(FM_DCOMPLEX,Vdims,NULL);
	  // We have one or more complex eigenvalues.  Allocate
	  // space for a complex eigenvector matrix
	  double *Vpc = (double*) Malloc(N*N*V.getElementSize());
	  // Loop through the columns of Vpc
	  i = 0;
	  while (i<N) {
	    // Is this eigenvalue complex?
	    if (eigenvals[2*i+1] != 0) {
	      // Yes - Vpc[:,i] = V[:,i] + I*V[:,i+1]
	      //       Vpc[:,i+1] = V[:,i] - I*V[:,i+1]
	      for (j=0;j<N;j++) {
		Vpc[2*(j+N*i)] = Vp[j+N*i];
		Vpc[2*(j+N*i)+1] = Vp[j+N*(i+1)];
		Vpc[2*(j+N*(i+1))] = Vp[j+N*i];
		Vpc[2*(j+N*(i+1))+1] = -Vp[j+N*(i+1)];
	      }
	      i += 2;
	    } else {
	      for (j=0;j<N;j++)
		Vpc[2*(j+N*i)] = Vp[j+N*i];
	      i++;
	    }
	  }
	  Free(Vp);
	  V.setDataPointer(Vpc);
	}
	Free(eigenvals);
      }
      break;
    case FM_COMPLEX:
      {
	float *eigenvals = (float*) Malloc(2*N*sizeof(float));
	float *Vp = (float*) Malloc(N*N*A.getElementSize());
	complexGenEigenDecompose(N, Vp, eigenvals, (float*)A.getReadWriteDataPointer(),
				 (float*)B.getReadWriteDataPointer(), true);
	// Copy the eigenvalues into a diagonal (complex) matrix
	D = Array(FM_COMPLEX,Vdims,NULL);
	float *Dp = (float*) Malloc(N*N*D.getElementSize());
	for (i=0;i<N;i++) {
	  Dp[2*(i+N*i)] = eigenvals[2*i];
	  Dp[2*(i+N*i)+1] = eigenvals[2*i+1];
	}
	D.setDataPointer(Dp);
	V = Array(FM_COMPLEX,Vdims,Vp);
	Free(eigenvals);
      }
      break;
    case FM_DCOMPLEX:
      {
	double *eigenvals = (double*) Malloc(2*N*sizeof(double));
	double *Vp = (double*) Malloc(N*N*A.getElementSize());
	dcomplexGenEigenDecompose(N, Vp, eigenvals, (double*)A.getReadWriteDataPointer(),
				  (double*)B.getReadWriteDataPointer(), true);
	// Copy the eigenvalues into a diagonal (complex) matrix
	D = Array(FM_DCOMPLEX,Vdims,NULL);
	double *Dp = (double*) Malloc(N*N*D.getElementSize());
	for (i=0;i<N;i++) {
	  Dp[2*(i+N*i)] = eigenvals[2*i];
	  Dp[2*(i+N*i)+1] = eigenvals[2*i+1];
	}
	D.setDataPointer(Dp);
	V = Array(FM_DCOMPLEX,Vdims,Vp);
	Free(eigenvals);
      }
      break;
    }
  }

  /**
   * Perform an eigen decomposition of the matrix A - This version computes the 
   * eigenvalues only in a vector
   */
  void GeneralizedEigenDecomposeCompactGeneral(Array A, Array B, Array& D) {
    int i, j;
    Class Aclass;

    printf("GD-CG\r\n");
    // Test for numeric
    if (A.isReferenceType())
      throw Exception("Cannot apply eigendecomposition to reference types.");
      if (!A.is2D())
      throw Exception("Cannot apply matrix operations to N-Dimensional arrays.");
    if (A.getDimensionLength(0) != A.getDimensionLength(1))
      throw Exception("Cannot eigendecompose a non-square matrix.");
    int N = A.getDimensionLength(0);
    // Create one square matrix to store the eigenvectors
    Dimensions Vdims(2);
    Vdims[0] = N;
    Vdims[1] = 1;
    // Handle the type of A - if it is an integer type, then promote to double
    Aclass = A.getDataClass();
    if (Aclass < FM_FLOAT) {
      A.promoteType(FM_DOUBLE);
      Aclass = FM_DOUBLE;
    }
    // Select the eigenvector decomposition routine based on A's type
    switch (Aclass) {
    case FM_FLOAT: 
      {
	// A temporary vector to store the eigenvalues
	float *eigenvals = (float*) Malloc(2*N*sizeof(float));
	floatGenEigenDecompose(N, NULL, eigenvals, 
			       (float*)A.getReadWriteDataPointer(),
			       (float*)B.getReadWriteDataPointer(),
			       false);
	// Make a pass through the eigenvals, and look for complex eigenvalues.
	bool complexEigenvalues = false;
	for (i=0;(i<N) && !complexEigenvalues;i++)
	  complexEigenvalues = (eigenvals[2*i+1] != 0);
	if (!complexEigenvalues) {
	  // Copy the eigenvalues into a real (float) vector
	  D = Array(FM_FLOAT,Vdims,NULL);
	  float *Dp = (float*) Malloc(N*D.getElementSize());
	  for (i=0;i<N;i++)
	    Dp[i] = eigenvals[2*i];
	  D.setDataPointer(Dp);
	} else {
	  // Copy the eigenvalues into a complex vector
	  D = Array(FM_COMPLEX,Vdims,NULL);
	  float *Dp = (float*) Malloc(N*D.getElementSize());
	  for (i=0;i<N;i++) {
	    Dp[2*i] = eigenvals[2*i];
	    Dp[2*i+1] = eigenvals[2*i+1];
	  }
	  D.setDataPointer(Dp);
	}
	Free(eigenvals);
      }
      break;
    case FM_DOUBLE: 
      {
	// A temporary vector to store the eigenvalues
	double *eigenvals = (double*) Malloc(2*N*sizeof(double));
	doubleGenEigenDecompose(N, NULL, eigenvals, 
				(double*)A.getReadWriteDataPointer(),
				(double*)B.getReadWriteDataPointer(),
				false);
	// Make a pass through the eigenvals, and look for complex eigenvalues.
	bool complexEigenvalues = false;
	for (i=0;(i<N) && !complexEigenvalues;i++)
	  complexEigenvalues = (eigenvals[2*i+1] != 0);
	if (!complexEigenvalues) {
	  // Copy the eigenvalues into a real (double) vector
	  D = Array(FM_DOUBLE,Vdims,NULL);
	  double *Dp = (double*) Malloc(N*D.getElementSize());
	  for (i=0;i<N;i++)
	    Dp[i] = eigenvals[2*i];
	  D.setDataPointer(Dp);
	} else {
	  // Copy the eigenvalues into a complex vector
	  D = Array(FM_DCOMPLEX,Vdims,NULL);
	  double *Dp = (double*) Malloc(N*D.getElementSize());
	  for (i=0;i<N;i++) {
	    Dp[2*i] = eigenvals[2*i];
	    Dp[2*i+1] = eigenvals[2*i+1];
	  }
	  D.setDataPointer(Dp);
	}
	Free(eigenvals);
      }
      break;
    case FM_COMPLEX:
      {
	float *eigenvals = (float*) Malloc(2*N*sizeof(float));
	complexGenEigenDecompose(N, NULL, eigenvals, 
				 (float*)A.getReadWriteDataPointer(),
				 (float*)B.getReadWriteDataPointer(),
				 false);
	// Copy the eigenvalues into a diagonal (complex) matrix
	D = Array(FM_COMPLEX,Vdims,eigenvals);
      }
      break;
    case FM_DCOMPLEX:
      {
	double *eigenvals = (double*) Malloc(2*N*sizeof(double));
	dcomplexGenEigenDecompose(N, NULL, eigenvals, 
				  (double*)A.getReadWriteDataPointer(),
				  (double*)B.getReadWriteDataPointer(),
				  false);
	// Copy the eigenvalues into a diagonal (complex) matrix
	D = Array(FM_DCOMPLEX,Vdims,eigenvals);
      }
      break;
    }
  }

  //!
  //@Module HERMITIAN Matrix Hermitian (Conjugate Transpose) Operator
  //@@Usage
  //Computes the Hermitian of the argument (a 2D matrix).  The syntax for its use is
  //@[
  //  y = a';
  //@]
  //where @|a| is a @|M x N| numerical matrix.  The output @|y| is a numerical matrix
  //of the same type of size @|N x M|.  This operator is the conjugating transpose,
  //which is different from the transpose operator @|.'| (which does not 
  //conjugate complex values).
  //@@Function Internals
  //The Hermitian operator is defined simply as
  //\[
  //  y_{i,j} = \overline{a_{j,i}}
  //\]
  //where @|y_ij| is the element in the @|i|th row and @|j|th column of the output matrix @|y|.
  //@@Examples
  //A simple transpose example:
  //@<
  //A = [1,2,0;4,1,-1]
  //A'
  //@>
  //Here, we use a complex matrix to demonstrate how the Hermitian operator conjugates the entries.
  //@<
  //A = [1+i,2-i]
  //A.'
  //@>
  //!
  Array Transpose(Array A) {
    A.hermitian();
    return A;
  }

  //!
  //@Module TRANSPOSE Matrix Transpose Operator
  //@@Usage
  //Performs a transpose of the argument (a 2D matrix).  The syntax for its use is
  //@[
  //  y = a.';
  //@]
  //where @|a| is a @|M x N| numerical matrix.  The output @|y| is a numerical matrix
  //of the same type of size @|N x M|.  This operator is the non-conjugating transpose,
  //which is different from the Hermitian operator @|'| (which conjugates complex values).
  //@@Function Internals
  //The transpose operator is defined simply as
  //\[
  //  y_{i,j} = a_{j,i}
  //\]
  //where @|y_ij| is the element in the @|i|th row and @|j|th column of the output matrix @|y|.
  //@@Examples
  //A simple transpose example:
  //@<
  //A = [1,2,0;4,1,-1]
  //A.'
  //@>
  //Here, we use a complex matrix to demonstrate how the transpose does \emph{not} conjugate the entries.
  //@<
  //A = [1+i,2-i]
  //A.'
  //@>
  //!
  Array DotTranspose(Array A) {
    A.transpose();
    return A;
  }


  inline Array PowerScalarMatrix(Array A, Array B) {
    // Do an eigendecomposition of B
    Array V, D;
    if (B.isSymmetric())
      EigenDecomposeFullSymmetric(B,V,D);
    else
      EigenDecomposeFullGeneral(B,V,D,false);
    // Get the diagonal part of D
    Array E = D.getDiagonal(0);
    // Call the vector version of the exponential
    Array F = DoPowerTwoArgFunction(A,E); // B, V, D, E, F
    // Construct a diagonal matrix from F
    Array G = Array::diagonalConstructor(F,0); // B, V, D, G, E, F
    // The output is (V*G)/V
    E = Multiply(V,G); // B, V, D, E, F
    return RightDivide(E,V); // B, D, F
  }

  inline Array PowerMatrixScalar(Array A, Array B) {
    // Do an eigendecomposition of A
    Array V, D;
    if (A.isSymmetric())
      EigenDecomposeFullSymmetric(A,V,D); //A, B, V, D
    else
      EigenDecomposeFullGeneral(A,V,D,false);
    // Get the diagonal part of D
    Array E = D.getDiagonal(0); // A, B, V, D, E
    // Call the vector version of the exponential
    Array F = DoPowerTwoArgFunction(E,B); // F, A, V, D
    // Construct a diagonal matrix from F
    Array G = Array::diagonalConstructor(F,0); // G, A, V, D, F
    // The output is (V*G)/V
    E = Multiply(V,G); // A, V, D, E, F
    return RightDivide(E,V); // C, A, D, F
  }

  /**
   * Matrix-matrix power - These are the cases to consider:
   *   1. scalar-scalar defer to dotpower
   *   2. square-scalar - if an integer, iteratively multiply
   *          To raise A^N power, we do a successive factorization
   *          A -> A^2 -> A^4
   * 13 -> 8 + 4 + 2 + 1
   *  5 -> 4 + 2 + 1
   *  1 -> 1
   *
   * 23 -> 16 + 8 + 4 + 2 + 1
   *  7 -> 4 + 2 + 1
   *  3 -> 2 + 1
   *  1 -> 1
   *
   * 
   *   3. scalar-square - Decompose 
   */
  //!
  //@Module POWER Matrix Power Operator
  //@@Usage
  //The power operator for scalars and square matrices.  This operator is really a 
  //combination of two operators, both of which have the same general syntax:
  //@[
  //  y = a ^ b
  //@]
  //The exact action taken by this operator, and the size and type of the output, 
  //depends on which of the two configurations of @|a| and @|b| is present:
  //\begin{enumerate}
  //  \item @|a| is a scalar, @|b| is a square matrix
  //  \item @|a| is a square matrix, @|b| is a scalar
  //\end{enumerate}
  //@@Function Internals
  //In the first case that @|a| is a scalar, and @|b| is a square matrix, the matrix power is defined in terms of the eigenvalue decomposition of @|b|.  Let @|b| have the following eigen-decomposition (problems arise with non-symmetric matrices @|b|, so let us assume that @|b| is symmetric):
  //\[
  //  b = E \begin{bmatrix} \lambda_1 & 0          & \cdots  & 0 \\
  //                              0   & \lambda_2  &  \ddots & \vdots \\
  //                              \vdots & \ddots & \ddots & 0 \\
  //                              0   & \hdots & 0 & \lambda_n \end{bmatrix}
  //      E^{-1}
  //\]
  //Then @|a| raised to the power @|b| is defined as
  //\[
  //  a^{b} = E \begin{bmatrix} a^{\lambda_1} & 0          & \cdots  & 0 \\
  //                              0   & a^{\lambda_2}  &  \ddots & \vdots \\
  //                              \vdots & \ddots & \ddots & 0 \\
  //                              0   & \hdots & 0 & a^{\lambda_n} \end{bmatrix}
  //      E^{-1}
  //\]
  //Similarly, if @|a| is a square matrix, then @|a| has the following eigen-decomposition (again, suppose @|a| is symmetric):
  //\[
  //  a = E \begin{bmatrix} \lambda_1 & 0          & \cdots  & 0 \\
  //                              0   & \lambda_2  &  \ddots & \vdots \\
  //                              \vdots & \ddots & \ddots & 0 \\
  //                              0   & \hdots & 0 & \lambda_n \end{bmatrix}
  //      E^{-1}
  //\]
  //Then @|a| raised to the power @|b| is defined as
  //\[
  //  a^{b} = E \begin{bmatrix} \lambda_1^b & 0          & \cdots  & 0 \\
  //                              0   & \lambda_2^b  &  \ddots & \vdots \\
  //                              \vdots & \ddots & \ddots & 0 \\
  //                              0   & \hdots & 0 & \lambda_n^b \end{bmatrix}
  //      E^{-1}
  //\]
  //@@Examples
  //We first define a simple @|2 x 2| symmetric matrix
  //@<
  //A = 1.5
  //B = [1,.2;.2,1]
  //@>
  //First, we raise @|B| to the (scalar power) @|A|:
  //@<
  //C = B^A
  //@>
  //Next, we raise @|A| to the matrix power @|B|:
  //@<
  //C = A^B
  //@>
  //!
  Array Power(Array A, Array B) throw(Exception){

    if (A.isScalar() && B.isScalar()) return DotPower(A,B);

    // Check for A & B numeric
    CheckNumeric(A,B);

    // Test for 2D on both A & B
    if (!A.is2D() || !B.is2D())
      throw Exception("Cannot apply exponential operation to N-Dimensional arrays.");

    // Test the types
    TypeCheck(A,B,true);

    // Both arguments must be square
    if ((A.getDimensionLength(0) != A.getDimensionLength(1)) ||
	(B.getDimensionLength(0) != B.getDimensionLength(1)))
      throw Exception("Power (^) operator can only be applied to scalar and square arguments.");

    // OK - check for A a scalar - if so, do a decomposition of B
    int i;
    if (A.isScalar())
      return PowerScalarMatrix(A,B);
    else if (B.isScalar())
      return PowerMatrixScalar(A,B);
    else 
      throw Exception("One of the arguments to (^) must be a scalar.");
  }

  Array UnitColon(Array A, Array B) throw(Exception) {
    Array C;
    if (!A.isScalar() || !B.isScalar())
      throw Exception("Both arguments to (:) operator must be scalars.");
    if (A.isComplex() || B.isComplex())
      throw Exception("Both arguments to (:) operator must be real.");
    // Make sure A and B are the same type - at least INT32
    Class Aclass, Bclass, Cclass;
    Aclass = A.getDataClass();
    Bclass = B.getDataClass();
    Cclass = (Aclass > Bclass) ? Aclass : Bclass;
    Cclass = (FM_INT32 > Cclass) ? FM_INT32 : Cclass;
    A.promoteType(Cclass);
    B.promoteType(Cclass);
    switch (Cclass) {
    case FM_INT32:
      C = Array::int32RangeConstructor(*((int32*)A.getDataPointer()),
				       1,
				       *((int32*)B.getDataPointer()),
				       false);
      break;
    case FM_FLOAT: 
      C = Array::floatRangeConstructor(*((float*)A.getDataPointer()),
				       1,
				       *((float*)B.getDataPointer()),
				       false);
      break;
    case FM_DOUBLE:
      C = Array::doubleRangeConstructor(*((double*)A.getDataPointer()),
					1,
					*((double*)B.getDataPointer()),
					false);
      break;
    }
    return C;
  }

  Array DoubleColon(Array A, Array B, Array C) throw(Exception){
    Array D;
    if (!A.isScalar() || !B.isScalar() || !C.isScalar())
      throw Exception("All three arguments to (:) operator must be scalars.");
    if (A.isComplex() || B.isComplex() || C.isComplex())
      throw Exception("All arguments to (:) operator must be real.");
    // Make sure A and B are the same type - at least INT32
    Class Aclass, Bclass, Cclass, Dclass;
    Aclass = A.getDataClass();
    Bclass = B.getDataClass();
    Cclass = C.getDataClass();
    Dclass = (Aclass > Bclass) ? Aclass : Bclass;
    Dclass = (Dclass > Cclass) ? Dclass : Cclass;
    Dclass = (FM_INT32 > Dclass) ? FM_INT32 : Dclass;
    A.promoteType(Dclass);
    B.promoteType(Dclass);
    C.promoteType(Dclass);
    switch (Dclass) {
    case FM_INT32:
      D = Array::int32RangeConstructor(*((int32*)A.getDataPointer()),
				       *((int32*)B.getDataPointer()),
				       *((int32*)C.getDataPointer()),
				       false);
      break;
    case FM_FLOAT: 
      D = Array::floatRangeConstructor(*((float*)A.getDataPointer()),
				       *((float*)B.getDataPointer()),
				       *((float*)C.getDataPointer()),
				       false);
      break;
    case FM_DOUBLE:
      D =  Array::doubleRangeConstructor(*((double*)A.getDataPointer()),
					 *((double*)B.getDataPointer()),
					 *((double*)C.getDataPointer()),
					 false);
      break;
    }
    return D;
  }
}
