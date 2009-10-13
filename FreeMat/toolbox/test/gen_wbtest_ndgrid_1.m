function gen_wbtest_ndgrid_1(verbose)
  myloc=fileparts(which('wbtest_ndgrid_1'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  error_refs = 0;
  y1 = []; y1_refs = {};
  y2 = []; y2_refs = {};
  try
    [y1,y2] = ndgrid(1:2,3:5);
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  y2_refs = {y2};
  end
  save reference/wbtest_ndgrid_1_ref.mat error_refs  y1_refs   y2_refs 
end
