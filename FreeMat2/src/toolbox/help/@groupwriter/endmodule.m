function endmodule(&p)
  for i=1:numel(p.clients)
    endmodule(p.clients{i});
  end
