[yi,zi] = test_sparse_mat('int32',400);
[yf,zf] = test_sparse_mat('float',400);
[yd,zd] = test_sparse_mat('double',400);
[yc,zc] = test_sparse_mat('complex',400);
[yz,zz] = test_sparse_mat('dcomplex',400);
testeq(int32(yc),sparse(int32(zc)))
testeq(float(yc),sparse(float(zc)))
testeq(double(yc),sparse(double(zc)))
testeq(complex(yc),sparse(complex(zc)))
testeq(dcomplex(yc),sparse(dcomplex(zc)))
