% Test eigenvalue function - full symmetric decomposition
function t = test_eig2
% First the float version
t1all = 1;
for i=1:100
  a = float(randn(8)); a = a + a';
  [v,d] = eig(a);
  emat = a*v - v*d;
  er = max(abs(emat(:)));
  bnd = 1.2*max(diag(abs(d)))*2^(-24)*8;
  t1 = (er < bnd);
  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
  t1all = t1all & t1;
end
% Now the double version
t2all = 1;
for i=1:100
  a = double(randn(8)); a = a + a';
  [v,d] = eig(a);
  emat = a*v - v*d;
  er = max(abs(emat(:)));
  bnd = 1.2*max(diag(abs(d)))*2^(-53)*8;
  t1 = (er < bnd);
  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
  t2all = t2all & t1;
end
% Now the complex version
t3all = 1;
for i=1:100
  a = complex(randn(8)+j*randn(8)); a = a + a';
  [v,d] = eig(a);
  emat = a*v - v*d;
  er = max(abs(emat(:)));
  bnd = 1.2*max(diag(abs(d)))*2^(-24)*8;
  t1 = (er < bnd);
  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
  t3all = t3all & t1;
end
% Now the double version
t4all = 1;
for i=1:100
  a = dcomplex(randn(8)+j*randn(8)); a = a + a';
  [v,d] = eig(a);
  emat = a*v - v*d;
  er = max(abs(emat(:)));
  bnd = 1.2*max(diag(abs(d)))*2^(-53)*8;
  t1 = (er < bnd);
  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
  t4all = t4all & t1;
end
t = t1all & t2all & t3all & t4all;




