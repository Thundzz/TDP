#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

double * alloc(int m, int n);

void init_rand(int m, int n, double * a, int lda);

void init_test(int m, int n, double * a, int lda);

void affiche(int m, int n, double * a, int lda, FILE* flux);

#endif
