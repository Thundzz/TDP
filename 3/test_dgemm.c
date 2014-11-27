#include <stdio.h>
#include <stdlib.h>
#include "grid.h"

#define N 3
#define NBTESTS 2
#define TEST_SUCCESS 1
#define TEST_FAILURE 0

int check_eq(double* test, double* ref)
{
  int i;
  for (i = 0; i < N; i++)
  {
    if(test[i] != ref[i])
      return TEST_FAILURE;
  }
  return TEST_SUCCESS;
}

void init_zero(double* m)
{
  int i;
  for (i = 0; i < N*N; ++i)
  {
    m[i] = 0;
  }
}

void init_id(double* m)
{
  int i, j;
  for(i = 0; i<N; i++)
  {
    for(j = 0; j<N; j++)
    {
      if (i==j)
        m[i*N+j] = 1;
      else
        m[i*N+j] = 0;
    } 
  }
}

int main(void) {
  double *A = malloc(N*N*sizeof(double));
  double *B = malloc(N*N*sizeof(double));
  double *C = malloc(N*N*sizeof(double));
  double realC[N*N];

  int i, j, res = 2;
  for(i = 0; i<N; i++)
  {
    for(j = 0; j<N; j++)
    {
      B[i*N+j] = i*N+j;
    }
  }
  init_zero(C);
  init_id(A);

  printf("Test 1 dgemm : A=id, B=quelconque\n");
  cblas_dgemm_scalaire(N, A, B, C);
  if (!check_eq(C, B))
  {
    fprintf(stderr, "Test 1 dgemm KO\n");
    res--;
  }

  init_zero(C);
  printf("Test 2 dgemm : A=quelconque, B=id\n");
  cblas_dgemm_scalaire(N, B, A, C);
  if (!check_eq(C, B))
  {
    fprintf(stderr, "Test 2 dgemm KO\n");
    res--;
  }

  printf("dgemm : %d/%d tests successful\n", res, NBTESTS);
  free(A);
  free(B);
  free(C);
  return 0;
}