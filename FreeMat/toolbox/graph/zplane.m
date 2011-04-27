% ZPLANE Zero-pole plot
% 
% Usage
% 
%  This function makes a zero-pole plot of a discrete-time
%  system defined by its zeros and poles. The various syntaxes
%  are
% 
%     zplane(z,p)
% 
%  where z and p are the zeros and the poles of the system
%  stored as column vectors, or
% 
%     zplane(b,a)
% 
%  where a and b are the polynomial coefficients of the
%  numerator and denominator stored as line vectors (roots is
%  used to find the zeros and poles). The symbol 'o' represents
%  a zero and the symbol 'x' represents a pole. The plot includes
%  the unit circle for reference.
% Contributed by Paulo Xavier Candeias under GPL
function zplane(zer,pol)
   figure;
   axis equal;
   axis square;
   grid('on')
   title('Pole/Zero Plot');
   xlabel('Real Part');
   ylabel('Imaginary Part');
   hold('on');
   [lz,cz] = size(zer);
   [lp,cp] = size(pol);
   if (cz == 1) & (cp == 1)
      % do nothing
   elseif (lz == 1) & (lp == 1)
      zer = roots(zer);
      pol = roots(pol);
   else
      error('wrong use (see help zplane)')
   end
   plot(real(zer),imag(zer),'bo');
   plot(real(pol),imag(pol),'bx');
   plot(exp(2i*pi*(0:0.01:1)),'b:');
