%!
%@Module CSVREAD Read Comma Separated Value (CSV) File
%@@Section IO
%@@Usage
%The @|csvread| function reads a text file containing comma
%separated values (CSV), and returns the resulting numeric
%matrix (2D).  The function supports multiple syntaxes.  The
%first syntax for @|csvread| is 
%@[
%   x = csvread('filename')
%@]
%which attempts to read the entire CSV file into array @|x|.
%The file can contain only numeric values.  Each entry in the
%file should be separated from other entries by a comma.  However,
%FreeMat will attempt to make sense of the entries if the comma
%is missing (e.g., a space separated file will also parse correctly).
%For complex values, you must be careful with the spaces).  The second
%form of @|csvread| allows you to specify the first row and column 
%(zero-based index)
%@[
%  x = csvread('filename',firstrow,firstcol)
%@]
%The last form allows you to specify the range to read also.  This form
%is
%@[
%  x = csvread('filename',firstrow,firstcol,readrange)
%@]
%where @|readrange| is either a 4-vector of the form @|[R1,C1,R2,C2]|,
%where @|R1,C1| is the first row and column to use, and @|R2,C2| is the
%last row and column to use.  You can also specify the @|readrange| as
%a spreadsheet range @|B12..C34|, in which case the index for the
%range is 1-based (as in a typical spreadsheet), so that @|A1| is the
%first cell in the upper left corner. Note also that @|csvread| is
%somewhat limited.  The number of columns in the file cannot exceed
%65535.  If it does, bad things will happen.
%@@Example
%Here is an example of a CSV file that we wish to read in
%@{  sample_data.csv
%10, 12, 13, 00, 45, 16
%09, 11, 52, 93, 05, 06
%01, 03, 04, 04, 90, -3
%14, 17, 13, 67, 30, 43
%21, 33, 14, 44, 01, 00
%@}
%We start by reading the entire file
%csvread('sample_data.csv')
%Next, we read everything starting with the second row, and third column
%csvread('sample_data.csv',1,2)
%Finally, we specify that we only want the @|3 x 3| submatrix starting
%with the second row, and third column
%csvread('sample_data.csv',1,2,[1,2,3,4])
%!
function x = csvread(filename,firstrow,firstcol,range)
%A good test file:
%10, 12, 13, 00, 45, 16
%09, 11, 52, 93, 05, 06
%
%01, 03, 04, 04, 90, -3
%14, 17, 13, 67, 30, 43, 55, 43, 77
%21, 33, 14, 44, 01, 00
%,,4,5,6,7,8,9
    if (nargin == 1)
        x = csvread_range(filename,0,0,inf,inf);
    elseif (nargin == 3)
        x = csvread_range(filename,firstrow,firstcol,inf,inf);
    elseif (nargin == 4)
        if (~isstr(range))
            if ((firstrow ~= range(1)) || (firstcol ~= range(2)))
                error('csvread requires that the range vector be a 4-vector or a  string ');
            end
            x = csvread_range(filename,range(1),range(2),range(3),range(4));
        else
            v = decode_ss_range(range);
            x = csvread_range(filename,v(1),v(2),v(3),v(4));
        end
    end

function x = csvread_range(filename,firstrow,firstcol,lastrow,lastcol)
    fp = fopen(filename,'r');
    % skip through the file
    i = 0;
    data = {};
    while ((i < lastrow) && ~feof(fp))
        line = fgetline(fp);
        if (~feof(fp) && (i >= firstrow) && (i <= lastrow))
            line = strtrim(line);
            %Replace blank entries with zeros
            line = strrep(strrep(line,',,',',0,'),',,',',0,');
            if (isempty(line))
                line = ' ';
            end
            % Check for the last character being a comma
            if (line(end) == ',')
                line = [line '0'];
            end
            % Check for the first character being a comma
            if (line(1) == ',')
                line = ['0' line];
            end
            % Try to parse this into a variable
            A = eval(['[' line ']'],'nan');
            if (isnan(A))
                error(sprintf(['Line %d of file %s is not a legal CSV ' ...
                               'entry'],i+1,filename));
            end
            data = [data,{A}];
        end
        i = i + 1;
    end
    % if lastcol is infinite, we have to zero extend them to all
    % have the same number of columns
    if (isinf(lastcol))
        maxcols = 0;
        for j=1:numel(data)
            maxcols = max(maxcols,numel(data{j}));
        end
        if ((numel(data) > 0) && (maxcols - firstcol > 0))
            x = zeros(numel(data),maxcols - (firstcol+1) + 1,'int8');
            for j=1:numel(data)
                tmp = [data{j},zeros(1,maxcols-numel(data{j}),'int8')];
                x(j,:) = tmp((firstcol+1):maxcols);
            end
        else
            x = [];
        end
    else
        % the lastcol is finite... we must zero extend them to match
        % lastcol (if necessary)
        if ((numel(data) > 0) && (lastcol > firstcol))
            x = zeros(numel(data),lastcol-firstcol+1,'int8');
            for j=1:numel(data)
                tmp = data{j};
                if (numel(tmp) < lastcol+1)
                    tmp = [tmp,zeros(1,lastcol+1-numel(tmp),'int8')];
                end
                x(j,:) = tmp((firstcol+1):(lastcol+1));
            end
        else
            x = [];
        end
    end
    fclose(fp);
    
function v = decode_ss_range(range)
    % find the ".."
    ndx = strfind(range,'..');
    if (numel(ndx) ~= 1)
        error(['spreadsheet range for csvread is illegal - it must be of ' ...
               'spreadsheet index form, e.g. A4..B9']);
    end
    first_part = range(1:(ndx-1));
    second_part = range((ndx+2):end);
    v = [decode_ss_coord(first_part),decode_ss_coord(second_part)];
    
function v = decode_ss_coord(s)
    % Extract the trailing digits
    numndx = max(find(~isdigit(s)));
    num = s((numndx+1):end);
    % Extract the leading digits
    letndx = min(find(~isalpha(s)));
    let = s(1:(letndx-1));
    if ((numndx+1) ~= letndx)
        error(['Malformed spreadsheet index - index must be of the form ' ...
               '(letters)(number), e.g., AA42'])
    end
    rowindex = str2num(num);
    % To convert the letters, we first convert to uppercase
    let(let>90) = let(let>90) - 32;
    % Then convert to base-26
    let = let - 64;
    % Then convert to decimal
    let = let(:)';
    n = numel(let);
    bse = 26 .^ ((n-1):-1:0);
    colindex = sum(let.*bse);
    % Convert rowindex and colindex to base 0
    rowindex = rowindex - 1;
    colindex = colindex - 1;
    v = [rowindex,colindex];
        
