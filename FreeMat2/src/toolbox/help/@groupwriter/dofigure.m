function dofigure(&p,figname)
  for i = 1:numel(p.clients)
    dofigure(p.clients{i},figname);
  end
