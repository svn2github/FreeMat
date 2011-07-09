function gen_wbtest_linspace_3(verbose)
  myloc=fileparts(which('wbtest_linspace_3'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  error_refs = 0;
  y1 = []; y1_refs = {};
  try
    y1=linspace(1,91,91);
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  end
  save reference/wbtest_linspace_3_ref.mat error_refs  y1_refs 
end
