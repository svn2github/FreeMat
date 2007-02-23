function p = htmlwriter(a)
  if (nargin == 0)
    p.myfile = -1;
    p.sectables = {};
    p.eqnlist = {};
    p.verbatim = false;
    p.modulename = '';
    p.groupname = '';
    p.ignore = false;
    p = class(p,'htmlwriter');
  elseif isa(a,'htmlwriter');
    p = a;
  else
    error('copy constructor not defined for htmlwriter');
  end
    
