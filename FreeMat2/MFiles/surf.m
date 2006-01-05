function h = surf(X,Y,Z,C)
  if (nargin == 3)
    h = surface('xdata',X,'ydata',Y,'zdata',Z);
  else
    h = surface('xdata',X,'ydata',Y,'zdata',Z,'cdata',C);
  end

