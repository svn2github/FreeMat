% Test sparse matrix array transpose
function x = test_sparse77
[yi1,zi1] = test_sparse_mat('int32',300,400);
[yf1,zf1] = test_sparse_mat('float',300,400);
[yd1,zd1] = test_sparse_mat('double',300,400);
[yc1,zc1] = test_sparse_mat('complex',300,400);
[yz1,zz1] = test_sparse_mat('dcomplex',300,400);
x = testeq(yi1.',zi1.') & testeq(yf1.',zf1.') & testeq(yd1.',zd1.') & testeq(yc1.',zc1.') & testeq(yz1.',zz1.');


