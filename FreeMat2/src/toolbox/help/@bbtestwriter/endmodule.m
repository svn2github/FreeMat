function endmodule(&p)
  fclose(p.myfile);
  if (p.empty)
    delete(p.filename);
  end
