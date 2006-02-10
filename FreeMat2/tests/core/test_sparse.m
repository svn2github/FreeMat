%  Test the sparse matrix suite
function x = test_sparse
printf('\n');
x = 1;
for i = 1:115
  printf('Testing sparse routine %d...',i);
  y = feval(sprintf('test_sparse%d',i));
  if (~y)
    printf('Failed\n');
  else
    printf('Passed\n');
  end
  x = x & y;
end
