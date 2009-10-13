function test_val = wbtest_meshgrid_1(verbose)
  myloc=fileparts(which('wbtest_meshgrid_1'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  load([myloc,dirsep,'reference',dirsep,'wbtest_meshgrid_1_ref.mat'])
  fail_count = 0;
  error_flag = 0;
  y1 = [];
  y2 = [];
  try
    [y1,y2] = meshgrid(-2:.4:2);
  catch
    error_flag = 1;
  end
  if (error_flag && ~error_refs)
     printf('Mismatch Errors: [y1,y2] = meshgrid(-2:.4:2)\n');
     fail_count = fail_count + 1;
  elseif (~error_flag && ~error_refs && ~wbtest_near(y1,y1_refs{1}))
    printf('Mismatch (near): output 1 [y1,y2] = meshgrid(-2:.4:2)\n');
    fail_count = fail_count + 1;
  elseif (~error_flag && ~error_refs && ~wbtest_near(y2,y2_refs{1}))
    printf('Mismatch (near): output 2 [y1,y2] = meshgrid(-2:.4:2)\n');
    fail_count = fail_count + 1;
  end
  test_val = (fail_count == 0);
end
