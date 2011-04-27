% CLF Clear Figure
% 
% Usage
% 
% This function clears the contents of the current figure.  The
% syntax for its use is
% 
%    clf
% 

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function clf
set(gcf,'children',[]);
set(gcf,'currentaxes',0);

