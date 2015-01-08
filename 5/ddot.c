#include "cblas.h"

//produit scalaire : res = X.Y 
double cblas_ddot(const int N, const double *X, const int incX,
                  const double *Y, const int incY)
{
  int i;
  double res= 0;
  for(i = 0; i < N; i ++)
  {
    res += X[i * incX] * Y[i* incY];
  }
  return res;
}
