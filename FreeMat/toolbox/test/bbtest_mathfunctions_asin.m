% Regression test function (black box) for asin
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_mathfunctions_asin
  bbtest_success = 1;
NumErrors = 0;
try
  t = linspace(-1,1);
catch
  NumErrors = NumErrors + 1;
end
try
  plot(t,asin(t))
catch
  NumErrors = NumErrors + 1;
end
try
  mprint('asinplot');
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end