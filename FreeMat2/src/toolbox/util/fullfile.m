%!
%@Module FULLFILE Build a Full Filename From Pieces
%@@Section OS
%@@Usage
%The @|fullfile| routine constructs a full filename from a set of
%pieces, namely, directory names and a filename.  The syntax is:
%@[
%  x = fullfile(dir1,dir2,...,dirn,filename)
%@]
%where each of the arguments are strings.  The @|fullfile| function
%is equivalent to @|[dir1 dirsep dir2 dirsep ... dirn dirsep filename]|.
%@@Example
%@<
%fullfile('path','to','my','file.m')
%@>
%!
function x = fullfile(varargin)
    if (nargin == 0)
        x = [];
        return;
    end
    x = [];
    for i=1:(nargin-1)
        x = [x,varargin{i},dirsep];
    end
    x = [x,varargin{end}];
    