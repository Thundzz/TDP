#include "perf.h"
#include "cblas.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int 
main() {
  perf_t start;
  perf_t stop;
  int i, nb_elements;
  for(nb_elements = 50; nb_elements < 100000000; nb_elements *= 1.25)
    {
      double *a = alloc(nb_elements, 1);
      double *b = alloc(nb_elements, 1);
      init_rand(nb_elements, 1, a, nb_elements);
      init_rand(nb_elements, 1, b, nb_elements);
      double mflops = 0;
      int nb_run = 100000000/nb_elements;
      perf(&start);
      for(i = nb_run; i--;)
	    {
	     cblas_ddot(nb_elements, a, 1, b, 1);
      }
      perf(&stop);
      perf_diff(&start,&stop);
      //perf_printh(&stop);
      //perf_printmicro(&stop);
      mflops = perf_mflops(&stop, nb_run*2*nb_elements);
      free(a);
      free(b);
      //mflops/=1000;
      printf("%.0f\t%.4f\n", (float)(nb_elements*2.0*(int)sizeof(double)/1024), mflops);
    }
    return 0;
}