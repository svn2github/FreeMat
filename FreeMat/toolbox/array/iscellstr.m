% ISCELLSTR ISCELLSTR Test For Cell Array of Strings
% 
% Usage
% 
% The syntax for iscellstr is 
% 
%    x = iscellstr(y)
% 
% and it returns a logical 1 if the argument is a cell array
% in which every cell is a character array (or is empty), and
% a logical 0 otherwise.

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function x = iscellstr(y)
  if (~iscell(y))
    x = logical(0);
    return;
  end
  m = numel(y);
  all_strings = logical(1);
  i = 1;
  while (all_strings & (i <= m))
    all_strings = all_strings & (isstr(y{i}) | isempty(y{i}));
    i = i + 1;
  end
  x = all_strings;
