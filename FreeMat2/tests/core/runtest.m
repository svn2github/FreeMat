function [succeeded,failed,errors] = runtest(name,suc_in,failed_in,errors_in)
status = 0;
%disp(['Running test:' name]);
printf('Running test: %s\n',name);
try
  status = feval(name);
catch
  status = -1;
end
succeeded = suc_in;
failed = failed_in;
errors = errors_in;
if (status == 0)
  failed{end+1} = name;
elseif (status == 1)
  succeeded{end+1} = name;
else
  errors{end+1} = name;
end
