%!
%@Module POLYDER Polynomial Coefficient Differentiation
%@@Section CURVEFIT
%@@Usage
%The @|polyder| function returns the polynomial coefficients resulting
%from differentiation of polynomial @|p|. The syntax for its use is either
%@[
% pder = polyder(p)
%@]
% for the derivitave of polynomial p, or
%@[
% convp1p2der = polyder(p1,p2)
%@]
% for the derivitave of polynomial conv(p1,p2), or still
%@[
% [nder,dder] = polyder(n,d)
%@]
%for the derivative of polynomial @|n/d| (@|nder| is the numerator
%and @|dder| is the denominator). In all cases the polynomial 
%coefficients are assumed to be in decreasing degree.
%Contributed by Paulo Xavier Candeias under GPL
%@@Example
%Here are some examples of the use of @|polyder|
%@<
%polyder([2,3,4])
%@>
%@<
%polyder([2,3,4],7)
%@>
%@<
%[n,d] = polyder([2,3,4],5)
%@>
%@@Tests
%@$"y=polyder(polyint([2,3,4]))","[2,3,4]","close"
%@$"y=polyint(polyder([2,3,4]),4)","[2,3,4]","close"
%!
function [pder1,pder2] = polyder(p1,p2)
   if nargin < 1 | nargout > nargin
      error('wrong use (see help polyder)');
   elseif nargin < 2
      p2 = 1;
   end
   zp1 = [0,p1(:).'];
   zp2 = [0,p2(:).'];
   pd1p2 = conv(zp1(1:end-1).*(length(zp1)-1:-1:1),zp2);
   p1pd2 = conv(zp1,zp2(1:end-1).*(length(zp2)-1:-1:1));
   if nargout < 2
      pder1 = pd1p2+p1pd2;
      pder2 = 0;
   else
      pder1 = pd1p2-p1pd2;
      pder2 = conv(zp2,zp2);
   end
   pder1 = pder1(min(find(pder1,1),length(pder1)):end);
   pder2 = pder2(min(find(pder2,1),length(pder2)):end);
