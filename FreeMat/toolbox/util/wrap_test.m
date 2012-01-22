function wrap_test(testname)
saveretvalue(1)
try
  p = feval(testname)
catch
end
  saveretvalue(~p)
