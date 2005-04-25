function x = test_sparse84
A = int32(100*sprandn(10,10,0.3));
B = int32(100*sprandn(10,10,0.3));
C = A < B;
c = full(A) < full(B);
testeq(c,C)
x = testeq(c,C)
C = A < -2;
c = full(A) < -2;
testeq(c,C)
x = x & testeq(c,C);
C = 2 < A;
c = 2 < full(A);
testeq(c,C)
x = x & testeq(c,C);
return
C = A > B;
c = full(A) < full(B);
x = x & testeq(c,C);
C = A == B;
c = full(A) < full(B);
x = x & testeq(c,C);
C = A <= B;
c = full(A) < full(B);
x = x & testeq(c,C);
C = A >= B;
c = full(A) < full(B);
x = x & testeq(c,C);
C = A ~= B;
c = full(A) < full(B);
x = x & testeq(c,C);
