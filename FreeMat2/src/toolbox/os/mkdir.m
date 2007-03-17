%!
%@Module MKDIR Make Directory
%@@Section OS
%@@Usage
%Creates a directory.  The general syntax for its use is
%@[
%  mkdir('dirname')
%@]
%which creates the directory @|dirname| if it does not exist.  The argument
%@|dirname| can be either a relative path or an absolute path.  For compatibility
%with MATLAB, the following syntax is also allowed
%@[
%  mkdir('parentdir','dirname')
%@]
%which attempts to create a directory @|dirname| in the directory given by @|parentdir|.
%However, this simply calls @|mkdir([parentdir dirsep dirname])|, and if this is not
%the required behavior, please file an enhancement request to have it changed.  Note that
%@|mkdir| returns a logical @|1| if the call succeeded, and a logical @|0| if not.
%!
function status = mkdir(parentdir,dirname)
  if (~exist('dirname'))
    status = mkdir_core(parentdir);
  else
    status = mkdir_core([parentdir dirsep dirname]);
  end
  
  
