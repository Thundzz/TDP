#include <stdio.h>
#include "cblas.h"

void cblas_dgemv(const enum CBLAS_ORDER order,
                 const enum CBLAS_TRANSPOSE TransA, const int M, const int N,
                 const double alpha, const double *A, const int lda,
                 const double *X, const int incX, const double beta,
                 double *Y, const int incY)
{
	int i, j;
	if (order == CblasRowMajor)
	{
		fprintf(stderr, "CblasRowMajor not supported. Reorder your matrices and use CblasColMajor instead\n");
		return;
	}
	if (TransA == CblasConjTrans)
		fprintf(stderr, "CblasConjTrans not supported. The program will use CblasTrans instead\n");
	else
	{
		for (i = 0; i < M; ++i)
		{
			Y[i*incY] *= beta;
			for (j = 0; j < N; ++j)
			{
				if(TransA == CblasNoTrans)
					Y[i*incY] += alpha*A[j*lda+i]*X[j*incX];
				else if(TransA == CblasTrans || TransA == CblasConjTrans)
					Y[i*incY] += alpha*A[i*lda+j]*X[j*incX];
			}
		}
	}
}