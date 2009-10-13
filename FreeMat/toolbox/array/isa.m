% ISA ISA Test Type of Variable
% 
% Usage
% 
% Tests the type of a variable.  The syntax for its
% use is
% 
%    y = isa(x,type)
% 
% where x is the variable to test, and type is
% the type.  Supported built-in types are
%   -  'cell' for cell-arrays
% 
%   -  'struct' for structure-arrays
% 
%   -  'logical' for logical arrays
% 
%   -  'uint8' for unsigned 8-bit integers
% 
%   -  'int8' for signed 8-bit integers
% 
%   -  'uint16' for unsigned 16-bit integers
% 
%   -  'int16' for signed 16-bit integers
% 
%   -  'uint32' for unsigned 32-bit integers
% 
%   -  'int32' for signed 32-bit integers
% 
%   -  'uint64' for unsigned 64-bit integers
% 
%   -  'int64' for signed 64-bit integers
% 
%   -  'single' for 32-bit floating point numbers
% 
%   -  'double' for 64-bit floating point numbers
% 
%   -  'char' for string arrays
% 
% If the argument is a user-defined type (via the class function), then
% the name of that class is returned.

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function y = isa(x,type)
  if (~strcmp(typeof(type),'char'))
    error('type argument to isa must be a string');
  end
  y = strcmp(class(x),type);
