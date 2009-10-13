function gen_wbtest_issquare_2(verbose)
  myloc=fileparts(which('wbtest_issquare_2'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  error_refs = 0;
  y1 = []; y1_refs = {};
  try
    y1=issquare(rand(4,1,1,1));
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  end
  save reference/wbtest_issquare_2_ref.mat error_refs  y1_refs 
end
