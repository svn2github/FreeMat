function [succeeded,failed,errors] = runtest(name,suc_in,failed_in,errors_in)
status = 0;
printf('Running test: %s...',name);
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
  printf('Failed\n');
elseif (status == 1)
  succeeded{end+1} = name;
  printf('Passed\n');
else
  errors{end+1} = name;
  printf('Error\n');
end
