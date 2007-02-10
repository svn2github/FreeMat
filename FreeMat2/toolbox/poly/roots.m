%!
%@Module ROOTS Find Roots of Polynomial
%@@Section CURVEFIT
%@@Usage
%The @|roots| routine will return a column vector containing the
%roots of a polynomial.  The general syntax is
%@[
%   z = roots(p)
%@]
%where @|p| is a vector containing the coefficients of the polynomial
%ordered in descending powers.  
%@@Function Internals
%Given a vector 
%\[
%   [p_1, p_2, \dots p_n]
%\]
%which describes a polynomial
%\[
%   p_1 x^{n-1} + p_2 x^{n-2} + \dots + p_n
%\]
%we construct the companion matrix (which has a characteristic polynomial
%matching the polynomial described by @|p|), and then find the eigenvalues
%of it (which are the roots of its characteristic polynomial), and
%which are also the roots of the polynomial of interest.  This technique
%for finding the roots is described in the help page for @|roots| on the Mathworks
%website.
%@@Example
%Here is an example of finding the roots to the polynomial
%\[
%   x^3 - 6x^2 - 72x - 27
%\]
%@<
%roots([1 -6 -72 -27])
%@>
%@@Tests
%@$"y=roots([1,-6,-72,-27])","[12.1228937846323905;-5.7345099422250749;-0.3883838424073199]","close"
%!
function z = roots(p)
  p = vec(p);
  n = numel(p)-1;
  A = diag(ones(n-1,1),-1);
  A(1,:) = -p(2:n+1)./p(1);
  z = eig(A);
  
