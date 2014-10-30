#include "cblas.h"

double cblas_ddot(const int N, const double *X, const int incX,
                  const double *Y, const int incY)
{
  int i;
  double res= X[0] + Y[0];
  for(i = 1; i < N; i ++)
  {
    res += X[i * incX] * Y[i* incY];
  }
  return res;
}
