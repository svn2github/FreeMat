% SUBSREF Overloaded Class Indexing
% 
% Usage
% 
% This method is called for expressions of the form
% 
%   c = a(b), c = a{b}, c = a.b
% 
% and overloading the subsref method allows you
% to define the meaning of these expressions for
% objects of class a.  These expressions are
% mapped to a call of the form
% 
%   b = subsref(a,s)
% 
% where s is a structure array with two fields. The
% first field is
%   -  type  is a string containing either '()' or
%  '{}' or '.' depending on the form of the call.
% 
%   -  subs is a cell array or string containing the
%  the subscript information.
% 
% When multiple indexing experssions are combined together
% such as b = a(5).foo{:}, the s array contains
% the following entries
% 
%   s(1).type = '()'  s(1).subs = {5}
%   s(2).type = '.'   s(2).subs = 'foo'
%   s(3).type = '{}'  s(3).subs = ':'
% 

% Copyright (c) 2007 Samit Basu
% Licensed under the GPL
function b = subsref(a,s)
  for i=1:numel(s)
    switch (s(i).type)
    case '()'
      a = a(s(i).subs{:});
    case '{}'
      a = a{s(i).subs{:}};
    case '.'
      a = a.(s(i).subs);
    otherwise
      error('illegal indexing structure argument to subsref');
    end
  end
  b = a;


