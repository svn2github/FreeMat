% Test eigenvalue function - compact symmetric decomposition
function t = test_eig1
% First the float version
a = float(randn(8)); a = a + a';
d = eig(a);
t1 = ((size(d,1) == 8) & (size(d,2) == 1));
a = randn(8); a = a + a';
d = eig(a);
t2 = ((size(d,1) == 8) & (size(d,2) == 1));
a = float(randn(8)) + i*float(randn(8)); a = a + a';
d = eig(a);
t3 = ((size(d,1) == 8) & (size(d,2) == 1));
a = randn(8) + i*randn(8); a = a + a';
d = eig(a);
t4 = ((size(d,1) == 8) & (size(d,2) == 1));
t = t1 & t2 & t3 & t4;




