function endmodule(&p)
  fclose(p.myfile);
  p.myfile = -1;
