% Test makeDenseArray function
function x = test_sparse57
[yi,zi] = test_sparse_mat('int32',400);
[yf,zf] = test_sparse_mat('float',400);
[yd,zd] = test_sparse_mat('double',400);
[yc,zc] = test_sparse_mat('complex',400);
[yz,zz] = test_sparse_mat('dcomplex',400);
x = testeq(full(yi),zi) & testeq(full(yf),zf) & testeq(full(yd),zd) & testeq(full(yc),zc) & testeq(full(yz),zz);
