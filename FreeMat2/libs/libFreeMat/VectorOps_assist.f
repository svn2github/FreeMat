* Copyright (c) 2002, 2003 Samit Basu
* 
* Permission is hereby granted, free of charge, to any person obtaining a 
* copy of this software and associated documentation files (the "Software"), 
* to deal in the Software without restriction, including without limitation 
* the rights to use, copy, modify, merge, publish, distribute, sublicense, 
* and/or sell copies of the Software, and to permit persons to whom the 
* Software is furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included 
* in all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
* DEALINGS IN THE SOFTWARE.
***************************************************************************
* Vector Add functions
***************************************************************************
* Add two int32 vectors
      SUBROUTINE INT32ADD(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*4 C(*), B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) + B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Add two float vectors
      SUBROUTINE FLOATADD(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      REAL*4 C(*), B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) + B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Add two double vectors
      SUBROUTINE DOUBLEADD(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      REAL*8 C(*), B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) + B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Add two complex vectors
      SUBROUTINE COMPLEXADD(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      COMPLEX*8 C(*), B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) + B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Add two complex vectors
      SUBROUTINE DCOMPLEXADD(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      COMPLEX*16 C(*), B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) + B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END


***************************************************************************
* Vector Subtract functions
***************************************************************************
* Subtract two int32 vectors
      SUBROUTINE INT32SUBTRACT(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*4 C(*), B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) - B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Subtract two float vectors
      SUBROUTINE FLOATSUBTRACT(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      REAL*4 C(*), B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) - B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Subtract two double vectors
      SUBROUTINE DOUBLESUBTRACT(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      REAL*8 C(*), B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) - B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Subtract two complex vectors
      SUBROUTINE COMPLEXSUBTRACT(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      COMPLEX*8 C(*), B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) - B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Subtract two complex vectors
      SUBROUTINE DCOMPLEXSUBTRACT(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      COMPLEX*16 C(*), B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) - B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

***************************************************************************
* Vector DotMultiply functions
***************************************************************************
* DotMultiply two int32 vectors
      SUBROUTINE INT32DOTMULTIPLY(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*4 C(*), B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) * B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* DotMultiply two float vectors
      SUBROUTINE FLOATDOTMULTIPLY(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      REAL*4 C(*), B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) * B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* DotMultiply two double vectors
      SUBROUTINE DOUBLEDOTMULTIPLY(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      REAL*8 C(*), B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) * B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* DotMultiply two complex vectors
      SUBROUTINE COMPLEXDOTMULTIPLY(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      COMPLEX*8 C(*), B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) * B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* DotMultiply two complex vectors
      SUBROUTINE DCOMPLEXDOTMULTIPLY(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      COMPLEX*16 C(*), B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) * B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

