% Test sparse matrix array vector-subset assignment
function x = test_sparse65
[yi1,zi1] = test_sparse_mat('int32',300,400);
[yf1,zf1] = test_sparse_mat('float',300,400);
[yd1,zd1] = test_sparse_mat('double',300,400);
[yc1,zc1] = test_sparse_mat('complex',300,400);
[yz1,zz1] = test_sparse_mat('dcomplex',300,400);
ndx = randi(ones(1500,1),300*400*ones(1500,1));
gi = int32(100*randn(1500,1));
gf = float(randn(1500,1));
gd = randn(1500,1);
gc = complex(randn(1500,1)+i*randn(1500,1));
gz = dcomplex(randn(1500,1)+i*randn(1500,1));
yi1(ndx) = gi; zi1(ndx) = gi;
yf1(ndx) = gf; zf1(ndx) = gf;
yd1(ndx) = gd; zd1(ndx) = gd;
yc1(ndx) = gc; zc1(ndx) = gc;
yz1(ndx) = gz; zz1(ndx) = gz;
x = testeq(yi1,zi1) & testeq(yf1,zf1) & testeq(yd1,zd1) & testeq(yc1,zc1) & testeq(yz1,zz1);
