% Check the delete functionality in an ndim setting
function test_val = test_delete3
a = [1,2;3,4];
a(2,2,2) = 7;
a(:,:,2) = [];
test_val = test(a == [1,2;3,4]);
