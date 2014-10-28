#include "perf.h"
#include "cblas.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>

#define NB_ITER 1
#define IMAX 1000
#define IMIN 100

void test_dgemm()
{
  perf_t start;
  perf_t stop;
//initialize arrays
  double * a, *b, *c;
  int i,j;
  FILE* output;
  
  output = fopen("dgemmkij.txt", "w+"); 
  a = alloc(IMAX, IMAX);
  b = alloc(IMAX, IMAX);
  c = alloc(IMAX, IMAX);
  init_test(IMAX, IMAX, a, IMAX);
  init_test(IMAX, IMAX, b, IMAX);
  init_zero(IMAX, IMAX, c, IMAX);

  for (i = IMIN; i<IMAX; i+=10)
  { 
  //ddot timing
      perf(&start);
      for (j = 0; j<NB_ITER ;j++)
      {
        cblas_dgemm_scalaire_kij(i, a, IMAX, b, IMAX, c, IMAX);
      }
      perf(&stop);
    //End of ddot timing

      perf_diff(&start,&stop);

      perf_printh(&stop);
      perf_printmicro(&stop);
       
      double mflops = perf_mflops(&stop, 2*i*i*i)*NB_ITER;

    printf("i : %d => Mflops : %.4f\n",i, mflops);
    fprintf(output, "%d %.4f\n", i, mflops);
  }
  
  free(a);
  free(b);
  fclose(output);
}

int main(void) {
  test_dgemm();
  return 0;
}