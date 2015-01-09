#include "cblas.h"
#include "mylapack.h"
#include "util.h"
#include "mpi.h"
#include "comm.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define N 12

void test_mpi_comm(int myrank, int nb_processes)
{
	/*Generate matrix on P0*/
	double * A = NULL;
	if(myrank == 0)
	{
		A = alloc(N, N);
		MATRIX_init_test(N, N, A, N);
		MATRIX_affiche(N, N, A, N, stdout);
	}
	int nb_cols = N/BSZ;
	int col_size = N*BSZ;

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
		for (int j = 0; j < N*BSZ; ++j)
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
}



void test_mpi_lu(int myrank, int nb_processes)
{
	/*Initialize global matrix on proc 0*/
	int nb_cols = N/BSZ;
	int col_size = N*BSZ;

	double* A = NULL;
	double* L = NULL;
	double* U = NULL;

	if(myrank == 0)
	{
		A = malloc(N*N*sizeof(double));
		L = malloc(N*N*sizeof(double));
		U = malloc(N*N*sizeof(double));

		MATRIX_init_lower(N, L, N);
		MATRIX_init_upper(N, U, N);
		printf("====== U ====== \n");
		MATRIX_affiche(N, N, U, N, stdout);
		printf("====== L ======\n");
		MATRIX_affiche(N, N, L, N, stdout);
		cblas_dgemm_scalaire(N, N, N,1.0,
								  L, N,
					 			  U, N,
	                 			  A, N);
		printf("====== A=LU =======\n");
		MATRIX_affiche(N, N, A, N, stdout);
	}

	MPI_Datatype type_column;
	create_column_type(&type_column, col_size);	

	int proc_num_cols = get_proc_num_cols(myrank, nb_cols, nb_processes);
	double** cols = malloc(proc_num_cols*sizeof(double*));
	for (int i = 0; i < proc_num_cols; ++i)
	{
		cols[i] = malloc((col_size+1)*sizeof(double));
	}
	int* local = malloc(proc_num_cols*sizeof(int));
	lu_scatter_columns(myrank, nb_processes, A, nb_cols, col_size, local, cols, proc_num_cols);

	lu_mpi_process(myrank, nb_processes, cols, proc_num_cols, nb_cols, N, BSZ);

	lu_gather_columns(myrank,A, nb_cols, col_size,
						cols, local, proc_num_cols, type_column);

	if(myrank == 0)
	{
		printf("LU decomposition of A\n");
		MATRIX_affiche(N, N, A, N, stdout);
		free(L);
		free(U);
		free(A);

	}

}

int main()
{
	int myrank, nb_processes;
	MPI_Init( NULL, NULL ); 
	MPI_Comm_rank( MPI_COMM_WORLD, &myrank ); 
	MPI_Comm_size( MPI_COMM_WORLD, &nb_processes);

	test_mpi_comm(myrank, nb_processes);
	test_mpi_lu(myrank, nb_processes);
	
	MPI_Finalize();
	return 0;
}