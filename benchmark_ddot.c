#include "perf.h"
#include "cblas.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>

#define NB_ITER 100000000
#define IMAX 100000000
#define IMIN 50

void test_ddot(const char * fileName)
{
  perf_t start;
  perf_t stop;
  //initialize arrays
  double * a, *b;
  int i,j, nbIter;
  FILE* output;
  output = fopen(fileName, "w+"); 

  
  for (i = IMIN; i<IMAX; i*=1.25)
  { 
    a = alloc(i, 1);
    b = alloc(i, 1);
    init_rand(i, 1, a, i);
    init_rand(i, 1, b, i);

    //ddot timing
    nbIter = NB_ITER / i; 
    perf(&start);
    for (j = 0; j< nbIter;j++)
    {
      cblas_ddot(i, a, 1, b, 1);
    }
    perf(&stop);
    //End of ddot timing

    free(a);
    free(b);
    perf_diff(&start,&stop);

    perf_printh(&stop);
    perf_printmicro(&stop);

    double mflops = perf_mflops(&stop, 2*i*nbIter);

    printf("i : %d => Mflops : %.4f\n",i, mflops);
    fprintf(output, "%d %.4f\n", i, mflops);
  }

  fclose(output);
}


int main(int argc, char ** argv) {
  (argc>= 2)? test_ddot(argv[1]) : test_ddot("ddot.txt");
  return 0;
}