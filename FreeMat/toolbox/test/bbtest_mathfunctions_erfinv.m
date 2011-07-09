% Regression test function (black box) for erfinv
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_mathfunctions_erfinv
  bbtest_success = 1;
NumErrors = 0;
try
  x = linspace(-.9,.9,100);
catch
  NumErrors = NumErrors + 1;
end
try
  y = erfinv(x);
catch
  NumErrors = NumErrors + 1;
end
try
  plot(x,y); xlabel('x'); ylabel('erfinv(x)');
catch
  NumErrors = NumErrors + 1;
end
try
  mprint erfinv1
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
