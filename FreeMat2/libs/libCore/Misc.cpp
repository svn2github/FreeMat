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

#include "Core.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include "Math.hpp"
#ifndef WIN32
#include <unistd.h>
#else
#include <windows.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include "Malloc.hpp"
#include "Command.hpp"
#include "SingularValueDecompose.hpp"
#include "System.hpp"

namespace FreeMat {
  //!
  //@Module DISP Display a Variable or Expression
  //@@Usage
  //Displays the result of a set of expressions.  The @|disp| function
  //takes a variable number of arguments, each of which is an expression
  //to output:
  //@[
  //  disp(expr1,expr2,...,exprn)
  //@]
  //This is functionally equivalent to evaluating each of the expressions
  //without a semicolon after each.
  //@@Example
  //Here are some simple examples of using @|disp|.
  //@<
  //a = 32;
  //b = 1:4;
  //disp(a,b,pi)
  //@>
  //!
  ArrayVector DispFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
    int length;
    Array C;
    ArrayVector retval;

    length = arg.size();
    for (int i=0;i<length;i++) {
      C = arg[i];
      C.printMe(eval->getPrintLimit());
    }
    retval.push_back(C);
    return retval;
  } 

  //!
  //@Module EIG Eigendecomposition of a Matrix
  //@@Usage
  //Computes the eigendecomposition of a square matrix.  The @|eig| function
  //has two forms.  The first returns only the eigenvalues of the matrix:
  //@[
  //  s = eig(A)
  //@]
  //The second form returns both the eigenvectors and eigenvalues as two 
  //matrices (the eigenvalues are stored in a diagonal matrix):
  //@[
  //  [D,V] = eig(A)
  //@]
  //where @|D| is the diagonal matrix of eigenvalues, and @|V| is the
  //matrix of eigenvectors.
  //@@Function Internals
  //Recall that $v$ is an eigenvector $A$ with associated eigenvalue
  //$d$ if
  //\[
  //  A v = d v.
  //\]
  //This can be written in matrix form as
  //\[
  //  A V = V D
  //\]
  //where
  //\[
  //  V = [v_1,v_2,\ldots,v_n], D = \mathrm{diag}(d_1,d_2,\ldots,d_n).
  //\]
  //The @|eig| function uses the @|LAPACK| class of functions @|GEEVX|
  //to compute the eigenvalue decomposition.
  //@@Example
  //Some examples of eigenvalue decompositions.  First, for a diagonal
  //matrix, the eigenvalues are the diagonal elements of the matrix.
  //@<
  //A = diag(1.02f,3.04f,1.53f)
  //eig(A)
  //@>
  //Next, we compute the eigenvalues of an upper triangular matrix, 
  //where the eigenvalues are again the diagonal elements.
  //@<
  //A = [1.0f,3.0f,4.0f;0,2.0f;6.7f;0.0f,0.0f,1.0f]
  //eig(A)
  //@>
  //Next, we compute the complete eigenvalue decomposition of
  //a random matrix, and then resynthesize the matrix.
  //@<
  //A = float(randn(2))
  //[D,V] = eig(A)
  //(V * D) / V
  //@>
  //!
  ArrayVector EigFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("eig function takes exactly one argument - the matrix to decompose");
    ArrayVector retval;
    stringVector dummy;
    Array A(arg[0]);
    Array V, D;
    EigenDecompose(A,V,D);
    if (nargout > 1) {
      retval.push_back(D);
      retval.push_back(V);
    } else {
      retval.push_back(D.getDiagonal(0));
    }
    return retval;
  }

  //!
  //@Module SVD Singular Value Decomposition of a Matrix
  //@@Usage
  //Computes the singular value decomposition (SVD) of a matrix.  The 
  //@|svd| function has two forms.  The first returns only the singular
  //values of the matrix:
  //@[
  //  s = svd(A)
  //@]
  //The second form returns both the singular values in a diagonal
  //matrix @|S|, as well as the left and right eigenvectors.
  //@[
  //  [U,S,V] = svd(A)
  //@]
  //@@Function Internals
  //Recall that $\sigma_i$ is a singular value of an $M \times N$
  //matrix $A$ if there exists two vectors $u_i, v_i$ where $u_i$ is
  //of length $M$, and $v_i$ is of length $u_i$ and
  //\[
  //  A v_i = \sigma_i u_i
  //\]
  //and generally
  //\[
  //  A = \sum_{i=1}^{K} \sigma_i u_i*v_i',
  //\]
  //where $K$ is the rank of $A$.  In matrix form, the left singular
  //vectors $u_i$ are stored in the matrix $U$ as
  //\[
  //  U = [u_1,\ldots,u_m], V = [v_1,\ldots,v_n]
  //\]
  //The matrix $S$ is then of size $M \times N$ with the singular
  //values along the diagonal.  The SVD is computed using the 
  //@|LAPACK| call of functions @|GESDD|.
  //@@Examples
  //Here is an example of a partial and complete singular value
  //decomposition.
  //@<
  //A = float(randn(2,3))
  //[U,S,V] = svd(A)
  //U*S*V'
  //svd(A)
  //@>
  //!
  ArrayVector SVDFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("svd function takes exactly one argument - the matrix to decompose");

    Array A(arg[0]);

    // Test for numeric
    if (A.isReferenceType())
      throw Exception("Cannot apply svd to reference types.");
  
    if (!A.is2D())
      throw Exception("Cannot apply matrix operations to N-Dimensional arrays.");

    int nrows = A.getDimensionLength(0);
    int ncols = A.getDimensionLength(1);
    Class Aclass(A.getDataClass());
    if (Aclass < FM_FLOAT) {
      A.promoteType(FM_DOUBLE);
      Aclass = FM_DOUBLE;
    }
    ArrayVector retval;
    switch (Aclass) {
    case FM_FLOAT:
      {
	int mindim;
	int maxdim;
	mindim = (nrows < ncols) ? nrows : ncols;
	maxdim = (nrows < ncols) ? ncols : nrows;
	// A temporary vector to store the singular values
	float *svals = (float*) Malloc(mindim*sizeof(float));
	// A temporary vector to store the left singular vectors
	float *umat = (float*) Malloc(nrows*nrows*sizeof(float));
	// A temporary vector to store the right singular vectors
	float *vtmat = (float*) Malloc(ncols*ncols*sizeof(float));
	floatSVD(nrows,ncols,umat,vtmat,svals,(float*) A.getReadWriteDataPointer());
	// Always transfer the singular values into an Array
	Dimensions dim;
	if (nargout == 1) {
	  dim[0] = mindim; dim[1] = 1;
	  retval.push_back(Array(FM_FLOAT,dim,svals));
	  Free(umat);
	  Free(vtmat);
	} else {
	  dim[0] = nrows; dim[1] = nrows;
	  retval.push_back(Array(FM_FLOAT,dim,umat));
	  dim[0] = nrows; dim[1] = ncols;
	  float *smat = (float*) Malloc(nrows*ncols*sizeof(float));
	  for (int i=0;i<mindim;i++)
	    smat[i+i*nrows] = svals[i];
	  retval.push_back(Array(FM_FLOAT,dim,smat));
	  dim[0] = ncols; dim[1] = ncols;
	  Array Utrans(FM_FLOAT,dim,vtmat);
	  Utrans.transpose();
	  retval.push_back(Utrans);
	  Free(svals);
	}
	break;
      }
    case FM_DOUBLE:
      {
	int mindim;
	int maxdim;
	mindim = (nrows < ncols) ? nrows : ncols;
	maxdim = (nrows < ncols) ? ncols : nrows;
	// A temporary vector to store the singular values
	double *svals = (double*) Malloc(mindim*sizeof(double));
	// A temporary vector to store the left singular vectors
	double *umat = (double*) Malloc(nrows*nrows*sizeof(double));
	// A temporary vector to store the right singular vectors
	double *vtmat = (double*) Malloc(ncols*ncols*sizeof(double));
	doubleSVD(nrows,ncols,umat,vtmat,svals,(double*) A.getReadWriteDataPointer());
	// Always transfer the singular values into an Array
	Dimensions dim;
	if (nargout == 1) {
	  dim[0] = mindim; dim[1] = 1;
	  retval.push_back(Array(FM_DOUBLE,dim,svals));
	  Free(umat);
	  Free(vtmat);
	} else {
	  dim[0] = nrows; dim[1] = nrows;
	  retval.push_back(Array(FM_DOUBLE,dim,umat));
	  dim[0] = nrows; dim[1] = ncols;
	  double *smat = (double*) Malloc(nrows*ncols*sizeof(double));
	  for (int i=0;i<mindim;i++)
	    smat[i+i*nrows] = svals[i];
	  retval.push_back(Array(FM_DOUBLE,dim,smat));
	  dim[0] = ncols; dim[1] = ncols;
	  Array Utrans(FM_DOUBLE,dim,vtmat);
	  Utrans.transpose();
	  retval.push_back(Utrans);
	  Free(svals);
	}
	break;
      }
    case FM_COMPLEX:
      {
	int mindim;
	int maxdim;
	mindim = (nrows < ncols) ? nrows : ncols;
	maxdim = (nrows < ncols) ? ncols : nrows;
	// A temporary vector to store the singular values
	float *svals = (float*) Malloc(mindim*sizeof(float));
	// A temporary vector to store the left singular vectors
	float *umat = (float*) Malloc(2*nrows*nrows*sizeof(float));
	// A temporary vector to store the right singular vectors
	float *vtmat = (float*) Malloc(2*ncols*ncols*sizeof(float));
	complexSVD(nrows,ncols,umat,vtmat,svals,(float*) A.getReadWriteDataPointer());
	// Always transfer the singular values into an Array
	Dimensions dim;
	if (nargout == 1) {
	  dim[0] = mindim; dim[1] = 1;
	  retval.push_back(Array(FM_FLOAT,dim,svals));
	  Free(umat);
	  Free(vtmat);
	} else {
	  dim[0] = nrows; dim[1] = nrows;
	  retval.push_back(Array(FM_COMPLEX,dim,umat));
	  dim[0] = nrows; dim[1] = ncols;
	  float *smat = (float*) Malloc(nrows*ncols*sizeof(float));
	  for (int i=0;i<mindim;i++)
	    smat[i+i*nrows] = svals[i];
	  retval.push_back(Array(FM_FLOAT,dim,smat));
	  dim[0] = ncols; dim[1] = ncols;
	  Array Utrans(FM_COMPLEX,dim,vtmat);
	  Utrans.hermitian();
	  retval.push_back(Utrans);
	  Free(svals);
	}
	break;
      }
    case FM_DCOMPLEX:
      {
	int mindim;
	int maxdim;
	mindim = (nrows < ncols) ? nrows : ncols;
	maxdim = (nrows < ncols) ? ncols : nrows;
	// A temporary vector to store the singular values
	double *svals = (double*) Malloc(mindim*sizeof(double));
	// A temporary vector to store the left singular vectors
	double *umat = (double*) Malloc(2*nrows*nrows*sizeof(double));
	// A temporary vector to store the right singular vectors
	double *vtmat = (double*) Malloc(2*ncols*ncols*sizeof(double));
	dcomplexSVD(nrows,ncols,umat,vtmat,svals,(double*) A.getReadWriteDataPointer());
	// Always transfer the singular values into an Array
	Dimensions dim;
	if (nargout == 1) {
	  dim[0] = mindim; dim[1] = 1;
	  retval.push_back(Array(FM_DOUBLE,dim,svals));
	  Free(umat);
	  Free(vtmat);
	} else {
	  dim[0] = nrows; dim[1] = nrows;
	  retval.push_back(Array(FM_DCOMPLEX,dim,umat));
	  dim[0] = nrows; dim[1] = ncols;
	  double *smat = (double*) Malloc(nrows*ncols*sizeof(double));
	  for (int i=0;i<mindim;i++)
	    smat[i+i*nrows] = svals[i];
	  retval.push_back(Array(FM_DOUBLE,dim,smat));
	  dim[0] = ncols; dim[1] = ncols;
	  Array Utrans(FM_DCOMPLEX,dim,vtmat);
	  Utrans.hermitian();
	  retval.push_back(Utrans);
	  Free(svals);
	}
      }
      break;
    }
    return retval;
  }

  //!
  //@Module LASTERR Retrieve Last Error Message
  //@@Usage
  //Either returns or sets the last error message.  The
  //general syntax for its use is either
  //@[
  //  msg = lasterr
  //@]
  //which returns the last error message that occured, or
  //@[
  //  lasterr(msg)
  //@]
  //which sets the contents of the last error message.
  //@@Example
  //Here is an example of using the @|error| function to
  //set the last error, and then retrieving it using
  //lasterr.
  //@<
  //try; error('Test error message'); catch; end;
  //lasterr
  //@>
  //Or equivalently, using the second form:
  //@<
  //lasterr('Test message');
  //lasterr
  //@>
  //!
  ArrayVector LasterrFunction(int nargout, const ArrayVector& arg,
			      WalkTree* eval) {
     ArrayVector retval;
     if (arg->size() == 0) {
       Array A = Array::stringConstructor(eval->getLastErrorString());
       retval.push_back(A);
     } else {
       Array tmp(arg[0]);
       eval->setLastErrorString(tmp.getContentsAsCString());
     }
     return retval;
   }

  //!
  //@Module SLEEP Sleep For Specified Number of Seconds
  //@@Usage
  //Suspends execution of FreeMat for the specified number
  //of seconds.  The general syntax for its use is
  //@[
  //  sleep(n),
  //@]
  //where @|n| is the number of seconds to wait.
  //!
  ArrayVector SleepFunction(int nargout, const ArrayVector& arg) {
     if (arg.size() != 1)
       throw Exception("sleep function requires 1 argument");
     int sleeptime;
     Array a(arg[0]);
     sleeptime = a.getContentsAsIntegerScalar();
#ifndef WIN32
     sleep(sleeptime);
#else
     Sleep(1000*sleeptime);
#endif
     return ArrayVector();
   }

  //!
  //@Module DIAG Diagonal Matrix Construction/Extraction
  //@@Usage
  //The @|diag| function is used to either construct a 
  //diagonal matrix from a vector, or return the diagonal
  //elements of a matrix as a vector.  The general syntax
  //for its use is
  //@[
  //  y = diag(x,n)
  //@]
  //If @|x| is a matrix, then @|y| returns the @|n|-th 
  //diagonal.  If @|n| is omitted, it is assumed to be
  //zero.  Conversely, if @|x| is a vector, then @|y|
  //is a matrix with @|x| set to the @|n|-th diagonal.
  //@@Examples
  //Here is an example of @|diag| being used to extract
  //a diagonal from a matrix.
  //@<
  //A = int32(10*rand(4,5))
  //diag(A)
  //diag(A,1)
  //@>
  //Here is an example of the second form of @|diag|, being
  //used to construct a diagonal matrix.
  //@<
  //x = int32(10*rand(1,3))
  //diag(x)
  //diag(x,-1)
  //@>
  //!
  ArrayVector DiagFunction(int nargout, const ArrayVector& arg) {
    Array a;
    Array b;
    Array c;
    ArrayVector retval;
    int32 *dp;
    int diagonalOrder;
    // First, get the diagonal order, and synthesize it if it was
    // not supplied
    if (arg.size() == 1) 
      diagonalOrder = 0;
    else {
      b = arg[1];
      if (!b.isScalar()) 
	throw Exception("second argument must be a scalar.\n");
      b.promoteType(FM_INT32);
      dp = (int32 *) b.getDataPointer();
      diagonalOrder = dp[0];
    }
    // Next, make sure the first argument is 2D
    a = arg[0];
    if (!a.is2D()) 
      throw Exception("First argument to 'diag' function must be 2D.\n");
    // Case 1 - if the number of columns in a is 1, then this is a diagonal
    // constructor call.
    if ((a.getDimensionLength(1) == 1) || (a.getDimensionLength(0) == 1))
      c = Array::diagonalConstructor(a,diagonalOrder);
    else
      c = a.getDiagonal(diagonalOrder);
    retval.push_back(c);
    return retval;
  }

  //!
  //@Module ISEMPTY Test For Variable Empty
  //@@Usage
  //The @|isempty| function returns a boolean that indicates
  //if the argument variable is empty or not.  The general
  //syntax for its use is
  //@[
  //  y = isempty(x).
  //@]
  //@@Examples
  //Here are some examples of the @|isempty| function
  //@<
  //a = []
  //isempty(a)
  //b = 1:3
  //isempty(b)
  //@>
  //Note that if the variable is not defined, @|isempty| 
  //does not return true.
  //@<
  //isempty(x)
  //@>
  //!
  ArrayVector IsEmptyFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 1)
      throw Exception("isempty function requires at least input argument");
    ArrayVector retval;
    retval.push_back(Array::logicalConstructor(arg[0].isEmpty()));
    return retval;
  }

  //!
  //@Module ERROR Causes an Error Condition Raised
  //@@Usage
  //The @|error| function causes an error condition (exception
  //to be raised).  The general syntax for its use is
  //@[
  //   error(s),
  //@]
  //where @|s| is the string message describing the error.  The
  //@|error| function is usually used in conjunction with @|try|
  //and @|catch| to provide error handling.
  //@@Example
  //Here is a simple example of an @|error| being issued inside
  //a @|try|/@|catch| clause.
  //@<
  //try; error('Error occurred'); catch; 
  //@>
  //!
  ArrayVector ErrorFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() == 0)
      throw Exception("Not enough inputs to error function");
    if (!(arg[0].isString()))
      throw Exception("Input to error function must be a string");
    throw Exception(arg[0].getContentsAsCString());
  }


