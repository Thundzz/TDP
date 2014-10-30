#include <stdio.h>
#include "cblas.h"


#define BLOCK_SIZE 40

/*A[i][j] === j*lda + i*/
#define MIN(a,b) ((a)<(b))?(a):(b)

void cblas_dgemm_scalaire_ikj(const int M,
				 const double *A, const int lda,
				 const double *B, const int ldb,
                 double *C, const int ldc)
{
	int i, j, k;
	for (i = 0; i < M; ++i)
	{
		for (k = 0; k < M; ++k)
		{
			for (j = 0; j < M; ++j)
			{
				//C[i][j] += A[k][i]*B[k][j];
				C[j*ldc +i] += A[i*lda +k]* B[j*ldb +k];
			}
		}
	}
}

void cblas_dgemm_scalaire_kij(const int M,
				 const double *A, const int lda,
				 const double *B, const int ldb,
                 double *C, const int ldc)
{
	int i, j, k;
	for (k = 0; k < M; ++k)
	{
		for (i = 0; i < M; ++i)
		{
			for (j = 0; j < M; ++j)
			{
				//C[i][j] += A[k][i]*B[k][j];
				C[j*ldc +i] += A[i*lda +k]* B[j*ldb +k];
			}
		}
	}

}

void cblas_dgemm_scalaire_ijk(const int M,
				 const double *A, const int lda,
				 const double *B, const int ldb,
                 double *C, const int ldc)
{
	int i, j, k;
	for (i = 0; i < M; ++i)
	{
		for (j = 0; j < M; ++j)
		{	
			for (k = 0; k < M; ++k)
			{
				//C[i][j] += A[k][i]*B[k][j];
				C[j*ldc +i] += A[i*lda +k]* B[j*ldb +k];
			}
		}
	}
}

void cblas_dgemm_scalaire_jik(const int M,
				 const double *A, const int lda,
				 const double *B, const int ldb,
                 double *C, const int ldc)
{
	int i, j, k;
	for (j = 0; j < M; ++j)
	{
		for (i = 0; i < M; ++i)
		{
			for (k = 0; k < M; ++k)
			{
				//C[i][j] += A[k][i]*B[k][j];
				C[j*ldc +i] += A[i*lda +k]* B[j*ldb +k];
			}
		}
	}
}

void cblas_dgemm_scalaire(const int M, const int K, const int N,
							  const double alpha, const double *A, const int lda,
				 			  const double *B, const int ldb,
                 			        double *C, const int ldc)
{
	int i, j, k;
	register double tmp;
	for (i = 0; i < M; ++i)
	{
		for (j = 0; j < N; ++j)
		{	
			tmp = 0;   //Stocke tmp dans un registre
			for (k = 0; k < K; ++k)
			{
				tmp += A[i*lda +k]* B[j*ldb +k];
			}
			C[j*ldc +i] += tmp * alpha;
		}
	}
}

/*
	A need to be K*N for this function to work
*/
void cblas_dgemm_block(const int M, const int K, const int N,
					   const double alpha, const double * A, const int lda,
					   const double * B, const int ldb,
					   		 double * C, const int ldc)
{
	int M2, N2, K2, i, j, k;
	for (i = 0; i < M; i +=BLOCK_SIZE)
	{
		for (j = 0; j < N; j +=BLOCK_SIZE)
		{
			for (k = 0; k < K; k +=BLOCK_SIZE)
			{
				M2 = MIN(BLOCK_SIZE, M-i);
				N2 = MIN(BLOCK_SIZE, N-j);
				K2 = MIN(BLOCK_SIZE, K-k);
				cblas_dgemm_scalaire(M2, K2, N2, alpha,
									 A+ i*lda +k, lda,
				 					 B+ j*ldb +k, ldb,
				 					 C+ j*ldc +i, ldc);
			}
		}
	}
}

void cblas_dgemm(const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_TRANSPOSE TransB, const int M, const int N,
                 const int K, const double alpha, const double *A,
                 const int lda, const double *B, const int ldb,
                 const double beta, double *C, const int ldc)
{
	if (Order == CblasRowMajor)
	{
		fprintf(stderr, "CblasRowMajor not supported. Reorder your matrices and use CblasColMajor instead\n");
		return;
	}
	if (TransA == CblasNoTrans || TransB == CblasTrans)
	{
		fprintf(stderr, "This implementation only supports C = C+alpha*trans(A)*B. Nothing has been done.\n");
		return;
	}

	cblas_dgemm_block(M, K, N, 
						alpha, A, lda,
					   	B, ldb,
					   	C, ldc);	
}