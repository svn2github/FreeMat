% CONTOUR3 CONTOUR3 3D Contour Plot Function
% 
% Usage
% 
% This command generates contour plots where the lines are plotted in 3D.
% The syntax for its use is identical to the contour function.  Please
% see its help for details.

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function ohandle = contour3(varargin)
  handle = contour(varargin{:});
  set(handle,'floating','on');
if ~ishold
	view( handle, 3 );
	grid;
end
  if (nargout > 0) 
    ohandle = handle;
  end
  
