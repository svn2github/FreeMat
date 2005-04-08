function y = derived2(a)
  if (nargin == 0)
    y.extra = pi;
    y.goog = 53;
    y = class(y,'derived2',base);
  elseif (strcmp(class(a),'derived2'))
    y = a;
  else
    y = class(a,'derived2',base);
  end
