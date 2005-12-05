
%Copyright (c) 2004,2005 Brian Yanoff, Samit Basu
function xlim(lim1, lim2)
  if isa(lim1,'string') && strcmp(lim1,'auto')
      axis_limits = axis;
      axis('auto');
      auto_limits = axis;
      axis_limits(1) = auto_limits(1);
      axis_limits(2) = auto_limits(2);
      axis(axis_limits);
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

  axis_limits = axis;
  axis_limits(1) = lo_lim;
  axis_limits(2) = hi_lim;
  axis(axis_limits);


