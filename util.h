#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

int * alloc(int m, int n);

void unalloc(int * a);

void init_rand(int m, int n, int * a, int lda);

void init_test(int m, int n, int * a, int lda);

void affiche(int m, int n, int * a, int lda, FILE* flux);

#endif
