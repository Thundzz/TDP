#include "util.h"
#include "cblas.h"

int main(void)
{
  double res;
  double * a = alloc(3,3);
  init_test(3, 3, a, 3);
  affiche(3,3,a, 3, stdout);
  
  res = cblas_ddot(3, a, 3, a+1, 3);
  printf("\nres : %g\n", res); 
  return 0;
}
