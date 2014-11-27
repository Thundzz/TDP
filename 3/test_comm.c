#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "grid.h"

#define N 6
#define NB 2
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

void init_self_block(double* m, int myrank)
{
	int i;
	for(i = 0; i<NB*NB; i++)
	{
		m[i] = myrank;
	}
}

void test_bcast(double* A, int myrank, int np, MPI_Comm *comm)
{
	int i;
	
	MPI_Bcast(A, NB*NB, MPI_DOUBLE, 0, *comm);

	for(i = 0; i<np; i++)
	{
		if(myrank == i)
		{
			printf("=== Block on proc %d === \n", myrank);
			print_matrix(A, NB);
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}
}

void print_grid(double* A, int myrank, int np)
{
	int i;
	if(myrank == 0)
	{
		printf("=== Communication grid map === \n");
		printf("0 - 1 - 2\n"
				"|   |   |\n"
				"3 - 4 - 5\n"
				"|   |   |\n"
				"6 - 7 - 8\n");
	}

	for(i = 0; i<np; i++)
	{
		if(myrank == i)
		{
			printf("=== Block on proc %d === \n", myrank);
			print_matrix(A, NB);
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}

}

int main(void) 
{
	int np;
	int myrank; 
	MPI_Comm comm_row, comm_col, comm_grid; 

	MPI_Init( NULL, NULL ); 
	MPI_Comm_rank( MPI_COMM_WORLD, &myrank); 
	MPI_Comm_size(MPI_COMM_WORLD, &np);

	int gd = N/NB; //Grid dimensions 
	if(np != gd*gd)
	{
		if(myrank == 0)
			printf("Please restart the test with 9 processes");
		MPI_Finalize();
		return 0;
	}

	double *A = malloc(NB*NB*sizeof(double));
	init_self_block(A, myrank);
	create_grid(myrank, gd, &comm_grid, &comm_row, &comm_col);
	print_grid(A, myrank, np);


	if(myrank == 0)
	{
		printf("Test 1 comm : Broadcast line\n");
		printf("First of each row broadcasts his matrix to the row\n");
	}
	test_bcast(A, myrank, np, &comm_row);
	if(myrank == 0)
	{
		printf("Test 2 comm : Broadcast column\n");
		printf("First of each column broadcasts his matrix to the row\n");
	}
	test_bcast(A, myrank, np, &comm_col);

	free(A);
	return 0;
}