%!
%@Module ZLIM Adjust Z Limits of plot
%@@Section HANDLE
%@@Usage
%There are several ways to use @|zlim| to adjust the z limits of
%a plot.  The other axes retain their current limits.  The four
%syntaxes are
%@[
%   zlim([lo,hi])   
%   zlim(lo,hi)
%   zlim('auto')
%   zlim auto
%@]
%where in the first two forms, the new z-limits on the plot are
%@|[lo,hi]|.  In the second two forms, the axes limits are 
%automatically selected by FreeMat.
%!

% Copyright (c) 2002-2006 Samit Basu

function zlim(lim1, lim2)
  if isa(lim1,'string') && strcmp(lim1,'auto')
     set(gca,'zlimmode','auto');
     return;
  elseif isa(lim1,'string')
      error('do not understand arguments to zlim function');
  end

  if prod(size(lim1))>1
    lo_lim = lim1(1);
    hi_lim = lim1(2);
  elseif ~isset('lim2')
    error('zlim requires a 2-vector or two scalar parameters');
  elseif prod(size(lim1))==1
    lo_lim = lim1;
    hi_lim = lim2(1);
  end
  set(gca,'zlim',[lo_lim,hi_lim]);

  
