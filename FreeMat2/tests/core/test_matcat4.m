% Check the type promotion for the matrix cat function
function test_val = test_matcat4
a = [1,2;3.0f,4+i];
test_val = test(strcmp(typeof(a),'complex'));
