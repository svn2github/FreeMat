% Regression test function (black box) for fullfile
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_os_fullfile
  bbtest_success = 1;
NumErrors = 0;
try
  fullfile('path','to','my','file.m')
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end