#include "perf.h"
#include "util.h"
#include "cblas.h"
#include <stdlib.h>
#include <stdio.h>
#define NB_ITER 100
#define IMIN 1000
#define IMAX 1000000

void 
perf(perf_t * p) {
  gettimeofday(p, NULL);  
}

void 
perf_diff(const perf_t * begin, perf_t * end) {
  end->tv_sec = end->tv_sec - begin->tv_sec;
  end->tv_usec = end->tv_usec - begin->tv_usec;
  if (end->tv_usec < 0) {
    (end->tv_sec)--;
    end->tv_usec += 1000000;
  }
}

void
perf_printh(const perf_t * p) {
  long m = p->tv_sec / 60;
  long s = p->tv_sec - m*60;
  long ms = p->tv_usec / 1000;
  long micros = p->tv_usec - ms*1000;

  //  printf("%ld sec %ld usec\n", p->tv_sec, p->tv_usec);
  printf("%ld:%ld:%ld:%ld\n",m,s,ms,micros);
}

void
perf_printmicro(const perf_t * p) {
  printf("%ld\n",p->tv_usec + ( p->tv_sec * 1000000) );
}


double
perf_mflops(const perf_t * p, const long nb_op) {
  return (double)nb_op / (p->tv_sec * 1000000 + p->tv_usec);
}


int 
main() {
  perf_t start;
  perf_t stop;

//initialize arrays
  double * a, *b;
  int i,j;
  FILE* output;
  
  output = fopen("out.txt", "w+"); 
  a = alloc(IMAX, 1);
  b = alloc(IMAX, 1);
  init_test(IMAX, 1, a, IMAX);
  init_test(IMAX, 1, b, IMAX);
  
  for (i = IMIN; i<IMAX; i*=1.25)
  { 
  //ddot timing
      perf(&start);
      for (j = 0; j<NB_ITER;j++)
      {
        cblas_ddot(i, a, 1, b, 1);
      }
      perf(&stop);
    //End of ddot timing

      perf_diff(&start,&stop);

      perf_printh(&stop);
      perf_printmicro(&stop);
       
      double mflops = perf_mflops(&stop, 1000000)*NB_ITER;

    printf("i : %d => Mflops : %.4f\n",i, mflops);
    fprintf(output, "%d %.4f\n", i, mflops);
  }
  
  free(a);
  free(b);
  fclose(output);
  return 0;
}
