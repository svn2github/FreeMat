% Check the delete-all functionality in an ndim setting
function test_val = test_delete4
a = [1,2,3;4,5,6];
a(:,:) = [];
test_val = test(isempty(a));
