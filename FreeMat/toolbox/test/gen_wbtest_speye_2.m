function gen_wbtest_speye_2(verbose)
  myloc=fileparts(which('wbtest_speye_2'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  error_refs = 0;
  y1 = []; y1_refs = {};
  try
    z=speye(5000);y1=full(z(1:10,1:10));
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  end
  save reference/wbtest_speye_2_ref.mat error_refs  y1_refs 
end
