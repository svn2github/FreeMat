% Regression test function (black box) for flipud
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_array_flipud
  bbtest_success = 1;
NumErrors = 0;
try
  x = int32(rand(4)*10)
catch
  NumErrors = NumErrors + 1;
end
try
  flipud(x)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  x = int32(rand(4,4,3)*10)
catch
  NumErrors = NumErrors + 1;
end
try
  flipud(x)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end