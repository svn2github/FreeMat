%!
%@Module ISHOLD Test Hold Status
%@@Section HANDLE
%@@Usage
%Returns the state of the @|hold| flag on the currently active
%plot.  The general syntax for its use is
%@[
%   ishold
%@]
%and it returns a logical 1 if @|hold| is @|on|, and a logical
%0 otherwise.
%!

% Copyright (c) 2002-2006 Samit Basu

function k = ishold
k = strcomp(get(gca,'nextplot'),'add');
