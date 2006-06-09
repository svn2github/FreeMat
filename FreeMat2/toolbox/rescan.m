%!
%@Module RESCAN Rescan M Files for Changes
%@@Section FREEMAT
%@@Usage
%Usually, FreeMat will automatically determine when M Files have
%changed, and pick up changes you have made to M files.  Sometimes,
%you have to force a refresh.  Use the @|rescan| command for this
%purpose.  The syntax for its use is 
%@[
%  rescan
%@]
%!

% Copyright (c) 2002-2006 Samit Basu

function rescan
  cd(pwd);
