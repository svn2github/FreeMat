function gen_wbtest_strfind_2(verbose)
  myloc=fileparts(which('wbtest_strfind_2'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  n_ = numel(wbinputs);
  error_refs = zeros(n_,1);
  y1_refs = cell(n_,1);
  for loopi=1:n_
    x1 = wbinputs{loopi};
    error_refs(loopi) = 0;
    y1 = [];
    try
      y1 = strfind(x1,'er');
    catch
      error_refs(loopi) = 1;
    end
    if (~error_refs(loopi))
     y1_refs(loopi) = {y1};
    end
  end
  save reference/wbtest_strfind_2_ref.mat error_refs  y1_refs 
end
