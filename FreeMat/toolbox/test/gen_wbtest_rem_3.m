function gen_wbtest_rem_3(verbose)
  myloc=fileparts(which('wbtest_rem_3'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  error_refs = 0;
  y1 = []; y1_refs = {};
  try
    y1=rem(2*pi,pi);
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  end
  save reference/wbtest_rem_3_ref.mat error_refs  y1_refs 
end
