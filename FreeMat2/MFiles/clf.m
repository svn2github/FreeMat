%!
%@Module CLF Clear Figure
%@@Section PLOT
%@@Usage
%This function clears the contents of the current figure.  The
%syntax for its use is
%@[
%   clf
%@]
%!
function clf
set(gcf,'children',[]);
set(gcf,'currentaxes',0);

