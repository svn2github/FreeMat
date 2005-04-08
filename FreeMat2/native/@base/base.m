function y = base(a)
  if (nargin == 0)
    y.type = 'base';
    y.color = 'blue';
    y.number = 42;
    y = class(y,'base');
  elseif (strcmp(class(a),'base'))
    y = a;
  else
    y = class(a,'base');
  end