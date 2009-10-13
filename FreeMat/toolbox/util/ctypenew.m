% CTYPENEW CTYPENEW Create New Instance of C Structure
% 
% Usage
% 
% The ctypenew function is a convenience function for
% creating a FreeMat structure that corresponds to a C
% structure.  The entire structure is initialized with zeros.
% This has some negative implications, because if the 
% structure definition uses cenums, they may come out
% as 'unknown' values if there are no enumerations corresponding
% to zero.  The use of the function is
% 
%    a = ctypenew('typename')
% 
% which creates a single structure of C structure type 'typename'.
% To create an array of structures, we can provide a second argument
% 
%    a = ctypenew('typename',count)
% 
% where count is the number of elements in the structure array.

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function a = ctypenew(typename,count)
  if (nargin == 1)
    count = 1;
  end
  if (nargin == 0)
    error('ctypenew requires a typename argument');
  end
  a = ctypethaw(zeros(1,ctypesize(typename,count),'uint8'),typename,count);
