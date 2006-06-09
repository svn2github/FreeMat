%!
%@Module ANGLE Phase Angle Function
%@@Section MATHFUNCTIONS
%@@Usage
%Compute the phase angle in radians of a complex matrix.  The general
%syntax for its use is
%@[
%  p = angle(c)
%@]
%where @|c| is an @|n|-dimensional array of numerical type.
%@@Function Internals
%For a complex number @|x|, its polar representation is
%given by
%\[
%  x = |x| e^{j\theta}
%\]
%and we can compute 
%\[
%  \theta = \mathrm{atan2}(\Im x, \Re x)
%\]
%@@Example
%Here are some examples of the use of @|angle| in the polar decomposition
%of a complex number.
%@<
%x = 3+4*i
%a = abs(x)
%t = angle(x)
%a*exp(i*t)
%@>
%   M version contributor: M.W. Vogel 01-30-06
%@@Tests
%@{"y=angle(3+4*i)","0.92729521800161","close"}
%@{"y=angle(4)","0","exact"}
%!
function p = angle(c)

p = atan2(imag(c), real(c));

