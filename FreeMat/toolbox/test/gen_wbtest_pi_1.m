function gen_wbtest_pi_1(verbose)
  myloc=fileparts(which('wbtest_pi_1'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  error_refs = 0;
  y1 = []; y1_refs = {};
  try
    y1=pi;
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  end
  save reference/wbtest_pi_1_ref.mat error_refs  y1_refs 
end
