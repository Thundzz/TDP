#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "matrix.h"
#include "grid.h"

int main(int argc, char** argv) {
	int np; 
	int myrank; 
	int N;

	MPI_Init( NULL, NULL ); 
	MPI_Comm_rank( MPI_COMM_WORLD, &myrank); 
	MPI_Comm_size(MPI_COMM_WORLD, &np);

	if(argc != 3)
	{
		if(myrank == 0){
			printf("Syntax: ./a.out 'nb_procs_per_row' 'nb_procs_per_col'\n");
		}
		MPI_Finalize();
		return -1;
	}

	int np_r = atoi(argv[1]); //Number of procs per row
	int np_c = atoi(argv[2]); //Number of procs per column

	if(np != (np_r * np_c))
	{
		if(myrank == 0){
			printf("Number of processes must be equal to the product of the two args\n");
		}
		MPI_Finalize();
		return -1;
	}

	//Global matrix
	matrix a;
	if(myrank == 0)
	{
		N = matrix_load(&a, "mat_grid.dat");
	}
	MPI_Bcast (&N, 1, MPI_INT , 0, MPI_COMM_WORLD); 
	
	if(myrank == 0)
	{
		printf("=== Global matrix of dim %d===\n", N);
		for (int ii=0; ii<N; ii++) {
	        for (int jj=0; jj<N; jj++) {
	            printf("%g ", a.content[ii*N+jj]);
	        }
	        printf("\n");
	    }
	}

	if(N % np_r != 0 || N % np_c != 0)
	{
		if(myrank == 0){
			printf("Error: Grid dimensions must divide matrix dimensions\n");
			matrix_free(&a);
		}
		MPI_Finalize();
		return -1;
	}

	double* b;
	MPI_Datatype type_block;
	b = partition_matrix(a.content, N, np_r, np_c, &type_block);

 	MPI_Comm comm_row, comm_col; 
 	create_grid(myrank, np_r, np_c, &comm_row, &comm_col);

	for (int k = 0; k < np; ++k)
	{
		if(myrank == k)
		{
			printf("=== This is proc %d ===\n", k);
			for (int i=0; i<N/np_c; i++) {
		        for (int j=0; j<N/np_r; j++) {
		            printf("%g ", b[i*N/np_r+j]);
		        }
		        printf("\n");
		    }
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}

	if(myrank == 0)
		matrix_free(&a);
	free(b);
	MPI_Finalize();
	return 0;
}