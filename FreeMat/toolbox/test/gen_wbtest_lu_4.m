function gen_wbtest_lu_4(verbose)
  myloc=fileparts(which('wbtest_lu_4'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  n_ = numel(wbinputs);
  error_refs = zeros(n_,1);
  y1_refs = cell(n_,1);
  y2_refs = cell(n_,1);
  y3_refs = cell(n_,1);
  y4_refs = cell(n_,1);
  y5_refs = cell(n_,1);
  for loopi=1:n_
    x1 = wbinputs{loopi};
    error_refs(loopi) = 0;
    y1 = [];
    y2 = [];
    y3 = [];
    y4 = [];
    y5 = [];
    try
      [y1,y2,y3,y4,y5]=lu(x1);
    catch
      error_refs(loopi) = 1;
    end
    if (~error_refs(loopi))
     y1_refs(loopi) = {y1};
     y2_refs(loopi) = {y2};
     y3_refs(loopi) = {y3};
     y4_refs(loopi) = {y4};
     y5_refs(loopi) = {y5};
    end
  end
  save reference/wbtest_lu_4_ref.mat error_refs  y1_refs   y2_refs   y3_refs   y4_refs   y5_refs 
end
