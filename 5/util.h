#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

#define MIN(a,b) ((a)<(b))?(a):(b)

double * alloc(int m, int n);

void MATRIX_init_rand(int m, int n, double * a, int lda);

/* Initializes a lower triangular matrix
* with an unitary diagonal.
*/
void MATRIX_init_lower(int m, double * a, int lda);

/* Initializes an upper triangular matrix
* with an null diagonal.
*/
void MATRIX_init_upper(int m, double* a, int lda);

void MATRIX_init_test(int m, int n, double * a, int lda);

void MATRIX_init_id(int m, int n, double * a, int lda);

void MATRIX_init_zero(int m, int n, double * a, int lda);

void MATRIX_init_example_A(int m, int n, double* a, int lda);

void MATRIX_init_example_b(int m, int n, double* a, int lda);

void MATRIX_init_example_x(int m, int n, double* a, int lda);

void MATRIX_affiche(int m, int n, double * a, int lda, FILE* flux);

#endif
