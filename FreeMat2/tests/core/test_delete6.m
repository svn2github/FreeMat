% Check the multi-dim delete function with an invalid argument
function test_val = test_delete6
a = [1,2,3;4,5,6;7,8,9];
a(3,3,2) = 10;
test_val = 0;
try
  a(1,:,2) = [];
catch
  test_val = 1;
end
