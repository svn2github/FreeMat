function gen_wbtest_conv2_2(verbose)
  myloc=fileparts(which('wbtest_conv2_2'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  n_ = numel(wbinputs);
  error_refs = zeros(n_,n_);
  y1_refs = cell(n_,n_);
  for loopi=1:n_
    for loopj=1:n_
      x1 = wbinputs{loopi};
      x2 = wbinputs{loopj};
      y1 = [];
      try
        y1=conv2(x1,x2,'same');
      catch
        error_refs(loopi,loopj) = 1;
      end
      if (~error_refs(loopi,loopj))
       y1_refs(loopi,loopj) = {y1};
      end
    end
  end
  save reference/wbtest_conv2_2_ref.mat error_refs  y1_refs 
end
