%
% function x = rawread(fname,size,precision)
% 
% Reads a raw array from a file.
%
%   - fname is the name of the file to read from
%   - size is an n-dimensional vector that stores the
%     size of the array in each dimension
%   - precision is the type of the data to read in:
%     'uint8','uchar','unsigned char' for unsigned, 8-bit integers
%     'int8','char','integer*1' for signed, 8-bit integers
%     'uint16','unsigned short' for unsigned, 16-bit  integers
%     'int16','short','integer*2' for  signed, 16-bit integers
%     'uint32','unsigned int' for unsigned, 32-bit integers
%     'int32','int','integer*4' for signed, 32-bit integers
%     'single','float32','float','real*4' for 32-bit floating point
%     'double','float64','real*8' for 64-bit floating point
%     'complex','complex*8' for  64-bit complex floating point (32 bits 
%          for the real and imaginary part).
%     'dcomplex','complex*16' for 128-bit complex floating point (64
%          bits for the real and imaginary part).
%
% As a special feature, one of the size elements can be 'inf', 
% in which case, the largest possible array is read in.

% Copyright (c) 2002, 2003 Samit Basu
%
% Permission is hereby granted, free of charge, to any person obtaining a 
% copy of this software and associated documentation files (the "Software"), 
% to deal in the Software without restriction, including without limitation 
% the rights to use, copy, modify, merge, publish, distribute, sublicense, 
% and/or sell copies of the Software, and to permit persons to whom the 
% Software is furnished to do so, subject to the following conditions:
%
% The above copyright notice and this permission notice shall be included 
% in all copies or substantial portions of the Software.
%
% THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
% OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
% FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
% THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
% LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
% FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
% DEALINGS IN THE SOFTWARE.
function x = rawread(fname,size,precision)
  fp = fopen(fname,'rb');
  x = fread(fp,size,precision);
  fclose(fp);
