% Test the save and load capability with cell arrays (bug 1581481)
function test_val = test_save1
   a{1} = 'bert';
   save tmp.mat a
   b = a;
   load tmp.mat
   test_val = strcomp(a{1},b{1});

