#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "util.h"

static int SEEDED=0;

double * alloc(int m, int n)
{
  if(!SEEDED)
  {
    srand(time(NULL));
    SEEDED ++;
  }
  double * a = (double *) malloc(m* n * sizeof(double));
  return a;
}

void init_rand(int m, int n, double * a, int lda)
{
  int i, j;
  for(i= 0; i< m; i++){
    for(j=0; j <n; j++){
      a[j*lda + i] = rand();
    }
  }
}

void init_test(int m, int n, double * a, int lda)
{
  int i, j;
  for(i= 0; i< m; i++){
    for(j=0; j <n; j++){
      a[j*lda + i] = i +j*m;
    }
  }
}

void init_zero(int m, int n, double * a, int lda)
{
  int i, j;
  for(i= 0; i< m; i++){
    for(j=0; j <n; j++){
      a[j*lda + i] = 0;
    }
  } 
}


void affiche(int m, int n, double * a, int lda, FILE* flux)
{
  int i, j; 
  double aij;
  for(i= 0; i< m; i++){
    for(j=0; j <n; j++){
      aij = a[j*lda + i];
      fprintf(flux, "%g ", aij);
    }
    fprintf(flux, "\n");
  }
}
