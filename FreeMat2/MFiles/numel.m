
function len = numel(x,varargin)
if (nargin==1)
  len = prod(size(x));
else
  len = prod(size(x(varargin{:})));
end
