function p = testwriter(a)
  if (nargin == 0)
    p.myfile = -1;
    p.sectables = {};
    p.eqnlist = {};
    p.verbatim = false;
    p.modulename = '';
    p.groupname = '';
    p.ignore = false;
    p.section_descriptors = [];
    p.sourcepath = '';
    p.num = 1;
    p = class(p,'testwriter');
  elseif isa(a,'testwriter');
    p = a;
  else
    error('copy constructor not defined for testwriter');
  end
    
