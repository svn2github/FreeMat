function gen_wbtest_ipermute_2(verbose)
  myloc=fileparts(which('wbtest_ipermute_2'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  n_ = numel(wbinputs);
  error_refs = zeros(n_,1);
  y1_refs = cell(n_,1);
  for loopi=1:n_
    x1 = wbinputs{loopi};
    error_refs(loopi) = 0;
    y1 = [];
    try
      y1=size(ipermute(permute(x1),[3,4,2,1]),[3,4,2,1]);
    catch
      error_refs(loopi) = 1;
    end
    if (~error_refs(loopi))
     y1_refs(loopi) = {y1};
    end
  end
  save reference/wbtest_ipermute_2_ref.mat error_refs  y1_refs 
end
