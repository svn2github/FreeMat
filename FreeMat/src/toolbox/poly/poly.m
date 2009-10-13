%!
%@Module POLY Convert Roots To Polynomial Coefficients
%@@Section CURVEFIT
%@@Usage
%This function calculates the polynomial coefficients for given roots
%@[ 
%    p = poly(r)
%@] 
%when @|r| is a vector, is a vector whose elements are the coefficients 
%of the polynomial whose roots are the elements of @|r|.  Alternately,
%you can provide a matrix
%@[
%    p = poly(A)
%@]
%when @|A| is an @|N x N| square matrix, is a row vector with 
%@|N+1| elements which are the coefficients of the
%characteristic polynomial, @|det(lambda*eye(size(A))-A)|.
%
%Contributed by Paulo Xavier Candeias under GPL.
%@@Example
%Here are some examples of the use of @|poly|
%@<
%A = [1,2,3;4,5,6;7,8,0]
%p = poly(A)
%r = roots(p)
%@>
%@@Tests
%@$near#y1=poly(x1)
%!
function p = poly(r)
   if (nargin < 1) | (nargout > 1)
      error('wrong use (see help poly)')
   end
   
   % Identify arguments
   if issquare(r)
      r = eig(r);
   else
      r = r(:).';
   end
   
   % Strip out infinities
   r = r( ~isinf(r) );
   
   % Expand recursion formula
   n = length(r);
   p = [1,zeros(1,n)];
   for v = 1:n
      p(2:v+1) = p(2:v+1)-r(v)*p(1:v);
   end

   % The result should be real if the roots are complex conjugates.
   r_neg_comp_roots = r(imag(r)<0);
   r_pos_comp_roots = r(imag(r)>0);
   if (numel(r_neg_comp_roots) == numel(r_pos_comp_roots))
     if sort(r(imag(r)>0))-sort(conj(r(imag(r)<0)))<teps(r)
        p = real(p);
     end
   end
