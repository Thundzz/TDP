#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "grid.h"

#define N 6
#define NB 3
#define TEST_SUCCESS 1
#define TEST_FAILURE 0

void print_matrix(double* A, int n)
{
  int i, j;
  for(i = 0; i<n; i++)
  {
    for(j = 0; j<n; j++)
    {
      printf("%g ", A[i*n+j]);
    } 
    printf("\n");
  }
}

void init_blocks(double* m)
{
  int i, j;
  for(i = 0; i<N; i++)
  {
    for(j = 0; j<N; j++)
    {
      if (i<NB && j<NB)   //Upper left block
        m[i*N+j] = 0;
      else if(i<NB && j>=NB) //Upper right block
        m[i*N+j] = 1;
      else if(i>=NB && j<NB) //Bottom left block
        m[i*N+j] = 2;
      else              //Bottom right block
        m[i*N+j] = 3;
    } 
  }
}

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

int check_block(double* test, int rank)
{
  int i;
  for (i = 0; i < N; i++)
  {
    if(test[i] != rank)
      return TEST_FAILURE;
  }
  return TEST_SUCCESS;
}

int main(void) 
{
  int np;
  int myrank;
  MPI_Datatype type_block;
  int error = 0;

  MPI_Init( NULL, NULL ); 
  MPI_Comm_rank( MPI_COMM_WORLD, &myrank); 
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  int gd = N/NB; //Grid dimensions 
  if(np != gd*gd)
  {
    if(myrank == 0)
      printf("Please restart the test with 4 processes");
    MPI_Finalize();
    return 0;
  }

  /* Test */
  double *A = malloc(N*N*sizeof(double)); //Initial matrix
  if(myrank == 0)
    init_blocks(A);

  double *bl_a = partition_matrix(A, N, gd, &type_block); //Scatter
  matrix mB = gather_matrix(bl_a, N, gd, &type_block); //Gather
  /* End Test */

  /* Check */
  if(myrank == 0)
  {
    printf("=== Global matrix === \n");
    print_matrix(A, N);
  }

  int i;
  for(i = 0; i<np; i++)
  {
    if(myrank == i)
    {
      printf("=== Block on proc %d === \n", myrank);
      print_matrix(bl_a, NB);
      if(!check_block(bl_a, myrank))
      {
        fprintf(stderr, "Error scatter on proc %d\n", myrank);
        error++;
      }
    }
    MPI_Barrier(MPI_COMM_WORLD);
  }

  if(myrank == 0)
  {
    printf("=== Rebuilt matrix === \n");
    print_matrix(mB.content, N);
    if(!check_eq(mB.content, A))
    {
        fprintf(stderr, "Error on gather\n");
        error++;
    }
    printf("scatter and gather test finished with %d errors\n", error);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  /* End Check */

  free(A);
  matrix_free(&mB);
  free(bl_a);

  return 0;
}