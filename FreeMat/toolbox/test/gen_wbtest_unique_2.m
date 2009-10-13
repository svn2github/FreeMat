function gen_wbtest_unique_2(verbose)
  myloc=fileparts(which('wbtest_unique_2'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  n_ = numel(wbinputs);
  error_refs = zeros(n_,1);
  y1_refs = cell(n_,1);
  y2_refs = cell(n_,1);
  y3_refs = cell(n_,1);
  for loopi=1:n_
    x1 = wbinputs{loopi};
    error_refs(loopi) = 0;
    y1 = [];
    y2 = [];
    y3 = [];
    try
      [y1,y2,y3]=unique(x1);
    catch
      error_refs(loopi) = 1;
    end
    if (~error_refs(loopi))
     y1_refs(loopi) = {y1};
     y2_refs(loopi) = {y2};
     y3_refs(loopi) = {y3};
    end
  end
  save reference/wbtest_unique_2_ref.mat error_refs  y1_refs   y2_refs   y3_refs 
end
