#include "cblas.h"
#include "mylapack.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#define BSZ 2

#define MAX(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define MIN(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

#define UNUSED(x) (void)(x)

/* Swap the content of line a and b 
 * This function suppses their lengths are equal to M
 * And the leading dimension is lda */
void swap_rows(const int M, double *a, double *b, const int lda)
{
	double tmp;
	for (int i = 0; i < M; i++)
	{
		int index = i*lda;
		tmp = a[index];
		a[index] = b[index];
		b[index] = tmp; 
	}
}
/* Finds the best value for the pivot.
 * The "best value" is the value which has the greatest
 * absolute value amongst all the possible candidates.
 */
int find_pivot(int M, double *a, int lda)
{
	UNUSED(lda);
	double best_val = 0;
	int piv = 0;
	for(int i = 0; i < M; i++){
		double cur_val = fabs(a[i]);
		if( cur_val > best_val){
			best_val = cur_val;
			piv = i;
		}
	}
	return piv;
}

/* Swaps the rows specified by the ipiv array, but only 
 * between indexes "st" and end"
 */
void swap_between(int M, double *A, int lda, int st, int end, int * ipiv)
{
	for (int i = st; i <= end; i++)
	{
		if(st != ipiv[i])
		{
			swap_rows(M, A + i, A + ipiv[i], lda);
		}
	}
}

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
		linr = n - j -1;
		cblas_dscal(colr, ajj, &a[j + 1+  j * lda], 1);
		cblas_dger(CblasColMajor ,colr, linr, -1.0, &a[j + 1 + j * lda], 1, 
			&a[j + (j + 1) * lda], lda, &a[j + 1 + (j + 1) * lda], lda);
		
	}
	return 0;
}

int LAPACKE_dtrsm(int uplo,int diag, int m, int n,
 double alpha, double * A, int lda, double *B, int ldb)
 {
 	assert(alpha == 1.0);
 	if(uplo == LAPACKE_LOWER)
 	{
 		assert(diag == LAPACKE_UNIT);
 		int i;
 		for (int j = 0; j < n; ++j)
 		{
	 		for(i = 0; i<m; i++){
 				B[i+j*ldb] -= cblas_ddot(i, &A[i], lda, B+ j*ldb, 1);
	 		}
 		}
 	}

 	else
 	{
 		assert(diag == LAPACKE_NUNIT);
 		int i;
 		for (int j = 0; j < n; ++j)
 		{
	 		for(i = m-1; i >= 0 ; i--){
	 			double aii = A[i*lda +i];
	 			B[i+j*ldb] -= cblas_ddot(m-1-i, &A[i + (i+1)*lda],
	 									 lda, &B[i+1 +j*ldb], 1);
	 			B[i+j*ldb] /= aii;
	 		}
 		}
 	}
 	return 0;
 }

int LAPACKE_dgesv(int  N,int NRHS, double * A, int LDA,
				  int * IPIV, double *B, int LDB)
{
	UNUSED(IPIV);
	/* Factorisation LU de A */
	LAPACKE_dgetf2( 0 , N , N , A , LDA, NULL );
	/* Résolution des systèmes d'équations linéaires */

	/* résolution Ly = b */
	LAPACKE_dtrsm(LAPACKE_LOWER, LAPACKE_UNIT , N, NRHS, 1.0, A, LDA, B, LDB);

	/* Résolution Ux = y */
	LAPACKE_dtrsm(LAPACKE_UPPER, LAPACKE_NUNIT, N, N, 1.0, A, LDA, B, LDB);

	return 0;
}



int LAPACKE_dgetrf( int matrix_order, int m, int n,
	double* a, int lda, int* ipiv )
{
	//assert(m==n);
	UNUSED(ipiv);
	UNUSED(matrix_order);
	for (int i = 0; i < MIN(m, n); i += BSZ)
	{
		int cur_bs = MIN(BSZ, MIN(m,n) - i);

		LAPACKE_dgetf2( 0 ,m-i, cur_bs, a+i*lda+i, lda, NULL);
		
		if(i + cur_bs <= n){
			LAPACKE_dtrsm(LAPACKE_LOWER, LAPACKE_UNIT,  cur_bs,
						  n- i - cur_bs , 1,  a + i*lda + i,
						  lda, a+(i+cur_bs)*lda+ i, lda);
			if(i+cur_bs <= m ){
				double * hor = a + i + (i+cur_bs)*lda ;
				double * ver = a + (i+cur_bs) +i*lda;
				double * c   = a + (i+cur_bs) + (i+cur_bs)*lda;
				cblas_dgemm_scalaire(n-i -cur_bs, cur_bs, m-i -cur_bs,
									 -1.0, ver, lda, hor, lda, c , lda);
			}
		}
	}
	return 0;
}

int LAPACKE_dgetf2_piv( int matrix_order, int m, int n,
	double* a, int lda, int* ipiv )
{
	UNUSED(matrix_order);
	int colr, linr;
	double ajj;
	for (int j = 0; j < MIN(m, n); ++j)
	{
		int p = j + find_pivot(m-j, a +j +j*lda, lda);
		ipiv[j] = p;
		if(p != j)
			swap_rows(n, a+j, a+p, lda);
		colr = m - j -1;
		ajj = 1. / a[j + j * lda];
		linr = n - j -1;
		cblas_dscal(colr, ajj, &a[j + 1+  j * lda], 1);
		cblas_dger(CblasColMajor ,colr, linr, -1.0, &a[j + 1 + j * lda], 1, 
			&a[j + (j + 1) * lda], lda, &a[j + 1 + (j + 1) * lda], lda);
	}
	return 0;
}

int LAPACKE_dgetrf_piv( int matrix_order, int m, int n,
	double* a, int lda, int* ipiv )
{
	assert(m==n);
	UNUSED(matrix_order);
	for (int i = 0; i < MIN(m, n); i += BSZ)
	{
		int cur_bs = MIN(BSZ, MIN(m,n) - i);
		LAPACKE_dgetf2_piv( 0 ,m-i, cur_bs, a+i*lda+i, lda, ipiv);
		int lim = MIN(m, i+cur_bs);
		for (int j = i; j <lim ; ++j)
		{
			ipiv[j] = i+ ipiv[j];
		}
		//swap_between(i, a, lda, i, i+cur_bs, ipiv);
		if(i + cur_bs <= n){
			swap_between(n-i-cur_bs, a+(i+cur_bs)*lda, lda, i, i+cur_bs , ipiv);
			LAPACKE_dtrsm(LAPACKE_LOWER, LAPACKE_UNIT,  cur_bs,
						  n- i - cur_bs , 1,  a + i*lda + i,
						  lda, a+(i+cur_bs)*lda+ i, lda);
			if(i+cur_bs <= m ){
				double * hor = a + i + (i+cur_bs)*lda ;
				double * ver = a + (i+cur_bs) +i*lda;
				double * c   = a + (i+cur_bs) + (i+cur_bs)*lda;
				cblas_dgemm_scalaire(n-i -cur_bs, cur_bs, m-i -cur_bs,
									 -1.0, ver, lda, hor, lda, c , lda);
			}
		}
	}
	return 0;
}