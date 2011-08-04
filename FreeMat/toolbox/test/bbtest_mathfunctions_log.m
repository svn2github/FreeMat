% Regression test function (black box) for log
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_mathfunctions_log
  bbtest_success = 1;
NumErrors = 0;
try
  x = linspace(1,100);
catch
  NumErrors = NumErrors + 1;
end
try
  plot(x,log(x))
catch
  NumErrors = NumErrors + 1;
end
try
  xlabel('x');
catch
  NumErrors = NumErrors + 1;
end
try
  ylabel('log(x)');
catch
  NumErrors = NumErrors + 1;
end
try
  mprint('logplot');
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end