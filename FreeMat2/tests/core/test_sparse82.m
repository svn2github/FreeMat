% Test sparse-sparse matrix array solution
function x = test_sparse82
[yi1,zi1] = test_sparse_mat('int32',400,400);
[yf1,zf1] = test_sparse_mat('float',400,400);
[yd1,zd1] = test_sparse_mat('double',400,400);
[yc1,zc1] = test_sparse_mat('complex',400,400);
[yz1,zz1] = test_sparse_mat('dcomplex',400,400);
[yi2,zi2] = test_sparse_mat('int32',400,600);
[yf2,zf2] = test_sparse_mat('float',400,600);
[yd2,zd2] = test_sparse_mat('double',400,600);
[yc2,zc2] = test_sparse_mat('complex',400,600);
[yz2,zz2] = test_sparse_mat('dcomplex',400,600);
x = testeq(yi1\yi2,zi1\zi2) & testeq(yf1\yf2,zf1\zf2) & testeq(yd1\yd1,zd1\zd2) & testeq(yc1\yc2,zc1\zc2) & testeq(yz1\yz2,zz1\zz2);


