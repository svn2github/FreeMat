% CAST Typecast Variable to Specified Type
% 
% Usage
% 
% The cast function allows you to typecast a variable from one
% type to another.  The syntax for its use is 
% 
%     y = cast(x,toclass)
% 
% where toclass is the name of the class to cast x to.  Note
% that the typecast must make sense, and that toclass must be
% one of the builtin types.  The current list of supported types is
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
%   -  'float' for 32-bit floating point numbers
% 
%   -  'single' is a synonym for 'float'
% 
%   -  'double' for 64-bit floating point numbers
% 
%   -  'char' for string arrays
% 

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = cast(a,toclass)
    switch(toclass)
      case 'cell'
        y = cell(a);
      case 'struct'
        y = struct(a);
      case 'logical'
        y = logical(a);
      case 'uint8'
        y = uint8(a);
      case 'int8'
        y = int8(a);
      case 'uint16'
        y = uint16(a);
      case 'int16'
        y = int16(a);
      case 'uint32'
        y = uint32(a);
      case 'int32'
        y = int32(a);
      case 'uint64'
        y = uint64(a);
      case 'int64'
        y = int64(a);
      case {'float','single'}
        y = float(a);
      case 'double'
        y = double(a);
      case 'char'
        y = char(a);
      otherwise
        error(['Unsupported class ' toclass ' requested in typecast'])
    end
    
