function x = test_lu3
A = double(randn(20));
a = sparse(A);
[L,U,P,Q,R] = lu(a);
b = R*a;
b = b(P,Q);
c = L*U;
keyboard
