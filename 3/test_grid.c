#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>

#include "matrix.h"
#include "grid.h"

#define PRINT_ERROR(prnt) do{if(myrank == 0){ printf(prnt);}} while(0)

int main(int argc, char** argv) {
	int np;
	int myrank; 

	MPI_Init( NULL, NULL ); 
	MPI_Comm_rank( MPI_COMM_WORLD, &myrank); 
	MPI_Comm_size(MPI_COMM_WORLD, &np);

	double debut = MPI_Wtime();
	double fin;
	if(argc != 5)
	{
		PRINT_ERROR("Syntax: ./a.out 'input_file_A' 'input_file_B' 'output_file' 'nb_iter' \n");
		MPI_Finalize();
		return -1;
	}
	int nb_iter = atoi(argv[4]);
	//Checks if np is a squared number, as grid must be perfect.
	int gd = sqrt(np);
	if(gd*gd != np)
	{
		PRINT_ERROR("Number of processes must be a perfect square\n");
		MPI_Finalize();
		return -1;
	}

	//Global matrixes
	int N, M;
	matrix a;
	matrix b;
	if(myrank == 0)
	{
		N = matrix_load(&a, argv[1]);
		M = matrix_load(&b, argv[2]);
	}
	MPI_Bcast (&N, 1, MPI_INT, 0, MPI_COMM_WORLD); 
	MPI_Bcast (&M, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (M != N)
	{
		if(myrank == 0){
			matrix_free(&a);
			matrix_free(&b);
		}
		PRINT_ERROR("Dimensions of A and B must be equal\n");
		MPI_Finalize();
		return -1;
	}

	if(gd > N || N % gd != 0)
	{
		if(myrank == 0){
			matrix_free(&a);
		}
		PRINT_ERROR("Error: Grid dimensions must divide matrix dimensions\n");
		MPI_Finalize();
		return -1;
	}

	double *bl_a, *bl_b;
	MPI_Datatype type_block;
	bl_a = partition_matrix(a.content, N, gd, &type_block);
	bl_b = partition_matrix(b.content, N, gd, &type_block);

 	MPI_Comm comm_row, comm_col, comm_grid; 
 	create_grid(myrank, gd, &comm_grid, &comm_row, &comm_col);


	double* bl_c = malloc(N/gd*N/gd*sizeof(double));
	for (int i = 0; i < N/gd*N/gd; ++i)
	{
		bl_c[i] = 0;
	}

	prod_matrix(N, N/gd, myrank,
			bl_a, bl_b, bl_c,
			comm_grid, comm_col, comm_row);

	matrix c = gather_matrix(bl_c, N, gd, &type_block);

	if(myrank == 0)
	{
	    matrix_store(argv[3], &c);
	}
	if(myrank == 0){
		matrix_free(&a);
		matrix_free(&b);
	}
	matrix_free(&c);
	free(bl_a);
	free(bl_b);
	free(bl_c);


	fin = MPI_Wtime();

	printf("Time spent: %g\n", fin-debut);
	MPI_Finalize();
	return 0;
}