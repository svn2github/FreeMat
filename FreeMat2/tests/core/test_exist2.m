function x = test_exist2
persistent y
if (exist('y'))
  return 1;
end


