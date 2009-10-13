% SYMVAR SYMVAR Find Symbolic Variables in an Expression
% 
% Usage
% 
% Finds the symbolic variables in an expression.  The syntax for its
% use is 
% 
%   syms = symvar(expr)
% 
% where expr is a string containing an expression, such as
% 'x^2 + cos(t+alpha)'.  The result is a cell array of strings
% containing the non-function identifiers in the expression.  Because
% they are usually not used as identifiers in expressions, the strings
%  'pi','inf','nan','eps','i','j' are ignored.

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function syms = symvar(expr)
  if (isempty(expr))
    syms = cell(0,0);
    return;
  end
  if (~isstr(expr))
    syms = cell(0,1);
    return;
  end
  ignore = {'pi','inf','nan','eps','i','j'};
  tsyms = regexp(char(expr),'(\b[a-zA-Z]\w*\b)(?!\s*\()','tokens');
  tsyms = unique([tsyms{:}]);
  syms = cell(0,1);
  for i = 1:numel(tsyms)
    if (~any(strcmp(tsyms{i},ignore)))
      syms = [syms;{tsyms{i}}];
    end
  end
  
  
