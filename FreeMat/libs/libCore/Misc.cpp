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
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "Malloc.hpp"
#include "Command.hpp"
#include "SingularValueDecompose.hpp"
#include <wx/utils.h>

namespace FreeMat {
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

  ArrayVector EigFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("eig function takes exactly one argument - the matrix to decompose");
    ArrayVector retval;
    stringVector dummy;
    retval.resize(2);
    Array A(arg[0]);
    Array V, D;
    EigenDecompose(A,V,D);
    retval[0] = D;
    retval[1] = V;
    return retval;
  }

  /**
   * Perform a singular value decomposition of the matrix A
   */
  ArrayVector SVDFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("svd function takes exactly one argument - the matrix to decompose");

    Array A(arg[0]);

    // Test for numeric
    if (A.isReferenceType())
      throw Exception("Cannot apply eigendecomposition to reference types.");
  
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

  //  ArrayVector* LasterrFunction(int nargout, ArrayVector* arg) {
  //    ArrayVector *retval;
  //    stringVector dummy;
  //    retval = new ArrayVector();
  //    if (arg->size() == 0) {
  //      Array *A = Array::stringConstructor(lasterr);
  //      retval->push_back(A);
  //    } else {
  //      lasterr = (*arg)[0]->getContentsAsCString();
  //    }
  //    return retval;
  //  }

  ArrayVector SleepFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("sleep function requires 1 argument");
    int sleeptime;
    Array a(arg[0]);
    sleeptime = a.getContentsAsIntegerScalar();
    wxSleep(sleeptime);
    return ArrayVector();
  }

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

  ArrayVector IsEmptyFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 1)
      throw Exception("isempty function requires at least input argument");
    ArrayVector retval;
    retval.push_back(Array::logicalConstructor(arg[0].isEmpty()));
    return retval;
  }

  ArrayVector ErrorFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() == 0)
      throw Exception("Not enough inputs to error function");
    if (!(arg[0].isString()))
      throw Exception("Input to error function must be a string");
    throw Exception(arg[0].getContentsAsCString());
  }


  ArrayVector PrintStats(int nargout, const ArrayVector& arg) {
    printObjectBalance();
    printExceptionCount();
    ArrayVector retval;
    return retval;
  }

  ArrayVector PrintArrays(int nargout, const ArrayVector& arg) {
    dumpAllArrays();
    ArrayVector retval;
    return retval;
  }



  ArrayVector TestFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("test function requires exactly one argument");
    ArrayVector retval;
    Array A(arg[0]);
    bool alltrue = true;
    if (A.isEmpty())
      alltrue = false;
    else {
      A.promoteType(FM_LOGICAL);
      const logical* dp = (const logical *) A.getDataPointer();
      int length = A.getLength();
      int i = 0;
      while (alltrue && (i<length)) {
	alltrue &= (dp[i]);
	i++;
      }
    }
    retval.push_back(Array::logicalConstructor(alltrue));
    return retval;
  }

  ArrayVector ClockFunction(int nargout, const ArrayVector& arg) {
    ArrayVector retval;
    retval.push_back(Array::uint32Constructor(clock()));
    return retval;
  }
#if 0
  // List all of the variables in the global context
  ArrayVector WhoGlobalsFunction(int nargout,const ArrayVector& arg,WalkTree* eval) {
    Context *ctxt = eval->getContext();
    Scope *scpe = ctxt->getGlobalScope();
    scpe->printData();
    return ArrayVector();
  }

  // List all of the variables in the current context
  ArrayVector WhoFunction(int nargout,const ArrayVector& arg,WalkTree* eval) {
    Context *ctxt = eval->getContext();
    Scope *scpe = ctxt->getCurrentScope();
    scpe->printData();
    return ArrayVector();
  }
#endif

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

  ArrayVector FdumpFunction(int nargout, const ArrayVector& arg,WalkTree* eval){
    if (arg.size() == 0)
      throw Exception("fdump function requires at least one argument");
    if (!(arg[0].isString()))
      throw Exception("first argument to fdump must be the name of a function (i.e., a string)");
    char *fname = arg[0].getContentsAsCString();
    Context *context = eval->getContext();
    FunctionDef *funcDef;
    if (!context->lookupFunction(fname,funcDef))
      throw Exception(std::string("function ") + fname + " undefined!");
    funcDef->updateCode();
    funcDef->printMe(eval->getInterface());
    return ArrayVector();
  }
  
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
  
  ArrayVector SystemFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1) 
      throw Exception("System function takes one string argument");
    char *systemArg = arg[0].getContentsAsCString();
    ArrayVector retval;
    if (strlen(systemArg) == 0) 
      return retval;
    Command *cp;
    cp = new Command(CMD_SystemCapture,
		     Array::stringConstructor(systemArg));
    SendGUICommand(cp);
    cp = GetGUIResponse();
    retval.push_back(cp->data);
    return retval;
  }
}

