#include "util.h"
#include "cblas.h"
#include <stdlib.h>

#define IMAX 1000000



int main(void)
{
  double * a, *b, *c;
  
  a = alloc(3,3);
  b = alloc(3,3);
  c = alloc(3,3);
  init_test(3, 3, a, 3);
  init_test(3, 3, b, 3);
  init_zero(3, 3, c, 3);


  /*Test de CLBAS DDOT avec IMAX 1000000*/
  //cblas_ddot(i, a, 1, b, 1);

  cblas_dgemm_scalaire(3, a, 3, b, 3, c, 3);


  affiche(3, 3,  a, 3, stdout);
  affiche(3, 3,  b, 3, stdout);  
  affiche(3, 3,  c, 3, stdout);
  free(a);
  free(b);
  free(c);
  return 0;
}
