% Test for reassign bug in repmat
function t = test_assign9
a = {1,2,3}
b = repmat(a,2);
a = 'hello';
b = repmat(a,2)
t = 1;
