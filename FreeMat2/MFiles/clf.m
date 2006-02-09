%!
%@Module CLF Clear Figure
%@@Section HANDLE
%@@Usage
%This function clears the contents of the current figure.  The
%syntax for its use is
%@[
%   clf
%@]
%!

% Copyright (c) 2002-2006 Samit Basu

function clf
set(gcf,'children',[]);
set(gcf,'currentaxes',0);

