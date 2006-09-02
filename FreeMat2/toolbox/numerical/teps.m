%!
%@Module TEPS Type-based Epsilon Calculation
%@@Section CONSTANTS
%@@Usage
%Returns @|eps| for double precision arguments and
%@|feps| for single precision arguments.  The syntax for
%@|teps| is
%@[
%   y = teps(x)
%@]
%The @|teps| function is most useful if you need to
%compute epsilon based on the type of the array.
%@@Example
%The following example demonstrates the use of the @|teps| function,
%and one of its numerical consequences.
%@<
%teps(float(3.4))
%teps(complex(3.4+i*2))
%teps(double(3.4))
%teps(dcomplex(3.4+i*2))
%@>
%!

% Copyright (c) 2002-2006 Samit Basu

% Copyright (c) 2005 Samit Basu
function y = teps(x)
switch(typeof(x))
  case {'float','complex'}
    y = feps;
  case {'double','dcomplex','int8','uint8','int16','uint16','int32','uint32','int64','uint64'}
    y = eps;
  otherwise
    error('teps only applies to numerical arrays');
end
