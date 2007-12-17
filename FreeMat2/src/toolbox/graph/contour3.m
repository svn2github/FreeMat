%!
%@Module CONTOUR3 3D Contour Plot Function
%@@Section HANDLE
%@@Usage
%This command generates contour plots where the lines are plotted in 3D.
%The syntax for its use is identical to the @|contour| function.  Please
%see its help for details.
%@@Example
%Here is a simple example of a 3D contour plot.
%@<
%[x,y] = meshgrid([-2:.25:2]);
%z=x.*exp(-x.^2-y.^2);
%contour3(x,y,z,30);
%axis square;
%view(-15,25)
%mprint contour3_1
%@>
%The resulting plot
%@figure contour3_1
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function ohandle = contour3(varargin)
  handle = contour(varargin{:});
  set(handle,'floating','on');
  if (nargout > 0) 
    ohandle = handle;
  end
  
