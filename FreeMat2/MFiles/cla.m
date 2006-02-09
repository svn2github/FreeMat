%!
%@Module CLA Clear Current Axis
%@@Section HANDLE
%@@Usage
%Clears the current axes.  The syntax for its use is
%@[
%  cla
%@]
%!

% Copyright (c) 2002-2006 Samit Basu

function cla
set(gca,'children',[]);
