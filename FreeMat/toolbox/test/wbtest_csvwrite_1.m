function test_val = wbtest_csvwrite_1(verbose)
  myloc=fileparts(which('wbtest_csvwrite_1'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  load([myloc,dirsep,'reference',dirsep,'wbtest_csvwrite_1_ref.mat'])
  fail_count = 0;
  error_flag = 0;
  y1 = [];
  try
    csvwrite('test_csvwrite.csv',[1,2,3;5,6,7]);y1=csvread('test_csvwrite.csv');
  catch
    error_flag = 1;
  end
  if (error_flag && ~error_refs)
     printf('Mismatch Errors: csvwrite(''test_csvwrite.csv'',[1,2,3;5,6,7]);y1=csvread(''test_csvwrite.csv'')\n');
     fail_count = fail_count + 1;
  elseif (~error_flag && ~error_refs && ~wbtest_exact(y1,y1_refs{1}))
    printf('Mismatch (exact): output 1 csvwrite(''test_csvwrite.csv'',[1,2,3;5,6,7]);y1=csvread(''test_csvwrite.csv'')\n');
    fail_count = fail_count + 1;
  end
  test_val = (fail_count == 0);
end
