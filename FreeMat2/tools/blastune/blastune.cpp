#include <QtCore>
#include <iostream>



typedef void (*dgemmptr) (char * ta, char* tb, int* m, int* n, int* k, double *alp,
			  const double*A, int* LDA, const double* B, 
			  int* LDB, double* BETA, double *C, int*LDC);

void doubleMatrixMatrixMultiply(int m, int n, int k,
				double* c, const double* a, const double *b,
				dgemmptr ptr) {
  // Use gemm, which computes
  // C = alpha*A*B + beta*C
  //      SUBROUTINE SGEMM ( TRANSA, TRANSB, M, N, K, ALPHA, A, LDA, B, LDB,
  //     $                   BETA, C, LDC )
  //*     .. Scalar Arguments ..
  //      CHARACTER*1        TRANSA, TRANSB
  //      INTEGER            M, N, K, LDA, LDB, LDC
  //      REAL               ALPHA, BETA
  //*     .. Array Arguments ..
  //      REAL               A( LDA, * ), B( LDB, * ), C( LDC, * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  SGEMM  performs one of the matrix-matrix operations
  //*
  //*     C := alpha*op( A )*op( B ) + beta*C,
  //*
  //*  where  op( X ) is one of
  //*
  //*     op( X ) = X   or   op( X ) = X',
  //*
  //*  alpha and beta are scalars, and A, B and C are matrices, with op( A )
  //*  an m by k matrix,  op( B )  a  k by n matrix and  C an m by n matrix.
  //*
  //*  Parameters
  //*  ==========
  //*
  //*  TRANSA - CHARACTER*1.
  //*           On entry, TRANSA specifies the form of op( A ) to be used in
  //*           the matrix multiplication as follows:
  //*
  //*              TRANSA = 'N' or 'n',  op( A ) = A.
  //*
  //*              TRANSA = 'T' or 't',  op( A ) = A'.
  //*
  //*              TRANSA = 'C' or 'c',  op( A ) = A'.
  //*
  //*           Unchanged on exit.
  
  char TRANSA = 'N';
  
  //*
  //*  TRANSB - CHARACTER*1.
  //*           On entry, TRANSB specifies the form of op( B ) to be used in
  //*           the matrix multiplication as follows:
  //*
  //*              TRANSB = 'N' or 'n',  op( B ) = B.
  //*
  //*              TRANSB = 'T' or 't',  op( B ) = B'.
  //*
  //*              TRANSB = 'C' or 'c',  op( B ) = B'.
  //*
  //*           Unchanged on exit.
  
  char TRANSB = 'N';
  
  //*
  //*  M      - INTEGER.
  //*           On entry,  M  specifies  the number  of rows  of the  matrix
  //*           op( A )  and of the  matrix  C.  M  must  be at least  zero.
  //*           Unchanged on exit.
  //*
  
  int M = m;
  
  //*  N      - INTEGER.
  //*           On entry,  N  specifies the number  of columns of the matrix
  //*           op( B ) and the number of columns of the matrix C. N must be
  //*           at least zero.
  //*           Unchanged on exit.
  //*
  
  int N = n;
  
  //*  K      - INTEGER.
  //*           On entry,  K  specifies  the number of columns of the matrix
  //*           op( A ) and the number of rows of the matrix op( B ). K must
  //*           be at least  zero.
  //*           Unchanged on exit.
  //*
  
  int K = k;
  
  //*  ALPHA  - REAL            .
  //*           On entry, ALPHA specifies the scalar alpha.
  //*           Unchanged on exit.
  //*
  
  double ALPHA = 1.0;
  
  //*  A      - REAL             array of DIMENSION ( LDA, ka ), where ka is
  //*           k  when  TRANSA = 'N' or 'n',  and is  m  otherwise.
  //*           Before entry with  TRANSA = 'N' or 'n',  the leading  m by k
  //*           part of the array  A  must contain the matrix  A,  otherwise
  //*           the leading  k by m  part of the array  A  must contain  the
  //*           matrix A.
  //*           Unchanged on exit.
  //*
  
  
  //*  LDA    - INTEGER.
  //*           On entry, LDA specifies the first dimension of A as declared
  //*           in the calling (sub) program. When  TRANSA = 'N' or 'n' then
  //*           LDA must be at least  max( 1, m ), otherwise  LDA must be at
  //*           least  max( 1, k ).
  //*           Unchanged on exit.
  //*
  
  int LDA = m;
  
  //*  B      - REAL             array of DIMENSION ( LDB, kb ), where kb is
  //*           n  when  TRANSB = 'N' or 'n',  and is  k  otherwise.
  //*           Before entry with  TRANSB = 'N' or 'n',  the leading  k by n
  //*           part of the array  B  must contain the matrix  B,  otherwise
  //*           the leading  n by k  part of the array  B  must contain  the
  //*           matrix B.
  //*           Unchanged on exit.
  //*
  
  
  //*  LDB    - INTEGER.
  //*           On entry, LDB specifies the first dimension of B as declared
  //*           in the calling (sub) program. When  TRANSB = 'N' or 'n' then
  //*           LDB must be at least  max( 1, k ), otherwise  LDB must be at
  //*           least  max( 1, n ).
  //*           Unchanged on exit.
  //*

  int LDB = k;

  //*  BETA   - REAL            .
  //*           On entry,  BETA  specifies the scalar  beta.  When  BETA  is
  //*           supplied as zero then C need not be set on input.
  //*           Unchanged on exit.
  //*
  
  double BETA = 0.0;
  
  //*  C      - REAL             array of DIMENSION ( LDC, n ).
  //*           Before entry, the leading  m by n  part of the array  C must
  //*           contain the matrix  C,  except when  beta  is zero, in which
  //*           case C need not be set on entry.
  //*           On exit, the array  C  is overwritten by the  m by n  matrix
  //*           ( alpha*op( A )*op( B ) + beta*C ).
  //*

  double *C = c;

  //*  LDC    - INTEGER.
  //*           On entry, LDC specifies the first dimension of C as declared
  //*           in  the  calling  (sub)  program.   LDC  must  be  at  least
  //*           max( 1, m ).
  //*           Unchanged on exit.
  //*
  
  int LDC = m;

  ptr( &TRANSA, &TRANSB, &M, &N, &K, &ALPHA, a, &LDA, b, &LDB,
	  &BETA, C, &LDC );
}

void usage() {
  std::cerr << "Usage: blastune <blas.so file>\n";
  exit(1);
}

void FillMatrix(double *a, int m, int n) {
  for (int i=0;i<m*n;i++)
    a[i] = (i/((float) m*n));
}

void TestDGEMM(int n, int reps, dgemmptr ptr) {
  double *a, *b, *c;
  a = new double[n*n];
  b = new double[n*n];
  c = new double[n*n];
  FillMatrix(a,n,n);
  FillMatrix(b,n,n);
  for (int i=0;i<reps;i++) {
    doubleMatrixMatrixMultiply(n,n,n,c,a,b,ptr);
  }
}

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);
  if (argc != 2) usage();
  QLibrary blaslib(argv[1]);
  dgemmptr ptr = (dgemmptr) blaslib.resolve("dgemm_");
  if (!ptr) {
    std::cerr << "Failed to resolve dgemm call\n";
    return 1;
  }
  TestDGEMM(500,10,ptr);
  std::cout << "Success\n";
  return 0;
}