***************************************************************************
* Vector Dotleftdivide functions
***************************************************************************
* Dotleftdivide two int32 vectors
      SUBROUTINE INT32DOTLEFTDIVIDE(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*4 C(*), B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = B(P) / A(M)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Dotleftdivide two float vectors
      SUBROUTINE FLOATDOTLEFTDIVIDE(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      REAL*4 C(*), B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = B(P) / A(M)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Dotleftdivide two double vectors
      SUBROUTINE DOUBLEDOTLEFTDIVIDE(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      REAL*8 C(*), B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = B(P) / A(M)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Dotleftdivide two complex vectors
      SUBROUTINE COMPLEXDOTLEFTDIVIDE(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      COMPLEX*8 C(*), B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = B(P) / A(M)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Dotleftdivide two complex vectors
      SUBROUTINE DCOMPLEXDOTLEFTDIVIDE(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      COMPLEX*16 C(*), B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) =  B(P) / A(M)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

***************************************************************************
* Vector Dotrightdivide functions
***************************************************************************
* Dotrightdivide two int32 vectors
      SUBROUTINE INT32DOTRIGHTDIVIDE(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*4 C(*), B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) / B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Dotrightdivide two float vectors
      SUBROUTINE FLOATDOTRIGHTDIVIDE(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      REAL*4 C(*), B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) / B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Dotrightdivide two double vectors
      SUBROUTINE DOUBLEDOTRIGHTDIVIDE(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      REAL*8 C(*), B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) / B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Dotrightdivide two complex vectors
      SUBROUTINE COMPLEXDOTRIGHTDIVIDE(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      COMPLEX*8 C(*), B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) / B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Dotrightdivide two complex vectors
      SUBROUTINE DCOMPLEXDOTRIGHTDIVIDE(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      COMPLEX*16 C(*), B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) / B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

***************************************************************************
* Vector Dotpower functions
***************************************************************************
      SUBROUTINE CICPOWER(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      COMPLEX A(*)
      INTEGER*4 B(*)
      COMPLEX C(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) ** B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

      SUBROUTINE CFCPOWER(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      COMPLEX A(*)
      REAL B(*)
      COMPLEX C(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) ** B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

      SUBROUTINE ZDZPOWER(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      COMPLEX*16 A(*)
      DOUBLE PRECISION B(*)
      COMPLEX*16 C(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) ** B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

      SUBROUTINE CCCPOWER(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      COMPLEX A(*)
      COMPLEX B(*)
      COMPLEX C(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) ** B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

      SUBROUTINE ZZZPOWER(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      COMPLEX*16 A(*)
      COMPLEX*16 B(*)
      COMPLEX*16 C(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) ** B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

      SUBROUTINE ZIZPOWER(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      COMPLEX*16 A(*)
      INTEGER*4 B(*)
      COMPLEX*16 C(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) ** B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

      SUBROUTINE DIDPOWER(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      DOUBLE PRECISION A(*)
      INTEGER*4 B(*)
      DOUBLE PRECISION C(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) ** B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

      SUBROUTINE DDDPOWER(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      DOUBLE PRECISION A(*)
      DOUBLE PRECISION B(*)
      DOUBLE PRECISION C(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) ** B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

      SUBROUTINE FIFPOWER(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      REAL A(*)
      INTEGER*4 B(*)
      REAL C(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) ** B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

      SUBROUTINE FFFPOWER(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      REAL A(*)
      REAL B(*)
      REAL C(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         C(I) = A(M) ** B(P)
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

***************************************************************************
* Vector Negate functions
***************************************************************************

* Negate a int32 vector
      SUBROUTINE INT32NEGATE(N, C, A)
      INTEGER N
      INTEGER*4 C(*), A(*)
      INTEGER I
      DO 10 I = 1, N
         C(I) = -A(I)
 10   CONTINUE
      END

* Negate a float vector
      SUBROUTINE FLOATNEGATE(N, C, A)
      INTEGER N
      REAL*4 C(*), A(*)
      INTEGER I
      DO 10 I = 1, N
         C(I) = -A(I)
 10   CONTINUE
      END

* Negate a double vector
      SUBROUTINE DOUBLENEGATE(N, C, A)
      INTEGER N
      REAL*8 C(*), A(*)
      INTEGER I
      DO 10 I = 1, N
         C(I) = -A(I)
 10   CONTINUE
      END

* Negate a complex vector
      SUBROUTINE COMPLEXNEGATE(N, C, A)
      INTEGER N
      COMPLEX*8 C(*), A(*)
      INTEGER I
      DO 10 I = 1, N
         C(I) = -A(I)
 10   CONTINUE
      END

* Negate a dcomplex vector
      SUBROUTINE DCOMPLEXNEGATE(N, C, A)
      INTEGER N
      COMPLEX*16 C(*), A(*)
      INTEGER I
      DO 10 I = 1, N
         C(I) = -A(I)
 10   CONTINUE
      END

***************************************************************************
* Vector LessThan functions
***************************************************************************
* Compare two int32 vectors
      SUBROUTINE INT32LESSTHAN(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      INTEGER*4 B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (A(M).LT.B(P)) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Compare two float vectors
      SUBROUTINE FLOATLESSTHAN(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      REAL*4 B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (A(M).LT.B(P)) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Compare two double vectors
      SUBROUTINE DOUBLELESSTHAN(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      REAL*8 B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (A(M).LT.B(P)) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Compare two complex vectors
      SUBROUTINE COMPLEXLESSTHAN(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      COMPLEX*8 B(*), A(*)
      INTEGER M, P, I
      INTRINSIC ABS
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (ABS(A(M)).LT.ABS(B(P))) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Compare two dcomplex vectors
      SUBROUTINE DCOMPLEXLESSTHAN(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      REAL*8 B(*), A(*)
      INTEGER M, P, I
      INTRINSIC ABS
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (ABS(A(M)).LT.ABS(B(P))) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END
      
***************************************************************************
* Vector Lessequals functions
***************************************************************************
* Compare two int32 vectors
      SUBROUTINE INT32LESSEQUALS(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      INTEGER*4 B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (A(M).LE.B(P)) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Compare two float vectors
      SUBROUTINE FLOATLESSEQUALS(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      REAL*4 B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (A(M).LE.B(P)) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Compare two double vectors
      SUBROUTINE DOUBLELESSEQUALS(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      REAL*8 B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (A(M).LE.B(P)) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Compare two complex vectors
      SUBROUTINE COMPLEXLESSEQUALS(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      COMPLEX*8 B(*), A(*)
      INTEGER M, P, I
      INTRINSIC ABS
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (ABS(A(M)).LE.ABS(B(P))) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Compare two dcomplex vectors
      SUBROUTINE DCOMPLEXLESSEQUALS(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      REAL*8 B(*), A(*)
      INTEGER M, P, I
      INTRINSIC ABS
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (ABS(A(M)).LE.ABS(B(P))) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END
      
***************************************************************************
* Vector GreaterThan functions
***************************************************************************
* Compare two int32 vectors
      SUBROUTINE INT32GREATERTHAN(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      INTEGER*4 B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (A(M).GT.B(P)) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Compare two float vectors
      SUBROUTINE FLOATGREATERTHAN(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      REAL*4 B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (A(M).GT.B(P)) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Compare two double vectors
      SUBROUTINE DOUBLEGREATERTHAN(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      REAL*8 B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (A(M).GT.B(P)) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Compare two complex vectors
      SUBROUTINE COMPLEXGREATERTHAN(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      COMPLEX*8 B(*), A(*)
      INTEGER M, P, I
      INTRINSIC ABS
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (ABS(A(M)).GT.ABS(B(P))) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Compare two dcomplex vectors
      SUBROUTINE DCOMPLEXGREATERTHAN(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      REAL*8 B(*), A(*)
      INTEGER M, P, I
      INTRINSIC ABS
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (ABS(A(M)).GT.ABS(B(P))) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END
      
***************************************************************************
* Vector Greaterequals functions
***************************************************************************
* Compare two int32 vectors
      SUBROUTINE INT32GREATEREQUALS(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      INTEGER*4 B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (A(M).GE.B(P)) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Compare two float vectors
      SUBROUTINE FLOATGREATEREQUALS(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      REAL*4 B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (A(M).GE.B(P)) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Compare two double vectors
      SUBROUTINE DOUBLEGREATEREQUALS(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      REAL*8 B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (A(M).GE.B(P)) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Compare two complex vectors
      SUBROUTINE COMPLEXGREATEREQUALS(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      COMPLEX*8 B(*), A(*)
      INTEGER M, P, I
      INTRINSIC ABS
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (ABS(A(M)).GE.ABS(B(P))) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Compare two dcomplex vectors
      SUBROUTINE DCOMPLEXGREATEREQUALS(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      REAL*8 B(*), A(*)
      INTEGER M, P, I
      INTRINSIC ABS
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (ABS(A(M)).GE.ABS(B(P))) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END
      
***************************************************************************
* Vector Notequals functions
***************************************************************************
* Compare two int32 vectors
      SUBROUTINE INT32NOTEQUALS(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      INTEGER*4 B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (A(M).NE.B(P)) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Compare two float vectors
      SUBROUTINE FLOATNOTEQUALS(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      REAL*4 B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (A(M).NE.B(P)) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Compare two double vectors
      SUBROUTINE DOUBLENOTEQUALS(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      REAL*8 B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (A(M).NE.B(P)) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Compare two complex vectors
      SUBROUTINE COMPLEXNOTEQUALS(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      COMPLEX*8 B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (A(M).NE.B(P)) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Compare two dcomplex vectors
      SUBROUTINE DCOMPLEXNOTEQUALS(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      REAL*8 B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (A(M).NE.B(P)) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END
      
***************************************************************************
* Vector Equals functions
***************************************************************************
* Compare two int32 vectors
      SUBROUTINE INT32EQUALS(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      INTEGER*4 B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (A(M).EQ.B(P)) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Compare two float vectors
      SUBROUTINE FLOATEQUALS(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      REAL*4 B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (A(M).EQ.B(P)) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Compare two double vectors
      SUBROUTINE DOUBLEEQUALS(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      REAL*8 B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (A(M).EQ.B(P)) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Compare two complex vectors
      SUBROUTINE COMPLEXEQUALS(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      COMPLEX*8 B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (A(M).EQ.B(P)) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END

* Compare two dcomplex vectors
      SUBROUTINE DCOMPLEXEQUALS(N, C, A, STRIDE1, B, STRIDE2)
      INTEGER N, STRIDE1, STRIDE2
      INTEGER*1 C(*)
      REAL*8 B(*), A(*)
      INTEGER M, P, I
      M = 1
      P = 1
      DO 10 I = 1, N
         IF (A(M).EQ.B(P)) THEN
            C(I) = 1
         ELSE
            C(I) = 0
         END IF
         M = M + STRIDE1
         P = P + STRIDE2
 10   CONTINUE
      END
      
