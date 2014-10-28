#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

double * alloc(int m, int n);

void init_rand(int m, int n, double * a, int lda);

void init_test(int m, int n, double * a, int lda);

void init_zero(int m, int n, double * a, int lda);

void affiche(int m, int n, double * a, int lda, FILE* flux);


void cblas_dgemm_scalaire_kij(const int M,
				 const double *A, const int lda,
				 const double *B, const int ldb,
                 double *C, const int ldc);


void cblas_dgemm_scalaire_ikj(const int M,
				 const double *A, const int lda,
				 const double *B, const int ldb,
                 double *C, const int ldc);
#endif
