% Check the isempty function
function test_val = test_isempty
a = [];
b = 1;
test_val = isempty(a) & ~isempty(b);
