#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define MAX(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define MIN(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })


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

void MATRIX_copie(double* c, int m, int n, double * a, int lda);

#endif
