%  Test the sparse matrix suite
function x = test_sparse
x = 1;
for i = 1:82
  printf('Testing sparse routine %d...',i);
  y = feval(sprintf('test_sparse%d',i));
  if (y)
    printf('<pass>\n');
  else
    printf('<fail>\n');
  end
  x = x & y;
end
