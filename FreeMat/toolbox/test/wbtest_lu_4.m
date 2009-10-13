function test_val = wbtest_lu_4(verbose)
  myloc=fileparts(which('wbtest_lu_4'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  load([myloc,dirsep,'reference',dirsep,'wbtest_lu_4_ref.mat'])
  fail_count = 0;
  for loopi=1:numel(wbinputs)
    x1 = wbinputs{loopi};
    error_flag = 0;
  y1 = [];
  y2 = [];
  y3 = [];
  y4 = [];
  y5 = [];
    try
      [y1,y2,y3,y4,y5]=lu(x1);
    catch
      error_flag = 1;
    end
    if (error_flag && ~error_refs(loopi) && ~(false))
       printf('Mismatch Errors: input %d [y1,y2,y3,y4,y5]=lu(x1)\n',loopi);
       fail_count = fail_count + 1;
    elseif (~error_flag && ~error_refs(loopi) && ~wbtest_near(y1,y1_refs{loopi}) && ~(false))
      printf('Mismatch (near): input %d output 1 [y1,y2,y3,y4,y5]=lu(x1)\n',loopi);
      fail_count = fail_count + 1;
    elseif (~error_flag && ~error_refs(loopi) && ~wbtest_near(y2,y2_refs{loopi}) && ~(false))
      printf('Mismatch (near): input %d output 2 [y1,y2,y3,y4,y5]=lu(x1)\n',loopi);
      fail_count = fail_count + 1;
    elseif (~error_flag && ~error_refs(loopi) && ~wbtest_near(y3,y3_refs{loopi}) && ~(false))
      printf('Mismatch (near): input %d output 3 [y1,y2,y3,y4,y5]=lu(x1)\n',loopi);
      fail_count = fail_count + 1;
    elseif (~error_flag && ~error_refs(loopi) && ~wbtest_near(y4,y4_refs{loopi}) && ~(false))
      printf('Mismatch (near): input %d output 4 [y1,y2,y3,y4,y5]=lu(x1)\n',loopi);
      fail_count = fail_count + 1;
    elseif (~error_flag && ~error_refs(loopi) && ~wbtest_near(y5,y5_refs{loopi}) && ~(false))
      printf('Mismatch (near): input %d output 5 [y1,y2,y3,y4,y5]=lu(x1)\n',loopi);
      fail_count = fail_count + 1;
    end
  test_val = (fail_count == 0);
end
