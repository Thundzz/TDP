#ifndef MYLAPACK_H
#define MYLAPACK_H

#define LAPACKE_LOWER 100 
#define LAPACKE_UPPER 101
#define LAPACKE_UNIT 200
#define LAPACKE_NUNIT 201
#define BSZ 3

double cblas_ddot(const int N, const double *X, const int incX,
                  const double *Y, const int incY);

void cblas_dgemm_scalaire(const int M, const int K, const int N,
							  const double alpha, const double *A, const int lda,
				 			  const double *B, const int ldb,
                 			        double *C, const int ldc);

int LAPACKE_dgetf2( int matrix_order, int m, int n,
	double* a, int lda, int* ipiv );

int LAPACKE_dtrsm(int UPLO,int DIAG, int M, int N,
 double ALPHA, double * A, int LDA, double *B, int LDB);

int LAPACKE_dgesv(int  N,int NRHS, double * A, int LDA, int * IPIV, double *B, int LDB);

int LAPACKE_dgetrf( int matrix_order, int m, int n, double* a, int lda, int* ipiv );

int LAPACKE_dgetf2_piv( int matrix_order, int m, int n,
	double* a, int lda, int* ipiv );

int LAPACKE_dgetrf_piv( int matrix_order, int m, int n,
	double* a, int lda, int* ipiv );
#endif