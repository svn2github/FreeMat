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
set(gca,'xlimmode','auto');
set(gca,'ylimmode','auto');
set(gca,'zlimmode','auto');
set(gca,'xscale','linear');
set(gca,'yscale','linear');
set(gca,'zscale','linear');
set(gca,'xtickmode','auto');
set(gca,'ytickmode','auto');
set(gca,'ztickmode','auto');
set(gca,'xticklabelmode','auto');
set(gca,'yticklabelmode','auto');
set(gca,'zticklabelmode','auto');
