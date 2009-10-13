%!
%@Module POLYINT Polynomial Coefficient Integration
%@@Section CURVEFIT
%@@Usage
% The polyint function returns the polynomial coefficients resulting
% from integration of polynomial p. The syntax for its use is either
%@[
% pint = polyint(p,k)
%@]
% or, for a default @|k = 0|,
%@[
% pint = polyint(p);
%@]
% where @|p| is a vector of polynomial coefficients assumed to be in
% decreasing degree and @|k| is the integration constant.
% Contributed by Paulo Xavier Candeias under GPL
%@@Example
%Here is are some examples of the use of @|polyint|.
%@<
%polyint([2,3,4])
%@>
%And
%@<
%polyint([2,3,4],5)
%@>
%@@Tests
%@$exact#y1=polyint(x1)
%@$exact#y1=polyint(x1,5)
%!
function pint = polyint(p,k)
   if nargin < 1
      error('wrong use (see help polyint)');
   elseif nargin < 2
      k = 0;
   end
   pint = [(p(:).')./(length(p):-1:1),k];
