function gen_wbtest_ndgrid_2(verbose)
  myloc=fileparts(which('wbtest_ndgrid_2'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  error_refs = 0;
  y1 = []; y1_refs = {};
  y2 = []; y2_refs = {};
  y3 = []; y3_refs = {};
  try
    [y1,y2,y3] = ndgrid(1:2,3:5,0:1);
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  y2_refs = {y2};
  y3_refs = {y3};
  end
  save reference/wbtest_ndgrid_2_ref.mat error_refs  y1_refs   y2_refs   y3_refs 
end
