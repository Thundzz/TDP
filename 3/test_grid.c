#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "matrix.h"
#include "grid.h"

int main(int argc, char** argv) {
	int np; 
	int myrank; 

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

	//Global matrices
	int N, M;
	matrix a;
	matrix b;
	if(myrank == 0)
	{
		N = matrix_load(&a, "mat_grid.dat");
		M = matrix_load(&b, "mat_grid.dat");
	}
	MPI_Bcast (&N, 1, MPI_INT, 0, MPI_COMM_WORLD); 
	MPI_Bcast (&M, 1, MPI_INT, 0, MPI_COMM_WORLD);
	if (M != N)
	{
		if(myrank == 0){
			printf("Dimensions of A and B must be equal\n");
			matrix_free(&a);
			matrix_free(&b);
		}
		MPI_Finalize();
		return -1;
	}


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

	double *bl_a, *bl_b;
	MPI_Datatype type_block;
	bl_a = partition_matrix(a.content, N, np_r, np_c, &type_block);
	bl_b = partition_matrix(b.content, N, np_r, np_c, &type_block);

 	MPI_Comm comm_row, comm_col, comm_grid; 
 	create_grid(myrank, np_r, np_c, &comm_grid, &comm_row, &comm_col);

	for (int k = 0; k < np; ++k)
	{
		if(myrank == k)
		{
			printf("=== This is proc %d ===\n", k);
			for (int i=0; i<N/np_c; i++) {
		        for (int j=0; j<N/np_r; j++) {
		            printf("%g ", bl_b[i*N/np_r+j]);
		        }
		        printf("\n");
		    }
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}

	double* bl_c = malloc(N/np_c*N/np_r*sizeof(double));
	for (int i = 0; i < N/np_c*N/np_r; ++i)
	{
		bl_c[i] = 0;
	}

	proc(N, N/np_c, 
			bl_a, bl_b, bl_c,
			myrank, np, np_c,
			comm_grid, comm_col, comm_row);

	for (int k = 0; k < np; ++k)
	{
		if(myrank == k)
		{
			printf("=== This is proc %d ===\n", k);
			for (int i=0; i<N/np_c; i++) {
		        for (int j=0; j<N/np_r; j++) {
		            printf("%g ", bl_c[i*N/np_r+j]);
		        }
		        printf("\n");
		    }
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}

	matrix c;
	matrix_zero(&c, N);
	int counts[np];
	int disps[np];
	for (int i=0; i<np_c; i++) {
        for (int j=0; j<np_r; j++) {
            disps[i*np_r+j] = i*N*N/np_c+j*N/np_r;
            counts [i*np_r+j] = 1;
        }
    }
	MPI_Gatherv(bl_c, N/np_c*N/np_r, MPI_DOUBLE,
               c.content, counts, disps, type_block,
               0, MPI_COMM_WORLD);

	if(myrank == 0)
	{
		printf("=== Final matrix ===\n");
		for (int i=0; i<N; i++) {
		    for (int j=0; j<N; j++) {
		        printf("%g ", c.content[i*N+j]);
		    }
		    printf("\n");
	    }
	    matrix_store("output.dat", &c);
	}
	if(myrank == 0){
		matrix_free(&a);
		matrix_free(&b);
	}
	matrix_free(&c);
	free(bl_a);
	free(bl_b);
	free(bl_c);

	MPI_Finalize();
	return 0;
}