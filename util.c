#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "util.h"

static int SEEDED=0;

int * alloc(int m, int n)
{
  if(!SEEDED)
  {
    srand(time(NULL));
    SEEDED ++;
  }
  int * a = (int *) malloc(m* n * sizeof(int));
  return a;
}

void unalloc(int * a)
{
  free(a);
}

void init_rand(int m, int n, int * a, int lda)
{
  int i, j;
  for(i= 0; i< m; i++){
    for(j=0; j <n; j++){
      a[j*lda + i] = rand();
    }
  }
}

void init_test(int m, int n, int * a, int lda)
{
  int i, j;
  for(i= 0; i< m; i++){
    for(j=0; j <n; j++){
      a[j*lda + i] = i +j*m;
    }
  } 
}


void affiche(int m, int n, int * a, int lda, FILE* flux)
{
  int i, j, aij;
  for(i= 0; i< m; i++){
    for(j=0; j <n; j++){
      aij = a[j*lda + i];
      fprintf(flux, "%d ", aij);
    }
    fprintf(flux, "\n");
  }
}
