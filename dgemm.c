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
							  const double *A, const int lda,
				 			  const double *B, const int ldb,
                 			        double *C, const int ldc)
{
	int i, j, k;
	double cij;
	for (i = 0; i < M; ++i)
	{
		for (j = 0; j < N; ++j)
		{	
			cij = C[j*ldc +i];
			for (k = 0; k < K; ++k)
			{
				cij += A[k*lda +i]* B[j*ldb +k];
			}
			C[j*ldc +i] = cij;
		}
	}
}

void cblas_dgemm_block(const int M, const int K, const int N,
					   const double * A, const int lda,
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
				printf("M2:%d, N2:%d, K2:%d\n", M2, N2, K2);
				cblas_dgemm_scalaire(M2, K2, N2,
									 A+ k*lda +i, lda,
				 					 B+ j*ldb +k, ldb,
				 					 C+ j*ldc +i, ldc);
			}
		}
	}
}