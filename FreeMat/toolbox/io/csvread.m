% CSVREAD CSVREAD Read Comma Separated Value (CSV) File
% 
% Usage
% 
% The csvread function reads a text file containing comma
% separated values (CSV), and returns the resulting numeric
% matrix (2D).  The function supports multiple syntaxes.  The
% first syntax for csvread is 
% 
%    x = csvread('filename')
% 
% which attempts to read the entire CSV file into array x.
% The file can contain only numeric values.  Each entry in the
% file should be separated from other entries by a comma.  However,
% FreeMat will attempt to make sense of the entries if the comma
% is missing (e.g., a space separated file will also parse correctly).
% For complex values, you must be careful with the spaces).  The second
% form of csvread allows you to specify the first row and column 
% (zero-based index)
% 
%   x = csvread('filename',firstrow,firstcol)
% 
% The last form allows you to specify the range to read also.  This form
% is
% 
%   x = csvread('filename',firstrow,firstcol,readrange)
% 
% where readrange is either a 4-vector of the form [R1,C1,R2,C2],
% where R1,C1 is the first row and column to use, and R2,C2 is the
% last row and column to use.  You can also specify the readrange as
% a spreadsheet range B12..C34, in which case the index for the
% range is 1-based (as in a typical spreadsheet), so that A1 is the
% first cell in the upper left corner. Note also that csvread is
% somewhat limited. 

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function x = csvread(filename,firstrow,firstcol,range)
    if (nargin == 1)
	x = dlmread(filename,',');
    elseif (nargin == 3)
        x = dlmread(filename,',',firstrow,firstcol);
    elseif (nargin == 4)
        x = dlmread(filename,',',range);
    end
        
