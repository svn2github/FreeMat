% Check the delete functionality in a vector setting
function test_val = test_delete1
a = [1,2,3,4,5];
a([2,3,4]) = [];
test_val = test(a == [1,5]);

