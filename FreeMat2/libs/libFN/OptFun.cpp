

static Array xval;
static Array yval;

extern 'C' void fcnstub(int* m, int *n, double *x, double *fvec, int *iflag) {
  
}
typedef void (*fcnptr)(int*, int*, double*, double*, int*);

void lmdif1_(fcnptr, int*m, int*n, double*x, double*fvec, double*tol,
	     int*info, int*iwa, double*wa, int*lwa);

//!
//@Module FITFUN Fit a Function
//@@Usage
//Fits @|n| (non-linear) functions of @|m| variables using least squares
//and Levenberg-Marquardt algorithm.  The general syntax for its usage
//is
//@[
//  [xopt,yopt] = fitfun(fcn,xinit,y,tol,params...)
//@]
//Where @|fcn| is the name of the function to be fit, @|xinit| is the
//initial guess for the solution (required), @|y| is the right hand side,
//i.e., the vector @|y| such that:
//\[
//   xopt = \arg \min_{x} \|f(x) - y\|_2^2, yopt = f(xopt)
//\]
//The parameter @|tol| is the tolerance used for convergence.
//The function @|fcn| must return a vector of the same size as @|y|,
//and @|params| are passed to @|fcn| after the argument @|x|, i.e.,
//\[
//  y = fcn(x,param1,param2,...).
//\]
//Note that both @|x| and @|y| (and the output of the function) must all
//be real variables.  Complex variables are not handled yet.
//!
ArrayVector FitFunFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
  if (arg.size()<3) 
    throw Exception("fitfun requires at least three arguments");
  if (!(arg[0].isString()))
    throw Exception("first argument to fitfun must be the name of a function (i.e., a string)");
  char *fname = arg[0].getContentsAsCString();
  Context *context = eval->getContext();
  FunctionDef *funcDef;
  if (!context->lookupFunction(fname,funcDef))
    throw Exception(std::string("function ") + fname + " undefined!");
  funcDef->updateCode();
  if (funcDef->scriptFlag)
    throw Exception("cannot use feval on a script");
  // Get the initial guess vector
  Array xinit(arg[1]);
  xinit.promoteType(FM_DOUBLE);
  int m, n;
  n = yvec.getLength();
  m = xinit.getLength();
  // Get the right hand side vector
  Array yvec(arg[2]);
  yvec.promoteType(FM_DOUBLE);
  // Get the tolerance
  Array tolc(arg[3]);
  double tol = tol.getContentsAsDoubleScalar();
  // Copy the arg array
  ArrayVector params(arg);
  params.erase(params.begin(),params.begin()+4);
  // Test to make sure the function works....
  ArrayVector tocall(params);
  tocall.push_front(xinit);
  ArrayVector cval(funcDef->evaluateFunction(eval,tocall,1));
  if (cval.getLength() != n)
    throw Exception("function output does not match size of vector 'y'");
  
  // Copy the arguments
  
 
}
