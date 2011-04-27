% SUB2IND Convert Multiple Indexing To Linear Indexing
% 
% Usage
% 
% The sub2ind function converts a multi-dimensional indexing expression
% into a linear (or vector) indexing expression.  The syntax for its use
% is
% 
%    y = sub2ind(sizevec,d1,d2,...,dn)
% 
% where sizevec is the size of the array being indexed into, and each
% di is a vector of the same length, containing index values.  The basic
% idea behind sub2ind is that it makes
% 
%   [z(d1(1),d2(1),...,dn(1)),...,z(d1(n),d2(n),...,dn(n))]
% 
% equivalent to
% 
%   z(sub2ind(size(z),d1,d2,...,dn))
% 
% where the later form is using vector indexing, and the former one is using
% native, multi-dimensional indexing.

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function n = sub2ind(sizevec,varargin)
  if (length(varargin) == 0)
    n = [];
    return;
  end
  % Make sure sizevec is large enough
  sizevec = sizevec(:)';
  if (length(sizevec) < length(varargin))
    sizevec = [sizevec,ones(1,length(varargin)-length(sizevec))];
  end
  % Test each indexing component - make sure they are the
  % same length, and that all are in the correct range
  indvecs = {};
  nomlength = length(varargin{1}(:));
  for i=1:length(varargin)
    indvecs{i} = varargin{i}(:);
    if (length(indvecs{i}) ~= nomlength)
      error 'all indexing arguments to sub2ind must be the same length'
    end
    if (min(indvecs{i}) < 1)
      error 'indexing arguments are out of range of an array of the given size'
    end
  end
  % Everything is OK, so combine these into a single indexing vector
  outvec = zeros(nomlength,1);
  slicesize = 1;
  for i=1:length(varargin)
    outvec = outvec + slicesize*(indvecs{i}-1);
    slicesize = slicesize*sizevec(i);
  end
  n = (outvec + 1)';
  n = reshape(n,size(varargin{1}));

