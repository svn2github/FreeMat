% Test generalized eigenvalue function - symmetric matrices
function t = test_eig5
% First the float version
t1all = 1;
printf('Float\n');
for i=2:100
  a = float(randn(i)); a = a + a'; 
  b = float(randn(i)); b = b*b';
  [v,d] = eig(a,b);
  emat = a*v - b*v*d;
  er = max(abs(emat(:)));
  bnd = 2*max(abs(eig(b)))*max(diag(abs(d)))*feps*i;
  t1 = (er < bnd);
  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); keyboard, end
  g = eig(a,b);
  e2 = max(abs(g-diag(d)));
  tb = e2<bnd;
  if (~tb) printf('compact/full decomp mismatch: er = %e (num = %d)\n',e2,i); keyboard; end
  t1all = t1all & t1 & tb;
end
% Now the double version
t2all = 1;
printf('Double\n');
for i=2:100
  a = double(randn(i)); a = a + a';
  b = double(randn(i)); b = b*b';
  [v,d] = eig(a,b);
  emat = a*v - b*v*d;
  er = max(abs(emat(:)));
  bnd = 2*max(abs(eig(b)))*max(diag(abs(d)))*eps*i;
  t1 = (er < bnd);
  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); keyboard, end
  g = eig(a,b);
  e2 = max(abs(g-diag(d)));
  tb = e2<bnd;
  if (~tb) printf('compact/full decomp mismatch: er = %e (num = %d)\n',e2,i); keyboard; end
  t2all = t2all & t1 & tb;
end
% Now the complex version
t3all = 1;
printf('Complex');
for i=2:100
  a = complex(randn(i)+j*randn(i)); a = a + a';
  b = complex(randn(i)+j*randn(i)); b = b*b';
  [v,d] = eig(a,b);
  emat = a*v - b*v*d;
  er = max(abs(emat(:)));
  bnd = 2*max(abs(eig(b)))*max(diag(abs(d)))*feps*i;
  t1 = (er < bnd);
  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); keyboard, end
  g = eig(a,b);
  e2 = max(abs(g-diag(d)));
  tb = e2<bnd;
  if (~tb) printf('compact/full decomp mismatch: er = %e (num = %d)\n',e2,i); keyboard; end
  t3all = t3all & t1 & tb;
end
% Now the double version
t4all = 1;
printf('DComplex');
for i=2:100
  a = dcomplex(randn(i)+j*randn(i)); a = a + a';
  b = dcomplex(randn(i)+j*randn(i)); b = b*b';
  [v,d] = eig(a,b);
  emat = a*v - b*v*d;
  er = max(abs(emat(:)));
  bnd = 2*max(abs(eig(b)))*max(diag(abs(d)))*eps*i;
  t1 = (er < bnd);
  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); keyboard, end
  g = eig(a,b);
  e2 = max(abs(g-diag(d)));
  tb = e2<bnd;
  if (~tb) printf('compact/full decomp mismatch: er = %e (num = %d)\n',e2,i); keyboard; end
  t4all = t4all & t1 & tb;
end
t = t1all & t2all & t3all & t4all;




