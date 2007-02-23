function p = groupwriter(a)
  if (nargin == 0)
    p.clients = {};
    p = class(p,'groupwriter');
  elseif isa(a,'groupwriter')
    p = a;
  else
    p.clients = a;
    p = class(p,'groupwriter');
  end
