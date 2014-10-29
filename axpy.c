#include "cblas.h"

void cblas_daxpy(const int N, const double alpha, const double *X,
                 const int incX, double *Y, const int incY)
{
	int i;
	for (i = 0; i < N; ++i)
	{
		Y[i*incY] += alpha * X[i*incX]; 
	}
}