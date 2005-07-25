function style = utilitymaplinestyletotype(spec)
  switch(spec)
   case {'-'}
    style='solid';
   case {':'}
    style='dotted';
   case {';'}
    style='dashdot';
   case {'|'}
    style='dashed';
   case {' '}
    style='none';
   otherwise:
    style='solid';
  end
