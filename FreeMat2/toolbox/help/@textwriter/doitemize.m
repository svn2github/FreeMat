function doitemize(&p,enums)
  for i=1:numel(enums)
    fprintf(p.myfile,'  - %s\n',expand_codes(enums{i}));
  end
  