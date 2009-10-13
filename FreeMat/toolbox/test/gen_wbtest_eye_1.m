function gen_wbtest_eye_1(verbose)
  myloc=fileparts(which('wbtest_eye_1'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  error_refs = 0;
  y1 = []; y1_refs = {};
  try
    y1=eye(3);
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  end
  save reference/wbtest_eye_1_ref.mat error_refs  y1_refs 
end
