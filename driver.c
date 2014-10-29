#include "util.h"
#include "cblas.h"
#include <stdlib.h>
#include <stdio.h>

#define SIZE 10

void unit_test_ddot()
{
  double *a, *b;
  a = alloc(SIZE, 1);
  b = alloc(SIZE, 1);
  init_test(SIZE, 1, a, 1);
  init_test(SIZE, 1, b, 1);
  
  double res = cblas_ddot(SIZE, a, 1, b, 1);
  affiche(SIZE, 1, a, 1, stdout);
  affiche(SIZE, 1, b, 1, stdout);
  printf("result = %g\n", res);

  free(a);
  free(b);
}

/* transpose([[0,3,6],[1,4,7],[2,5,8]]) * [[0,3,6],[1,4,7],[2,5,8]] */
void unit_test_dgemm()
{
  double * a, *b, *c;
  a = alloc(3,3);
  b = alloc(3,3);
  c = alloc(3,3);
  init_test(3, 3, a, 3);
  init_test(3, 3, b, 3);
  init_zero(3, 3, c, 3);

  cblas_dgemm_scalaire_ikj(3, a, 3, b, 3, c, 3);

  affiche(3, 3,  a, 3, stdout);
  affiche(3, 3,  b, 3, stdout);  
  affiche(3, 3,  c, 3, stdout);
  free(a);
  free(b);
  free(c);
}

void unit_test_daxpy()
{
  double *a, *b;
  a = alloc(SIZE, 1);
  b = alloc(SIZE, 1);
  init_test(SIZE, 1, a, 1);
  init_test(SIZE, 1, b, 1);
  
  printf("=== A === \n");
  affiche(SIZE, 1, a, 1, stdout);
  printf("=== B === \n");
  affiche(SIZE, 1, b, 1, stdout);

  cblas_daxpy(SIZE, 2, a, 1, b, 1);

  printf("=== B = alpha*A+B === \n");
  affiche(SIZE, 1, b, 1, stdout);

  free(a);
  free(b);
}

//[[0,3,6],[1,4,7],[2,5,8]]*[0,1,2]*1 + 5*[0,1,2]
void unit_test_dgemv()
{
  double * a, *b, *c;
  a = alloc(3,3);
  b = alloc(SIZE, 1);
  c = alloc(SIZE, 1);
  init_test(SIZE, 1, b, 1);
  init_test(SIZE, 1, c, 1);
  init_test(3, 3, a, 3);

  printf("=== A === \n");
  affiche(3, 3,  a, 3, stdout);
  printf("=== X === \n");
  affiche(3, 1,  b, 1, stdout);  
  printf("=== Y === \n");
  affiche(3, 1,  c, 1, stdout);

  cblas_dgemv(CblasColMajor, CblasTrans, 3, 3, 1, a, 3, b, 1, 5, c, 1); 

  printf("=== Y = alpha*A*X + beta*Y === \n");
  affiche(3, 1,  c, 1, stdout);
  free(a);
  free(b);
  free(c);
}

//[[0,3,6],[1,4,7],[2,5,8]]+ 3*[0,1,2]*transpose([0,1,2])
void unit_test_dger()
{
  double * a, *b, *c;
  a = alloc(3,3);
  b = alloc(SIZE, 1);
  c = alloc(SIZE, 1);
  init_test(SIZE, 1, b, 1);
  init_test(SIZE, 1, c, 1);
  init_test(3, 3, a, 3);

  printf("=== A === \n");
  affiche(3, 3,  a, 3, stdout);
  printf("=== X === \n");
  affiche(3, 1,  b, 1, stdout);  
  printf("=== Y === \n");
  affiche(3, 1,  c, 1, stdout);

  cblas_dger(CblasColMajor, 3, 3, 3, b, 1, c, 1, a, 3);

  printf("=== A = alpha*X*transpose(Y) + A === \n");
  affiche(3, 3,  a, 3, stdout);
  free(a);
  free(b);
  free(c);
}

int main(void)
{
  //unit_test_ddot();
  //unit_test_dgemm();  
  //unit_test_daxpy();
  unit_test_dgemv();
  //unit_test_dger();
  return 0;
}