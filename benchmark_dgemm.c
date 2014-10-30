#include "perf.h"
#include "cblas.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>

#define NB_ITER 1
#define IMAX 1000
#define IMIN 100


/*Possible combinations : kij, ijk, ikj, jik*/
void test_dgemm()
{
  perf_t start;
  perf_t stop;
  //initialize arrays
  double * a, *b, *c;
  int i,j;
  FILE* output;
  
  output = fopen("dgemmscal.txt", "w+"); 


  for (i = IMIN; i<IMAX; i+=10)
  {
      a = alloc(i, i);
      b = alloc(i, i);
      c = alloc(i, i);
      init_rand(i, i, a, i);
      init_rand(i, i, b, i);
      init_zero(i, i, c, i); 
      //ddot timing
      perf(&start);
      for (j = 0; j<NB_ITER ;j++)
      {
        cblas_dgemm_scalaire(i, i, i, a, i, b, i, c, i);
      }

      perf(&stop);
      //End of ddot timing
      
      free(a);
      free(b);
      free(c);

      perf_diff(&start,&stop);

      perf_printh(&stop);
      perf_printmicro(&stop);
       
      double mflops = perf_mflops(&stop, 2*i*i*i)*NB_ITER;

    printf("i : %d => Mflops : %.4f\n",i, mflops);
    fprintf(output, "%d %.4f\n", i, mflops);
  }
  fclose(output);
}

int main(void) {
  test_dgemm();
  return 0;
}