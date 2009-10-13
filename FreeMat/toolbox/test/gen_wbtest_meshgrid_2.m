function gen_wbtest_meshgrid_2(verbose)
  myloc=fileparts(which('wbtest_meshgrid_2'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  error_refs = 0;
  y1 = []; y1_refs = {};
  y2 = []; y2_refs = {};
  try
    [y1,y2] = meshgrid([1,2,3,4],[6,7,8]);
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  y2_refs = {y2};
  end
  save reference/wbtest_meshgrid_2_ref.mat error_refs  y1_refs   y2_refs 
end
