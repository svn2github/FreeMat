function endmodule(&p)
  fprintf(p.myfile,'</BODY>\n');
  fprintf(p.myfile,'</HTML>\n');
  fclose(p.myfile);
  p.myfile = -1;
  if (~isempty(p.eqnlist))
    fp = fopen(sprintf('../tmp/%s_eqn.tex',p.modulename),'w');
    fprintf(fp,'\\documentclass{article}\n');
    fprintf(fp,'\\usepackage{amsmath}\n');
    fprintf(fp,'\\pagestyle{empty}\n');
    fprintf(fp,'\\begin{document}\n');
    for i=1:numel(p.eqnlist)
      fprintf(fp,'\\[\n');
      g = p.eqnlist{i};
      if (g(end) == 10) g = g(1:(end-1)); end
      fprintf(fp,'%s',g);
      fprintf(fp,'\\]\n');
      fprintf(fp,'\\pagebreak\n');
    end
    fprintf(fp,'\\end{document}\n');
    fclose(fp);
    cdir = pwd;
    cd('../tmp');
    system(sprintf('latex %s_eqn.tex',p.modulename));
    cd(cdir);
    system(sprintf('dvipng -T tight ../tmp/%s_eqn.dvi',p.modulename));
  end
