function test_val = wbtest_speye_2(verbose)
  myloc=fileparts(which('wbtest_speye_2'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  load([myloc,dirsep,'reference',dirsep,'wbtest_speye_2_ref.mat'])
  fail_count = 0;
  error_flag = 0;
  y1 = [];
  try
    z=speye(5000);y1=full(z(1:10,1:10));
  catch
    error_flag = 1;
  end
  if (error_flag && ~error_refs)
     printf('Mismatch Errors: z=speye(5000);y1=full(z(1:10,1:10))\n');
     fail_count = fail_count + 1;
  elseif (~error_flag && ~error_refs && ~wbtest_exact(y1,y1_refs{1}))
    printf('Mismatch (exact): output 1 z=speye(5000);y1=full(z(1:10,1:10))\n');
    fail_count = fail_count + 1;
  end
  test_val = (fail_count == 0);
end
