% Test the global variable declaration
function test_val = test_global1
global shared;
shared = 55;
test_val = test(test_global1_assist == 110);

function tick = test_global1_assist
global shared;
tick = shared*2;
