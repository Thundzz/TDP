#include <stdlib.h>
#include <stdio.h>
#include "cblas.h"
#include "util.h"

void cblas_dger(const enum CBLAS_ORDER order, const int M, const int N,
                const double alpha, const double *X, const int incX,
                const double *Y, const int incY, double *A, const int lda)
{
	int i,j;
	if (order == CblasRowMajor)
	{
		fprintf(stderr, "CblasRowMajor not supported. Reorder your matrices and use CblasColMajor instead\n");
		return;
	}
	else
	{
		for (i = 0; i < M; ++i)
		{
			for (j = 0; j < N; ++j)
			{
				A[j*lda+i] += alpha * X[i*incX] * Y[j*incY];
			}
		}
	}
}