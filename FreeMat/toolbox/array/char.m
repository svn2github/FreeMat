% CHAR CHAR Convert to character array or string
% 
% Usage
% 
% The char function can be used to convert an array
% into a string.  It has several forms.  The first form
% is
% 
%    y = char(x)
% 
% where x is a numeric array containing character codes.
% FreeMat does not currently support Unicode, so the
% character codes must be in the range of [0,255].  The
% output is a string of the same size as x.  A second
% form is
% 
%    y = char(c)
% 
% where c is a cell array of strings, creates a matrix string
% where each row contains a string from the corresponding cell array.
% The third form is
% 
%    y = char(s1, s2, s3, ...)
% 
% where si are a character arrays.  The result is a matrix string
% where each row contains a string from the corresponding argument.

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = char(varargin)
  if ((nargin == 1) && ~iscell(varargin{1}))
    y = string(varargin{1});
  elseif ((nargin == 1))
    y = char_block(varargin{1});
  else
    y = char_block(varargin);
  end

      
function y = char_block(x)
  maxwidth = 0;
  for (i=1:numel(x))
    p = char_flatten(string(x{i}));
    x{i} = p;
    maxwidth = max(maxwidth,size(p,2));
  end;
  y = [];
  for (i=1:numel(x))
    k = x{i}; p = size(k,2);
    y = [y;[k,' '+zeros(max(1,size(k,1)),maxwidth-p)]];
  end
  
function y = char_flatten(x)
  y = reshape(x,size(x,1),numel(x)/max(1,size(x,1)));
  