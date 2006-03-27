%!
%@Module XLIM Adjust X Limits of plot
%@@Section HANDLE
%@@Usage
%There are several ways to use @|xlim| to adjust the x limits of
%a plot.  The y-axis retains its current limits.  The four
%syntaxes are
%@[
%   xlim([lo,hi])   
%   xlim(lo,hi)
%   xlim('auto')
%   xlim auto
%@]
%where in the first two forms, the new x-limits on the plot are
%@|[lo,hi]|.  In the second two forms, the axes limits are 
%automatically selected by FreeMat.
%@@Example
%Here is an example of using @|xlim| to zoom in on the x axis of a
%plot without changing the y limits.  First, the plot with default
%limits
%@<
%x = linspace(-1,1);
%y = sin(2*pi*x);
%plot(x,y,'r-');
%mprint xlim1
%@>
%which results in
%@figure xlim1
%Next, we zoom in on the plot using the @|xlim| function
%@<
%plot(x,y,'r-')
%xlim(-0.2,0.2)
%mprint xlim2
%@>
%which results in
%@figure xlim2
%!

% Copyright (c) 2002-2006 Samit Basu

%Copyright (c) 2004,2005 Brian Yanoff, Samit Basu
function xlim(lim1, lim2)
  if isa(lim1,'string') && strcmp(lim1,'auto')
      set(gca,'xlimmode','auto');
      return;
  elseif isa(lim1,'string')
      error('do not understand arguments to xlim function');
  end

  if prod(size(lim1))==2
    lo_lim = lim1(1);
    hi_lim = lim1(2);
  elseif ~isset('lim2')
    error('xlim requires a 2-vector or two scalar parameters');
  elseif prod(size(lim1))==1
    lo_lim = lim1;
    hi_lim = lim2(1);
  end
  set(gca,'xlim',[lo_lim,hi_lim]);
