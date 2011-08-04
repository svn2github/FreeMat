% Regression test function (black box) for transpose
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_array_transpose
  bbtest_success = 1;
NumErrors = 0;
try
  A = [1+i,2+i;3-2*i,4+2*i]
catch
  NumErrors = NumErrors + 1;
end
try
  transpose(A)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end