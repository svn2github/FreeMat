% PATCH PATCH Patch Graphics Function
% 
% Usage
% 
% This routine is used to create a patch object that can be plotting 2D and 3D surfaces.  A 
% patch is a polygon defined by the xyz coordinates
% of its vertices and optionally by the color at the vertices.
% There are several forms for the patch function:
% 
%   h = patch(X,Y,C,properties...)
%   h = patch(X,Y,Z,C,properties...)
%   h = patch(properties...)
%   h = patch(V)
% 
% Where X, Y and Z are matrices or vectors of x, y or z coordinates
% and C is a matrix or vector of color values (the colormap
% for the current fig is applied).  

% Copyright (c) 2002-2008 Samit Basu, Eugene Ingerman
% Licensed under the GPL

function ohandle = patch(varargin)

   if( nargin==1 )
	error('not implemented')
   end
	
  % search for the propertyname/value pairs
  propstart = 0;
  if (nargin > 2)
    propstart = nargin-1;
    while ((propstart >= 1) && isa(varargin{propstart},'char') && ...
    pvalid('patch',varargin{propstart}))
      propstart = propstart - 2;
    end
    propstart = propstart + 2;
  end
  propset = {};
  if ((propstart > 0) && (propstart < nargin))
     propset = varargin(propstart:end);
     varargin(propstart:end) = [];
  end
  if (length(varargin) == 0)
	h = hpatch(propset{:});
  elseif (length(varargin) == 3)
    [vertices, faces, facevertexcdata]=parse_input( varargin{1}, varargin{2}, ones(size(varargin{1})), varargin{3} );
    h = hpatch('xdata',varargin{1},'ydata',varargin{2},'zdata',ones(size(varargin{1})),'cdata',varargin{3},...
	'faces',faces,'vertices',vertices,'facevertexcdata',facevertexcdata,'facecolor','flat','edgecolor','flat',propset{:});
  elseif (length(varargin) == 4)
   [vertices, faces, facevertexcdata]=parse_input( varargin{1}, varargin{2}, varargin{3}, varargin{4} );
    h = hpatch('xdata',varargin{1},'ydata',varargin{2},'zdata',varargin{3},'cdata',varargin{4},...
	'faces',faces,'vertices',vertices,'facevertexcdata',facevertexcdata,'facecolor','flat','edgecolor','flat',propset{:});
  else
    error('Unrecognized arguments to patch command');
  end

if (nargout > 0)
    ohandle = h;
end

function [vertices, faces, facevertexcdata]=parse_input( xdata, ydata, zdata, cdata )
	if( size(xdata) ~= size(ydata) | size(xdata) ~= size(zdata) | length(size(xdata))>2 )
		error('Data size mismatch');
	end
	[nx ny]=size(xdata);
	sz = size(cdata);
	if( ~( (sz(1)==1) | (sz(1)==1 & sz(2)==nx) | (sz(1)==ny & sz(2)==nx) ) )
		error('CData size mismatch');
	end
	vertices=[xdata(:) ydata(:) zdata(:)];
	faces = reshape( 1:nx*ny, size(xdata) )';
	facevertexcdata = cdata; %cdata(:);
