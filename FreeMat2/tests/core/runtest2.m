function [succeeded,failed,errors] = runtest2(name,suc_in,failed_in,errors_in)
try
  status = feval(name);
catch
  status = -1;
end
succeeded = [];
failed = [];
errors = [];
