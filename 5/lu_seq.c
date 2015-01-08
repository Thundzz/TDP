#include "cblas.h"
#include "mylapack.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define UNUSED(x) (void)(x)

int LAPACKE_dgetf2( int matrix_order, int m, int n,
	double* a, int lda, int* ipiv )
{
	UNUSED(ipiv);
	UNUSED(matrix_order);
	int colr, linr;
	double ajj;
	for (int j = 0; j < MIN(m, n); ++j)
	{
		colr = m - j -1;
		ajj = 1. / a[j + j * lda];
		cblas_dscal(colr, ajj, &a[j + 1+  j * lda], 1);

		if(j < MIN(m, n)){
			colr = m - j -1;
			linr = n - j -1;
			cblas_dger(CblasColMajor ,colr, linr, -1.0, &a[j + 1 + j * lda], 1, &a[j + (
				j + 1) * lda], lda, &a[j + 1 + (j + 1) * lda], lda);
		}
	}
	return 0;
}


int LAPACKE_dtrsm(int uplo,int diag, int m, int n,
 double alpha, double * A, int lda, double *B, int ldb)
 {
 	UNUSED(ldb);
 	UNUSED(n);
 	assert(alpha == 1.0);
 	if(uplo == LAPACKE_LOWER)
 	{
 		assert(diag == LAPACKE_UNIT);
 		int i;
 		for(i = 0; i<m; i++){
 			B[i] -= cblas_ddot(i, &A[i], lda, B, 1);
 		}
 	}

 	else
 	{
 		assert(diag == LAPACKE_NUNIT);
 		int i;
 		for(i = m-1; i >= 0 ; i--){
 			double aii = A[i*lda +i];
 			B[i] -= cblas_ddot(m-1-i, &A[i + (i+1)*lda], lda, &B[i+1], 1);
 			B[i] /= aii;
 		}
 	}
 	return 0;
 }

int LAPACKE_dgesv(int  N,int NRHS, double * A, int LDA, int * IPIV, double *B, int LDB)
{
	UNUSED(IPIV);
	/* Factorisation LU de A */
	LAPACKE_dgetf2( 0 , N , N , A , LDA, NULL );
	/* Résolution des systèmes d'équations linéaires */
	for (int i = 0; i < NRHS ; ++i)
	{	
		/* résolution Ly = b */
		LAPACKE_dtrsm(LAPACKE_LOWER, LAPACKE_UNIT , N, N, 1.0, A, LDA, B+ i*LDB, LDB);

		/* Résolution Ux = y */
		LAPACKE_dtrsm(LAPACKE_UPPER, LAPACKE_NUNIT, N, N, 1.0, A, LDA, B+ i*LDB, LDB);

	}
	return 0;
}