[yd1,zd1] = test_sparse_mat('double',100,100);
[yz1,zz1] = test_sparse_mat('dcomplex',100,100);
[yd2,zd2] = test_sparse_mat('double',100,60);
[yz2,zz2] = test_sparse_mat('dcomplex',100,60);
s1 = yd1\yd2;
s2 = zd1\zd2;
s3 = yz1\yz2;
s4 = zz1\zz2;
