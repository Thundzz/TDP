#include "cblas.h"
#include "mylapack.h"
#include "util.h"
#include "mpi.h"
#include "comm.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define N 12
#define NB 3

int main(void)
{
	/* Initialisation des constantes MPI */
	int myrank, nb_processes;
	MPI_Init( NULL, NULL ); 
	MPI_Comm_rank( MPI_COMM_WORLD, &myrank ); 
	MPI_Comm_size( MPI_COMM_WORLD, &nb_processes);

	/*Generate matrix on P0*/
	double * A;
	if(myrank == 0)
	{
		A = alloc(N, N);
		MATRIX_init_test(N, N, A, N);
		MATRIX_affiche(N, N, A, N, stdout);
	}
	int nb_cols = N/NB;
	int col_size = N*NB;

	/*Creation of a MPI Type column*/
	MPI_Datatype type_column;
	create_column_type(&type_column, col_size);		

	int proc_num_cols = get_proc_num_cols(myrank, nb_cols, nb_processes);
	double** cols = malloc(proc_num_cols*sizeof(double*));
	for (int i = 0; i < proc_num_cols; ++i)
	{
		cols[i] = malloc((col_size+1)*sizeof(double));
	}
	int* local = malloc(proc_num_cols*sizeof(int));


	MATRIX_dispatch(myrank, nb_processes, A, nb_cols, col_size,
					cols, proc_num_cols, local, type_column);
	/*Calculations...*/
	for (int i = 0; i < proc_num_cols; ++i)
	{
		for (int j = 0; j < N*NB; ++j)
		{
			cols[i][j] = myrank;
		}
	}
	MATRIX_collect(myrank, A, nb_cols, col_size,
					cols, proc_num_cols, local, type_column);
	
	if(myrank == 0)
	{
		MATRIX_affiche(N, N, A, N, stdout);
	}

	for (int i = 0; i < proc_num_cols; ++i)
	{
		free(cols[i]);
	}
	free(cols);
	free(local);
	MPI_Finalize();
	return 0;
}