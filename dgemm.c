#include "cblas.h"




/*A[i][j] === j*lda + i*/

void cblas_dgemm_scalaire(const int M,
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