//   ArrayVector PrintStats(int nargout, const ArrayVector& arg) {
//     printObjectBalance();
//     printExceptionCount();
//     ArrayVector retval;
//     return retval;
//   }

//   ArrayVector PrintArrays(int nargout, const ArrayVector& arg) {
//     dumpAllArrays();
//     ArrayVector retval;
//     return retval;
//   }

//   ArrayVector TestFunction(int nargout, const ArrayVector& arg) {
//     if (arg.size() != 1)
//       throw Exception("test function requires exactly one argument");
//     ArrayVector retval;
//     Array A(arg[0]);
//     bool alltrue = true;
//     if (A.isEmpty())
//       alltrue = false;
//     else {
//       A.promoteType(FM_LOGICAL);
//       const logical* dp = (const logical *) A.getDataPointer();
//       int length = A.getLength();
//       int i = 0;
//       while (alltrue && (i<length)) {
// 	alltrue &= (dp[i]);
// 	i++;
//       }
//     }
//     retval.push_back(Array::logicalConstructor(alltrue));
//     return retval;
//   }

//   ArrayVector ClockFunction(int nargout, const ArrayVector& arg) {
//     ArrayVector retval;
// #ifndef WIN32
//     retval.push_back(Array::uint32Constructor(clock()));
// #else
// 	throw Exception("Clock function not available under win32");
// #endif
//     return retval;
//   }

  //!
  //@Module EVAL Evaluate a String
  //@@Usage
  //The @|eval| function evaluates a string.  The general syntax
  //for its use is
  //@[
  //   eval(s)
  //@]
  //where @|s| is the string to evaluate.
  //@@Example
  //Here are some examples of @|eval| being used.
  //@<
  //eval('a = 32')
  //@>
  //The primary use of the @|eval| statement is to enable construction
  //of expressions at run time.
  //@<
  //s = ['b = a' ' + 2']
  //eval(s)
  //@>
  //!
  ArrayVector EvalFunction(int nargout, const ArrayVector& arg,WalkTree* eval){
    if (arg.size() != 1)
      throw Exception("eval function takes exactly one argument - the string to execute");
    char *line = arg[0].getContentsAsCString();
    char *buf = (char*) malloc(strlen(line)+2);
    sprintf(buf,"%s\n",line);
    eval->evaluateString(buf);
    free(buf);
    return ArrayVector();
  }
  
  //!
  //@Module SOURCE Execute an Arbitrary File
  //@@Usage
  //The @|source| function executes the contents of the given
  //filename one line at a time (as if it had been typed at
  //the @|-->| prompt).  The @|source| function syntax is
  //@[
  //  source(filename)
  //@]
  //where @|filename| is a @|string| containing the name of
  //the file to process.
  //@@Example
  //First, we write some commands to a file (note that it does
  //not end in the usual @|.m| extension):
  //@<
  //fp = fopen('source_test','w');
  //fprintf(fp,'a = 32;\n');
  //fprintf(fp,'b = a;\n');
  //fclose(fp);
  //@>
  //Now we source the resulting file.
  //@<
  //clear all
  //source source_test
  //who
  //@>
  //!
  ArrayVector SourceFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
    if (arg.size() != 1)
      throw Exception("source function takes exactly one argument - the filename of the script to execute");
    char *line = arg[0].getContentsAsCString();
    FILE *fp;
    fp = fopen(line,"r");
    if (!fp)
      throw Exception(std::string("unable to open file ") + line + " for reading");
    while (!feof(fp)) {
      char buffer[4096];
      fgets(buffer,sizeof(buffer),fp);
      eval->evaluateString(buffer);
    }
    fclose(fp);
    return ArrayVector();
  }

