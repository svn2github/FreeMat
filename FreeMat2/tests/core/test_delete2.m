% Check the delete functionality in an ndim setting
function test_val = test_delete2
a = [1,2,3;4,5,6];
a(:,2) = [];
test_val = test(a == [1,3;4,6]);

