%!
%@Module POLYFIT Fit Polynomial To Data
%@@Section CURVEFIT
%@@Usage
%The @|polyfit| routine has the following syntax
%@[
%  p = polyfit(x,y,n)
%@]
%where @|x| and @|y| are vectors of the same size, and
%@|n| is the degree of the approximating polynomial.  
%The resulting vector @|p| forms the coefficients of
%the optimal polynomial (in descending degree) that fit
%@|y| with @|x|.  
%@@Function Internals
%The @|polyfit| routine finds the approximating polynomial
%\[
%   p(x) = p_1 x^n + p_2 x^{n-1} + \dots + p_n x + p_{n+1}
%\]
%such that
%\[
%   \sum_{i} (p(x_i) - y_i)^2
%\]
%is minimized.  It does so by forming the Vandermonde matrix
%and solving the resulting set of equations using the backslash
%operator.  Note that the Vandermonde matrix can become poorly
%conditioned with large @|n| quite rapidly.
%@@Example
%A classic example from Edwards and Penny, consider the problem
%of approximating a sinusoid with a polynomial.  We start with
%a vector of points evenly spaced on the unit interval, along with
%a vector of the sine of these points.
%@<
%x = linspace(0,1,20);
%y = sin(2*pi*x);
%plot(x,y,'r-')
%mprintplot polyfit1
%@>
%The resulting plot is shown here
%@figure polyfit1
%Next, we fit a third degree polynomial to the sine, and use
%@|polyval| to plot it
%@<
%p = polyfit(x,y,3)
%f = polyval(p,x);
%plot(x,y,'r-',x,f,'ko');
%mprintplot polyfit2
%@>
%The resulting plot is shown here
%@figure polyfit2
%Increasing the order improves the fit, as
%@<
%p = polyfit(x,y,11)
%f = polyval(p,x);
%plot(x,y,'r-',x,f,'ko');
%mprintplot polyfit3
%@>
%The resulting plot is shown here
%@figure polyfit3
%!
function p = polyfit(x,y,n)
x = x(:);
y = y(:);
if (size(x,1) ~= size(y,1))
  error('size mismatch on arguments to polyfit - they should be the same size');
end
if (n < 0 | n > 200)
  error('invalid polynomial order in polyfit - must be between [1,200] inclusive');
end
A = zeros(size(x,1),n+1);
xp = x*0.0f+1.0f;
for i=(n+1):-1:1
  A(:,i) = xp;
  xp = xp.*x;
end
p = (A\y)';
