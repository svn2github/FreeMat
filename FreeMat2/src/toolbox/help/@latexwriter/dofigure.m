function dofigure(&p,figname)
  fprintf(p.myfile,'\n\n');
  fprintf(p.myfile,'\\centerline{\\includegraphics[width=8cm]{%s}}\n\n', ...
          figname);
  