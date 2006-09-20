%!
%@Module ADDPATH Add 
%@@Section FREEMAT
%@@Usage
%The @|addpath| routine adds a set of directories to the current path.
%The first form takes a single directory and adds it to the beginning
%or top of the path:
%@[
%  addpath('directory')
%@]
%The second form add several directories to the top of the path:
%@[
%  addpath('dir1','dir2',...,'dirn')
%@]
%Finally, you can provide a flag to control where the directories get
%added to the path
%@[
%  addpath('dir1','dir2',...,'dirn','-flag')
%@]
%where if @|flag| is either @|'-0'| or @|'-begin'|, the directories are
%added to the top of the path, and if the @|flag| is either @|'-1'| or 
%@|'-end'| the directories are added to the bottom (or end) of the path.
%!
function addpath(varargin)
    if (nargin == 0) return; end
    atbegin = 1;
    if (any(strcmp(varargin{end},{'-0','-begin'})))
        varargin(end) = [];
    elseif (any(strcmp(varargin{end},{'-1','-end'})))
        atbegin = 0;
        varargin(end) = [];
    end
    if (atbegin)
        g = [];
        for i=1:numel(varargin)
            g = [g,varargin{i},pathsep];
        end
        g = [g,getpath];
        setpath(g);
    else
        g = getpath;
        for i=1:numel(varargin)
            g = [g,pathsep,varargin{i}];
        end
        setpath(g);
    end
