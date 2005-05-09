% Test sparse matrix array ndim-subset assignment
function x = test_sparse66t
a = int32(10*sprand(10,8,0.2));
gi = int32(10*randn(3));
ndxr = randi(ones(3,1),10*ones(3,1));
ndxr = [4;4;8];
ndxc = randi(ones(3,1),8*ones(3,1));
ndxc = [1;3;7];
sndxr = sort(ndxr);
sndxc = sort(ndxc);
A = full(a);
A(ndxr,ndxc) = gi;
full(a)
gi
a(ndxr,ndxc) = gi;
x=0;
keyboard
return

[yi1,zi1] = test_sparse_mat('int32',30,40);
[yf1,zf1] = test_sparse_mat('float',30,40);
[yd1,zd1] = test_sparse_mat('double',30,40);
[yc1,zc1] = test_sparse_mat('complex',30,40);
[yz1,zz1] = test_sparse_mat('dcomplex',30,40);
ndxr = randi(ones(10,1),30*ones(10,1));
ndxc = randi(ones(10,1),40*ones(10,1));
gi = int32(10*randn(10,10));
gf = float(randn(10,10));
gd = randn(10,10);
gc = complex(randn(10,10)+i*randn(10,10));
gz = dcomplex(randn(10,10)+i*randn(10,10));
keyboard
yi1(ndxr,ndxc) = gi; zi1(ndxr,ndxc) = gi;
yf1(ndxr,ndxc) = gf; zf1(ndxr,ndxc) = gf;
yd1(ndxr,ndxc) = gd; zd1(ndxr,ndxc) = gd;
yc1(ndxr,ndxc) = gc; zc1(ndxr,ndxc) = gc;
yz1(ndxr,ndxc) = gz; zz1(ndxr,ndxc) = gz;
x = testeq(yi1,zi1) & testeq(yf1,zf1) & testeq(yd1,zd1) & testeq(yc1,zc1) & testeq(yz1,zz1);
keyboard
