% Test DeleteSparseMatrix function
function x = test_sparse56
xi = test_sparse_mat('int32',100);
xf = test_sparse_mat('float',100);
xd = test_sparse_mat('double',100);
xc = test_sparse_mat('complex',100);
xz = test_sparse_mat('dcomplex',100);
xi = [];
xf = [];
xd = [];
xc = [];
xz = [];
x = isempty(xi) & isempty(xf) & isempty(xd) & isempty(xc) & isempty(xz);

