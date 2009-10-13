function test_val = wbtest_strrep_2(verbose)
  myloc=fileparts(which('wbtest_strrep_2'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  load([myloc,dirsep,'reference',dirsep,'wbtest_strrep_2_ref.mat'])
  fail_count = 0;
  for loopi=1:numel(wbinputs)
    x1 = wbinputs{loopi};
    error_flag = 0;
  y1 = [];
    try
      y1=strrep(x1,'time','money');
    catch
      error_flag = 1;
    end
    if (error_flag && ~error_refs(loopi) && ~(false))
       printf('Mismatch Errors: input %d y1=strrep(x1,''time'',''money'')\n',loopi);
       fail_count = fail_count + 1;
    elseif (~error_flag && ~error_refs(loopi) && ~wbtest_exact(y1,y1_refs{loopi}) && ~(false))
      printf('Mismatch (exact): input %d output 1 y1=strrep(x1,''time'',''money'')\n',loopi);
      fail_count = fail_count + 1;
    end
  test_val = (fail_count == 0);
end
