%!
%@Module MESHGRID Generate Grid Mesh For Plots
%@@Section ARRAY
%@@Usage
%The @|meshgrid| function generates arrays that can be used for the
%generation of surface plots.  The syntax is one of 
%@[
%  [X,Y] = meshgrid(x)
%  [X,Y] = meshgrid(x,y)
%  [X,Y,Z] = meshgrid(x,y,z)
%@]
%where @|x,y,z| are vectors, and @|X,Y,Z| are matrices.  In the first
%case @|[X,Y] = meshgrid(x)|, the rows of @|X| and the columns of @|Y|
%contain copies of the vector @|x|.  In the second case 
%@|[X,Y] = meshgrid(x,y)|, the rows of @|X| contain copies of @|x|, and
%the columns of @|Y| contain copies of @|y|.  In the third case, each
%input is copied along the row, column or slice direction of the
%corresponding output variable.
%@@Example
%In the first example:
%@<
%[X,Y] = meshgrid(-2:.4:2)
%@>
%Next, we use different vectors for @|X| and for @|Y|:
%@<
%[X,Y] = meshgrid([1,2,3,4],[6,7,8])
%@>
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function [X,Y,Z] = meshgrid(x,y,z)
  if (nargin == 0)
    error('meshgrid requires at least one input argument');
  end
  if (nargin == 1)
    X = repmat(x(:)',[numel(x),1]);
    Y = X';
  elseif (nargout == 2)
    X = repmat(x(:)',[numel(y),1]);
    Y = repmat(y(:),[1,numel(x)]);
  else
    X = repmat(x(:)',[numel(y),1,numel(z)]);
    Y = repmat(y(:),[1,numel(x),numel(z)]);
    Z = repmat(reshape(z,[1,1,numel(z)]),[numel(y),numel(x),1]);
  end
  
  
    
    
