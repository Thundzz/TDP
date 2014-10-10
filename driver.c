#include "util.h"
#include "cblas.h"
#include <stdlib.h>

#define IMIN 50
#define IMAX 1000000

int main(void)
{
  double * a, *b;
  int i;
  
  a = alloc(IMAX, 1);
  b = alloc(IMAX, 1);
  init_test(IMAX, 1, a, IMAX);
  init_test(IMAX, 1, b, IMAX);

  for (i = IMIN; i<IMAX; i*=1.25)
  { 
  //ddot timing
    cblas_ddot(i, a, 1, b, 1);
  //End of ddot timing
  }
  free(a);
  free(b);
  return 0;
}
