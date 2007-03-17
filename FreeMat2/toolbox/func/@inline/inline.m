%!
%@Module INLINE Construct Inline Function
%@@Section FUNCTION
%@@Usage
%Constructs an inline function object.  The syntax for its use is
%either
%@[
%   y = inline(expr)
%@]
%which uses the @|symvar| function to identify the variables in the
%expression, or the explicit form
%@[
%   y = inline(expr,var1,var2,...,varn)
%@]
%where the variables are explicitly given.  Note that inline functions
%are only partially supported in FreeMat.  If you need features of the
%inline function that are not currently implemented, please file a
%feature request at the FreeMat website.
%@@Example
%Here we construct an inline expression using the autodetection 
%of @|symvar|
%@<
%a = inline('x^2')
%a(3)
%a(i)
%@>
%In this case, we have multiple arguments (again, autodetected)
%@<
%a = inline('x+y-cos(x+y)')
%a(pi,-pi)
%@>
%In this form, we specify which arguments we want to use (thereby
%also specifying the order of the arguments
%@<
%a = inline('x+t-sin(x)','x','t')
%a(0.5,1)
%@>
%Inline objects can also be used with @|feval|
%@<
%a = inline('cos(t)')
%feval(a,pi/2)
%@>
%!
function y = inline(expr,varargin)
  % Create a default structure - we store the original expression
  % and the expression that assigns the arguments
  default.expr = '';
  default.assign_args = '';
  default.args = {};
  if (nargin == 0)
    y = class(default,'inline');
  elseif ((nargin == 1) && isa(expr,'inline'))
    y = expr;
  else
    if (nargin == 1)
      varargin = symvar(expr);
    end
    assign_args = '';
    for i=1:length(varargin)
      if (~ischar(varargin{i}))
	error(sprintf('argument %d to inline constructor is not a string',i));
      end
      assign_args = sprintf('%s %s = args{%d};',assign_args,varargin{i},i);
    end
    default.expr = expr;
    default.assign_args = assign_args;
    default.args = varargin;
    y = class(default,'inline');
  end
