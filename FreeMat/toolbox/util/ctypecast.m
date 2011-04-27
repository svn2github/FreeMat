% CTYPECAST Cast FreeMat Structure to C Structure
% 
% Usage
% 
% The ctypecast function is a convenience function for ensuring that
% a FreeMat structure fits the definition of a C struct (as defined
% via ctypedefine.  It does so by encoding the structure
% to a byte array using ctypefreeze and then recovering it using
% the ctypethaw function.  The usage is simply
% 
%    s = ctypecast(s,typename)
% 
% where s is the structure and typename is the name of the
% C structure that describes the desired layout and types for elements
% of s.  This function is equivalent to calling ctypefreeze
% and ctypethaw in succession on a FreeMat structure.

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function s = ctypecast(s,typename)
  if (nargin ~= 2) error('ctypecast requires a struct and a typename'); end
  s = ctypethaw(ctypefreeze(s,typename),typename);
