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
%@$exact#y1=polyder(polyint(x1))#(any(loopi==[50:52]))
%@$exact#y1=polyder(x1,4)
%!
function [pder1,pder2] = polyder(p1,p2)
   if nargin < 1 | nargout > nargin
      error('wrong use (see help polyder)');
   end
   if (nargin == 1) 
      % Simple derivative case
      n = numel(p1);
      if (n <= 1)
        pder1 = 0;
        return;
      end;
      x1 = (p1(:).').*(n-1:-1:0);
      x1 = x1(1:end-1);
      pder1 = polyder_trim_zeros(x1);
      if (isa(p1,'single'))
        pder1 = single(pder1);
      end
      return;
   end
   f1 = conv(p1,polyder(p2));
   f2 = conv(polyder(p1),p2);
   m = max(numel(f1),numel(f2));
   f1 = [zeros(1,m-numel(f1)),f1(:).'];
   f2 = [zeros(1,m-numel(f2)),f2(:).'];
   if (nargout < 2)
     pder1 = polyder_trim_zeros(f1+f2);
   else
     pder1 = polyder_trim_zeros(f1-f2);
     pder2 = polyder_trim_zeros(conv(p2,p2));
   end;
   if (isa(p1,'single'))
     pder1 = single(pder1);
   end
   
function y = polyder_trim_zeros(x)
  if (isempty(x) | isempty(find(x,1)))
    y = 0;
  else
    y = x(find(x,1):end);
  end

