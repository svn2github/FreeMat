function endmodule(&p)
  fprintf(p.myfile,'</BODY>\n');
  fprintf(p.myfile,'</HTML>\n');
  fclose(p.myfile);
  p.myfile = -1;
