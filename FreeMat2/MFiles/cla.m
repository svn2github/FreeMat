%!
%@Module CLA Clear Current Axis
%@@Section HANDLE
%@@Usage
%Clears the current axes.  The syntax for its use is
%@[
%  cla
%@]
%!
function cla
set(gca,'children',[]);
