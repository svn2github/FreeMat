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
  Array Add(const Array& A, const Array& B) {
    return(DoGenericTwoArgFunction(A,B,addFuncs));
  }

  /**
   * Subtract two objects.
   */
  Array Subtract(const Array& A, const Array& B) {
    return(DoGenericTwoArgFunction(A,B,subtractFuncs));
  }

  /**
   * Element-wise multiplication.
   */
  Array DotMultiply(Array A, Array B) {
    return(DoGenericTwoArgFunction(A,B,dotMultiplyFuncs));
  }

  /**
   * Element-wise right divide.
   */
  Array DotRightDivide(Array A, Array B) {
    return(DoGenericTwoArgFunction(A,B,dotRightDivideFuncs));
  }

  /**
   * Element-wise left divide.
   */
  Array DotLeftDivide(Array A, Array B) {
    return(DoGenericTwoArgFunction(A,B,dotLeftDivideFuncs));
  }

  /**
   * Element-wise power.
   */
  Array DotPower(Array A, Array B) {
    return(DoPowerTwoArgFunction(A,B));
  }

  /**
   * Element-wise less than.
   */
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
   * Perform an eigen decomposition of the matrix A
   */
  void EigenDecompose(Array A, Array& V, Array& D) throw(Exception) {
    int i, j;
    Class Aclass;

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
	floatEigenDecompose(N, Vp, eigenvals, (float*)A.getReadWriteDataPointer());
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
	doubleEigenDecompose(N, Vp, eigenvals, (double*)A.getReadWriteDataPointer());
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
	complexEigenDecompose(N, Vp, eigenvals, (float*)A.getReadWriteDataPointer());
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
	dcomplexEigenDecompose(N, Vp, eigenvals, (double*)A.getReadWriteDataPointer());
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

  Array Transpose(Array A) {
    A.hermitian();
    return A;
  }

  Array DotTranspose(Array A) {
    A.transpose();
    return A;
  }


  inline Array PowerScalarMatrix(Array A, Array B) {
    // Do an eigendecomposition of B
    Array V, D;
    EigenDecompose(B,V,D);
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
    EigenDecompose(A,V,D); //A, B, V, D
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
