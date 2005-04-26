%  Test the sparse matrix suite
function x = test_sparse
x = 1;
for i = 1:115
  printf('Testing sparse routine %d...\r',i);
  y = feval(sprintf('test_sparse%d',i));
  if (~y)
    printf('\nFailed test %d\n',i);
  end
  x = x & y;
end