//   ArrayVector FdumpFunction(int nargout, const ArrayVector& arg,WalkTree* eval){
//     if (arg.size() == 0)
//       throw Exception("fdump function requires at least one argument");
//     if (!(arg[0].isString()))
//       throw Exception("first argument to fdump must be the name of a function (i.e., a string)");
//     char *fname = arg[0].getContentsAsCString();
//     Context *context = eval->getContext();
//     FunctionDef *funcDef;
//     if (!context->lookupFunction(fname,funcDef))
//       throw Exception(std::string("function ") + fname + " undefined!");
//     funcDef->updateCode();
//     funcDef->printMe(eval->getInterface());
//     return ArrayVector();
//   }

  //!
  //@Module FEVAL Evaluate a Function
  //@@Usage
  //The @|feval| function executes a function using its name.
  //The syntax of @|feval| is
  //@[
  //  [y1,y2,...,yn] = feval(fname,x1,x2,...,xm)
  //@]
  //where @|fname| is the name of the function to evaluate, and
  //@|xi| are the arguments to the function, and @|yi| are the
  //return values.
  //@@Example
  //Here is an example of using @|feval| to call the @|cos| 
  //function indirectly.
  //@<
  //feval('cos',pi/4)
  //@>
  //!
  ArrayVector FevalFunction(int nargout, const ArrayVector& arg,WalkTree* eval){
    if (arg.size() == 0)
      throw Exception("feval function requires at least one argument");
    if (!(arg[0].isString()))
      throw Exception("first argument to feval must be the name of a function (i.e., a string)");
    char *fname = arg[0].getContentsAsCString();
    Context *context = eval->getContext();
    FunctionDef *funcDef;
    if (!context->lookupFunction(fname,funcDef))
      throw Exception(std::string("function ") + fname + " undefined!");
    funcDef->updateCode();
    if (funcDef->scriptFlag)
      throw Exception("cannot use feval on a script");
    ArrayVector newarg(arg);
    //    newarg.pop_front();
    newarg.erase(newarg.begin());
    return(funcDef->evaluateFunction(eval,newarg,nargout));
  }

  //!
  //@Module SYSTEM Call an External Program
  //@@Usage
  //The @|system| function allows you to call an external
  //program from within FreeMat, and capture the output.
  //The syntax of the @|system| function is
  //@[
  //  y = system(cmd)
  //@]
  //where @|cmd| is the command to execute.  The return
  //array @|y| is of type @|cell-array|, where each entry
  //in the array corresponds to a line from the output.
  //@@Example
  //Here is an example of calling the @|ls| function (the
  //list files function under Un*x-like operating system).
  //@<
  //y = system('ls')
  //y{1}
  //@>
  //!
  ArrayVector SystemFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1) 
      throw Exception("System function takes one string argument");
    char *systemArg = arg[0].getContentsAsCString();
    ArrayVector retval;
    if (strlen(systemArg) == 0) 
      return retval;
    stringVector cp(DoSystemCallCaptured(systemArg));
    Array* np = new Array[cp.size()];
    for (int i=0;i<cp.size();i++)
      np[i] = Array::stringConstructor(cp[i]);
    Dimensions dim(2);
    dim[0] = cp.size();
    dim[1] = 1;
    Array ret(FM_CELL_ARRAY,dim,np);
    retval.push_back(ret);
    return retval;
  }
}

