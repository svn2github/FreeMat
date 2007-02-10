%!
%@Module TEST Test Function
%@@Section ELEMENTARY
%@@Usage
%Tests for the argument array to be all logical 1s.  It is 
%completely equivalent to the @|all| function applied to
%a vectorized form of the input.  The syntax for the @|test|
%function is
%@[
%   y = test(x)
%@]
%and the result is equivalent to @|all(x(:))|.
%@@Tests
%@{ test_test1.m
%% Check that test function works properly for a scalar 1
%function test_val = test_test1
%test_val = test(1) == 1;
%@}
%@{ test_test2.m
%% Check that test function works properly for a scalar 0
%function test_val = test_test2
%
%test_val = test(0) == 0;
%@}
%@{ test_test3.m
%% Check that test function works properly for a vector of 1
%function test_val = test_test3
%
%test_val = test([1,1,1,1]) == 1;
%@}
%@{ test_test4.m
%% Check that test function works properly for a vector with one zero
%function test_val = test_test4
%
%test_val = test([1,0,1,1]) == 0;
%@}
%@{ test_test5.m
%% Check that test function throws an error when not given an argument
%function test_val = test_test5
%
%test_val = 0;
%
%try
%  test;
%catch
%  test_val = 1;
%end
%@}
%!
function y = test(x)
   y=all(x(:));

