#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "util.h"

static int SEEDED=0;

//Alloue un tableau de doubles de taille MxN
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

//Remplit aléatoirement une matrice A de taille MxN et de leading dimension lda  
void MATRIX_init_rand(int m, int n, double * a, int lda)
{
  int i, j;
  for(i= 0; i< m; i++){
    for(j=0; j <n; j++){
      a[j*lda + i] = rand();
    }
  }
}

//Remplit une matrice A de taille MxN par les nombres de 0 à MxN
void MATRIX_init_test(int m, int n, double * a, int lda)
{
  int i, j;
  for(i= 0; i< m; i++){ 
    for(j=0; j <n; j++){
      a[j*lda + i] = i +j*m;
    }
  }
}

//Initialise A <- Matrice identité
void MATRIX_init_id(int m, int n, double * a, int lda)
{
  int i, z;
  MATRIX_init_zero(m, n, a, lda);
  z = MIN(m, n);
  for (i = 0; i<z; i++)
  {
    a[i*lda+i] = 1;
  }
}

//Initialise A = 0
void MATRIX_init_zero(int m, int n, double * a, int lda)
{
  int i, j;
  for(i= 0; i< m; i++){
    for(j=0; j <n; j++){
      a[j*lda + i] = 0;
    }
  } 
}

void MATRIX_init_lower(int m, double* a, int lda){
  int i, j;
  for(i= 0; i< m; i++){
    for(j=0; j <i+1; j++){
      if (i==j){
        a[j*lda + i] = 1 ;
      }
      else{
        a[j*lda + i] = i +j*m;
      }
    }
  } 
}
void MATRIX_init_upper(int m, double* a, int lda){
  int i, j;
  for(i= 0; i< m; i++){
    for(j=i; j < m; j++){
        a[j*lda + i] = i +j*m+1;
    }
  }
}

void MATRIX_affiche(int m, int n, double * a, int lda, FILE* flux)
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
  fprintf(flux, "\n");
}
