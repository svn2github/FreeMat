function x = test_sparse50
a = rand(200,100);
b = rand(100,300);
a(a>0.1) = 0;
c = a*b;
A = sparse(a);
C = A*b;
x = testeq(c,C);
