function gen_wbtest_sprintf_4(verbose)
  myloc=fileparts(which('wbtest_sprintf_4'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  error_refs = 0;
  y1 = []; y1_refs = {};
  try
    y1=sprintf('%d aa ',[5 6; 7 8]);
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  end
  save reference/wbtest_sprintf_4_ref.mat error_refs  y1_refs 
end
