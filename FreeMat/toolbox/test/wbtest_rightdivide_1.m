function test_val = wbtest_rightdivide_1(verbose)
  myloc=fileparts(which('wbtest_rightdivide_1'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  load([myloc,dirsep,'reference',dirsep,'wbtest_rightdivide_1_ref.mat'])
  fail_count = 0;
  for loopi=1:numel(wbinputs)
    for loopj=1:numel(wbinputs)
      x1 = wbinputs{loopi};
      x2 = wbinputs{loopj};
      error_flag = 0;
     y1 = [];
      try
        y1=x1/x2;
      catch
        error_flag = 1;
      end
    if (error_flag && ~error_refs(loopi,loopj) && ~(((size(x1,1)==1)||any(isinf(x1))||any(isinf(x2))||any(loopi==[39,60,61,64])||any(loopj==[39,60,61,64]))))
       printf('Mismatch Errors: input %d, %d y1=x1/x2\n',loopi,loopj);
        fail_count = fail_count + 1;
  elseif (~error_flag && ~error_refs(loopi,loopj) && ~wbtest_near(y1,y1_refs{loopi,loopj}) && ~(((size(x1,1)==1)||any(isinf(x1))||any(isinf(x2))||any(loopi==[39,60,61,64])||any(loopj==[39,60,61,64]))))
    printf('Mismatch (near): input %d,%d output 1 y1=x1/x2\n',loopi,loopj);
    fail_count = fail_count + 1;
    end
  end
  test_val = (fail_count == 0);
end
