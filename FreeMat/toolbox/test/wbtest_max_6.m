function test_val = wbtest_max_6(verbose)
  myloc=fileparts(which('wbtest_max_6'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  load([myloc,dirsep,'reference',dirsep,'wbtest_max_6_ref.mat'])
  fail_count = 0;
  for loopi=1:numel(wbinputs)
    for loopj=1:numel(wbinputs)
      x1 = wbinputs{loopi};
      x2 = wbinputs{loopj};
      error_flag = 0;
     y1 = [];
     y2 = [];
      try
        [y1,y2]=max(x1,x2);
      catch
        error_flag = 1;
      end
    if (error_flag && ~error_refs(loopi,loopj) && ~(false))
       printf('Mismatch Errors: input %d, %d [y1,y2]=max(x1,x2)\n',loopi,loopj);
        fail_count = fail_count + 1;
  elseif (~error_flag && ~error_refs(loopi,loopj) && ~wbtest_exact(y1,y1_refs{loopi,loopj}) && ~(false))
    printf('Mismatch (exact): input %d,%d output 1 [y1,y2]=max(x1,x2)\n',loopi,loopj);
    fail_count = fail_count + 1;
  elseif (~error_flag && ~error_refs(loopi,loopj) && ~wbtest_exact(y2,y2_refs{loopi,loopj}) && ~(false))
    printf('Mismatch (exact): input %d,%d output 2 [y1,y2]=max(x1,x2)\n',loopi,loopj);
    fail_count = fail_count + 1;
    end
  end
  test_val = (fail_count == 0);
end
