function gen_wbtest_fileparts_1(verbose)
  myloc=fileparts(which('wbtest_fileparts_1'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  error_refs = 0;
  y1 = []; y1_refs = {};
  y2 = []; y2_refs = {};
  y3 = []; y3_refs = {};
  y4 = []; y4_refs = {};
  try
    [y1,y2,y3,y4]=fileparts('/home/basu/testfile.mgz');
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  y2_refs = {y2};
  y3_refs = {y3};
  y4_refs = {y4};
  end
  save reference/wbtest_fileparts_1_ref.mat error_refs  y1_refs   y2_refs   y3_refs   y4_refs 
end
