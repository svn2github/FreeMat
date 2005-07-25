function label = trimprint(val, scientificflag)
  if (~scientificflag)
    label = sprintf('%f',val);
  else
    label = sprintf('%e',val);
  end
