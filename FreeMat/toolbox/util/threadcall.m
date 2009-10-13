% THREADCALL THREADCALL Call Function In A Thread
% 
% Usage
% 
% The threadcall function is a convenience function for executing
% a function call in a thread.  The syntax for its use is
% 
%    [val1,...,valn] = threadcall(threadid,timeout,funcname,arg1,arg2,...)
% 
% where threadid is the ID of the thread (as returned by the
% threadnew function), funcname is the name of the function to call,
% and argi are the arguments to the function, and timeout is the
% amount of time (in milliseconds) that the function is allowed to take.

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function varargout = threadcall(id,timeout,funcname,varargin)
  threadstart(id,funcname,nargout,varargin{:});
  if (~threadwait(id,timeout))
    error(sprintf('Timeout on call to %s',funcname));
  end
  varargout = cell(1,nargout);
  varargout{:} = threadvalue(id);
  
