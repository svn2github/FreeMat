a = rand(10);
a(a>0.1) = 0;
A = sparse(a);
B = A + A;
