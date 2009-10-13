function test_val = wbtest_eig_4(verbose)
  myloc=fileparts(which('wbtest_eig_4'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  load([myloc,dirsep,'reference',dirsep,'wbtest_eig_4_ref.mat'])
  fail_count = 0;
  for loopi=1:numel(wbinputs)
    x1 = wbinputs{loopi};
    error_flag = 0;
  y1 = [];
  y2 = [];
    try
      [y1,y2]=eig(x1,'nobalance');
    catch
      error_flag = 1;
    end
    if (error_flag && ~error_refs(loopi) && ~((any(loopi==[14,59,64]))))
       printf('Mismatch Errors: input %d [y1,y2]=eig(x1,''nobalance'')\n',loopi);
       fail_count = fail_count + 1;
    elseif (~error_flag && ~error_refs(loopi) && ~wbtest_near(y1,y1_refs{loopi}) && ~((any(loopi==[14,59,64]))))
      printf('Mismatch (near): input %d output 1 [y1,y2]=eig(x1,''nobalance'')\n',loopi);
      fail_count = fail_count + 1;
    elseif (~error_flag && ~error_refs(loopi) && ~wbtest_near(y2,y2_refs{loopi}) && ~((any(loopi==[14,59,64]))))
      printf('Mismatch (near): input %d output 2 [y1,y2]=eig(x1,''nobalance'')\n',loopi);
      fail_count = fail_count + 1;
    end
  test_val = (fail_count == 0);
end
