% Test the QR decomposition (full, without pivoting)
function test_val = test_qr9
  printf('Float\n');
  t1all = 1;
  for n=2:100
    a = float(rand(n,2*n));
    [q,r] = qr(a);
    err = abs(a-q*r);
    err = max(err(:));
    bnd = 2*max(abs(diag(r)))*feps*(n^(0.5));
    t1 = (err < bnd);
    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
    t1all = t1all & t1;
  end
  printf('Double\n');
  t2all = 1;
  for n=2:100
    a = double(rand(n,2*n));
    [q,r] = qr(a);
    err = abs(a-q*r);
    err = max(err(:));
    bnd = 2*max(abs(diag(r)))*eps*(n^(0.5));
    t2 = (err < bnd);
    if (~t2) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
    t2all = t2all & t2;
  end
  printf('Complex\n');
  t3all = 1;
  for n=2:100
    a = complex(rand(n,2*n)+i*rand(n,2*n));
    [q,r] = qr(a);
    err = abs(a-q*r);
    err = max(err(:));
    bnd = 2*max(abs(diag(r)))*feps*(n^(0.5));
    t3 = (err < bnd);
    if (~t3) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
    t3all = t3all & t3;
  end
  printf('Dcomplex\n');
  t4all = 1;
  for n=2:100
    a = dcomplex(rand(n,2*n)+i*rand(n,2*n));
    [q,r] = qr(a);
    err = abs(a-q*r);
    err = max(err(:));
    bnd = 2*max(abs(diag(r)))*eps*(n^(0.5));
    t4 = (err < bnd);
    if (~t4) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
    t4all = t4all & t4;
  end
  test_val = t1all & t2all & t3all & t4all;
