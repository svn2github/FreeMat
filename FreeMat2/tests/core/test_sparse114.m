function x = test_sparse114
A = sprandn(10,10,0.3)>0;
B = sprandn(10,10,0.3)>0;
C = A & B;
c = full(A) & full(B);
testeq(c,C)
x = testeq(c,C)
C = A & 0
c = full(A) & 0
keyboard
testeq(c,C)
x = x & testeq(c,C)
C = 1 & A;
c = 1 & full(A);
testeq(c,C)
x = x & testeq(c,C)
