% CSVWRITE Write Comma Separated Value (CSV) File
% 
% Usage
% 
% The csvwrite function writes a given matrix to a text
% file using comma separated value (CSV) notation.  Note that
% you can create CSV files with arbitrary sized matrices, but
% that csvread has limits on line length.  If you need to
% reliably read and write large matrices, use rawwrite and
% rawread respectively.  The syntax for csvwrite is 
% 
%    csvwrite('filename',x)
% 
% where x is a numeric array.  The contents of x are written
% to filename as comma-separated values.  You can also specify
% a row and column offset to csvwrite to force csvwrite to
% write the matrix x starting at the specified location in the 
% file.  This syntax of the function is
% 
%    csvwrite('filename',x,startrow,startcol)
% 
% where startrow and startcol are the offsets in zero-based
% indexing.  

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function csvwrite(filename,x,firstrow,firstcol)
    if (nargin == 2)
        firstrow = 0;
        firstcol = 0;
    elseif (nargin == 3)
        firstcol = 0;
    elseif (nargin < 2)
        error('csvwrite expects a matrix to write to the CSV file');
    end
    if (ndims(x) ~= 2)
        error('csvwrite cannot write multidimensional arrays');
    end
    if (~isnumeric(x))
        error('csvwrite cannot write non-numerical arrays');
    end
    if (isreal(x))
      csvwrite_real(filename,x,firstrow,firstcol);
    else
      csvwrite_complex(filename,x,firstrow,firstcol);
    end
    
function csvwrite_real(filename,x,firstrow,firstcol)
    fp = fopen(filename,'w');
    for i=0:(firstrow-1)
        fprintf(fp,'\n');
    end
    if (firstcol > 0)
        prepend_string = repmat(',',[1,(firstcol-1)]);
    else
        prepend_string = '';
    end
    for j=1:size(x,1)
        fprintf(fp,prepend_string);
        for k=1:(size(x,2)-1)
            fprintf(fp,'%.20g',x(j,k));
            fprintf(fp,',');
        end
        fprintf(fp,'%.20g',x(j,end));
        fprintf(fp,'\n');
    end
    fclose(fp);
    
function csvwrite_complex(filename,x,firstrow,firstcol)
    fp = fopen(filename,'w');
    for i=0:(firstrow-1)
        fprintf(fp,'\n');
    end
    if (firstcol > 0)
        prepend_string = repmat(',',[1,(firstcol-1)]);
    else
        prepend_string = '';
    end
    for j=1:size(x,1)
        fprintf(fp,prepend_string);
        for k=1:(size(x,2)-1)
            fprintf(fp,['%.20g','+','%.20g','i'],real(x(j,k)),imag(x(j,k)));
            fprintf(fp,',');
        end
        fprintf(fp,['%.20g','+','%.20g','i'],real(x(j,end)),imag(x(j,end)));
        fprintf(fp,'\n');
    end
    fclose(fp);
    
