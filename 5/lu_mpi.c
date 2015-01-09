#include "util.h"
#include "mpi.h"
#include "comm.h"
#include "mylapack.h"
#include <stdio.h>
#include <stdlib.h>

void lu_scatter_columns(int myrank, int nb_processes, double* A, int nb_cols, int col_size, int* local,
						double** cols, int proc_num_cols)
{
	/*Creation of a MPI Type column*/
	MPI_Datatype type_column;
	create_column_type(&type_column, col_size);		

	/*Divide global matrix into columns and distribute to all processes*/
	MATRIX_dispatch(myrank, nb_processes, A, nb_cols, col_size,
					cols, proc_num_cols, local, type_column);
}

void lu_gather_columns(int myrank, double* A, int nb_cols, int col_size,
							double** cols, int* local, int proc_num_cols, MPI_Datatype type_column)
{
	/*Gather columns on proc 0*/
	MATRIX_collect(myrank, A, nb_cols, col_size,
				cols, proc_num_cols, local, type_column);
	

	/*Free columns*/
	for (int i = 0; i < proc_num_cols; ++i)
	{
		free(cols[i]);
	}
	free(cols);
	free(local);
}

void lu_mpi_process(int myrank, int nb_processes,
					double** cols, int* col_ids, int proc_num_cols, int nb_cols, int mb, int nb)
{
	int next_col_id = 0;
	
	int LU_from;
	double* LU_tmp = malloc(mb*nb*sizeof(double));
	
	for (int i = 0; i < nb_cols; ++i)
	{
		if(i % nb_processes != i % (2*nb_processes))
			LU_from = nb_processes - 1 - (i%nb_processes);
		else
			LU_from = i%nb_processes;
		if(myrank == LU_from)
		{
			// MATRIX_affiche(nb, nb, &cols[next_col_id][nb*i], mb, stdout);
			LAPACKE_dgetrf(0, mb-nb*i, nb, &cols[next_col_id][nb*i], mb, NULL);
			MATRIX_copie(LU_tmp, mb-nb*i, nb, &cols[next_col_id][nb*i], mb);
			next_col_id++;
		}
		MPI_Bcast(LU_tmp, mb*nb, MPI_DOUBLE, LU_from, MPI_COMM_WORLD);
		
		if  (i < nb_cols-1)
		{
			for (int k = 0; k < proc_num_cols; ++k)
			{
				if(col_ids[k] > i)
				{
					LAPACKE_dtrsm(LAPACKE_LOWER, LAPACKE_UNIT,  nb, nb, 1,  LU_tmp,  mb, &cols[k][nb*i], mb);
					
					// MATRIX_affiche(mb-nb*(i+1), nb, &LU_tmp[nb*(i+1)], mb, stdout);
					// MATRIX_affiche(nb, nb, &cols[k][nb*i], mb, stdout);		
					// MATRIX_affiche(mb-nb*(i+1), nb, &cols[k][nb*(i+1)], mb, stdout);		

					cblas_dgemm_scalaire(mb-nb*(i+1), nb, nb,
						   		-1.0, &LU_tmp[nb*(i+1)], mb,
						   		&cols[k][nb*i], mb,
						   		&cols[k][nb*(i+1)], mb);
				}
			}
		}
	}
	// if(myrank == 1){
	// 	MATRIX_affiche(mb, nb, cols[0], mb, stdout);		
	// 	MATRIX_affiche(mb, nb, cols[1], mb, stdout);		
	// }
	//free(LU_tmp);
}

#define N 6
#define NB 2

int main()
{
	int myrank, nb_processes;
	MPI_Init( NULL, NULL ); 
	MPI_Comm_rank( MPI_COMM_WORLD, &myrank ); 
	MPI_Comm_size( MPI_COMM_WORLD, &nb_processes);

	/*Initialize global matrix on proc 0*/
	int nb_cols = N/NB;
	int col_size = N*NB;
	double * A = NULL;
	if(myrank == 0)
	{
		double * L = alloc(N, N);
		double * U = alloc(N, N);
		A = alloc(N, N);

		MATRIX_init_lower(N, L, N);
		MATRIX_init_upper(N, U, N);
		MATRIX_affiche(N, N, U, N, stdout);
		MATRIX_affiche(N, N, L, N, stdout);
		cblas_dgemm_scalaire(N, N, N,1.0,
								  L, N,
					 			  U, N,
	                 			  A, N);

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

	lu_mpi_process(myrank, nb_processes, cols, local, proc_num_cols, nb_cols, N, NB);

	lu_gather_columns(myrank,A, nb_cols, col_size,
						cols, local, proc_num_cols, type_column);

	if(myrank == 0)
	{
		MATRIX_affiche(N, N, A, N, stdout);
		free(A);
	}

	return 0;
}