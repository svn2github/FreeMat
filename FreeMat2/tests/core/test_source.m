% Check that the source function does not double-execute the last line of the script
function test_val = test_source
source('test_source_script.m')
test_val = test(n == 2);
