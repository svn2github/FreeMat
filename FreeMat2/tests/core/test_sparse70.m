% Test sparse to IJV to sparse conversion
function x = test_sparse70
[yi1,zi1] = test_sparse_mat('int32',300,400);
[yf1,zf1] = test_sparse_mat('float',300,400);
[yd1,zd1] = test_sparse_mat('double',300,400);
[yc1,zc1] = test_sparse_mat('complex',300,400);
[yz1,zz1] = test_sparse_mat('dcomplex',300,400);
[iyi1,jyi1,vyi1] = find(yi1);
[iyf1,jyf1,vyf1] = find(yf1);
[iyd1,jyd1,vyd1] = find(yd1);
[iyc1,jyc1,vyc1] = find(yc1);
[iyz1,jyz1,vyz1] = find(yz1);
x = testeq(sparse(iyi1,jyi1,vyi1),zi1);
x = x & testeq(sparse(iyf1,jyf1,vyf1),zf1);
x = x & testeq(sparse(iyd1,jyd1,vyd1),zd1);
x = x & testeq(sparse(iyc1,jyc1,vyc1),zc1);
x = x & testeq(sparse(iyz1,jyz1,vyz1),zz1);
