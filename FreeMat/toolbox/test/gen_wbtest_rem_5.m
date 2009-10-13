function gen_wbtest_rem_5(verbose)
  myloc=fileparts(which('wbtest_rem_5'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  error_refs = 0;
  y1 = []; y1_refs = {};
  try
    y1=rem([9 3 2 0],[1 0 2 2]);
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  end
  save reference/wbtest_rem_5_ref.mat error_refs  y1_refs 
end
