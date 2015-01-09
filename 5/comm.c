#include "cblas.h"
#include "mylapack.h"
#include "util.h"
#include "mpi.h"
#include "comm.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void create_column_type(MPI_Datatype * type_column, int col_size)
{
	MPI_Type_vector(1, col_size, 1, MPI_DOUBLE, type_column);
	MPI_Type_commit(type_column);
}


int get_proc_num_cols(int myrank, int nb_cols, int nb_processes)
{
	int r = nb_cols % nb_processes;
	int proc_num_cols = nb_cols/nb_processes; 
	if (r > 0)
	{
		if(proc_num_cols % 2 == 0 && myrank < r)
			proc_num_cols ++;
		else if(proc_num_cols % 2 != 0 && myrank >= nb_processes - r)
			proc_num_cols ++;
	}
	return proc_num_cols;
}

void MATRIX_dispatch(int myrank, int nb_processes, double* A, int nb_cols, int col_size, 
					double** cols, int proc_num_cols, int* local, 
					MPI_Datatype type_column)
{
	if(myrank == 0)
	{
		int next_local_id = 0;
		int sndto;
		for (int i = 0; i < nb_cols; i++)
		{
			if(i % nb_processes != i % (2*nb_processes))
				sndto = nb_processes - 1 - (i%nb_processes);
			else
				sndto = i%nb_processes;
			if(sndto != 0)
			{
				MPI_Send(A+i*(col_size), 1, type_column, sndto, i, MPI_COMM_WORLD);
			}
			else //local column
			{
				for (int j = 0; j < col_size; ++j)
				{
					cols[next_local_id][j] = A[i*(col_size)+j];
				}
				local[next_local_id] = i;
				next_local_id++;
			}
		}
	}

	else
	{
		MPI_Status st;
		for (int recvd_cols = 0; recvd_cols < proc_num_cols; recvd_cols++)	
		{
			MPI_Recv(cols[recvd_cols], 1, type_column, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &st);
			cols[recvd_cols][col_size] = st.MPI_TAG;
			local[recvd_cols] = st.MPI_TAG;
		}
	}
}

void MATRIX_collect(int myrank, double* A, int nb_cols, int col_size,
					double** cols, int proc_num_cols, int* local, 
					MPI_Datatype type_column)
{
	if(myrank == 0)
	{
		int next_local_id = 0;
		for (int colnum = 0; colnum < nb_cols; colnum++)	
		{	
			if(local[next_local_id] == colnum) //Local column
			{
				for (int j = 0; j < col_size; ++j)
					A[colnum*(col_size)+j] = cols[next_local_id][j];
				next_local_id++;
			}
			else
			{
				MPI_Status st;
				MPI_Recv(A+colnum*(col_size), 1, type_column, MPI_ANY_SOURCE, colnum, MPI_COMM_WORLD, &st);
			}
		}
	}
	else
	{
		int colnum;
		for (int i = 0; i < proc_num_cols; ++i)
		{
			colnum = local[i];
			MPI_Send(cols[i], 1, type_column, 0, colnum, MPI_COMM_WORLD);
		}
	}
}

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
					double** cols, int proc_num_cols, int nb_cols, int mb, int nb)
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
			LAPACKE_dgetrf(0, mb-nb*i, nb, &cols[next_col_id][nb*i], mb, NULL);
			MATRIX_copie(LU_tmp, mb, nb, cols[next_col_id], mb);
			next_col_id++;
		}
		MPI_Bcast(LU_tmp, mb*nb, MPI_DOUBLE, LU_from, MPI_COMM_WORLD);
		
		if(i < nb_cols-1)
		{
			for (int k = next_col_id; k < proc_num_cols; ++k)
			{
				LAPACKE_dtrsm(LAPACKE_LOWER, LAPACKE_UNIT,  nb, nb, 1,  &LU_tmp[nb*i],  mb, &cols[k][nb*i], mb);

				cblas_dgemm_scalaire(mb-nb*(i+1), nb, nb,
					   		-1.0, &LU_tmp[nb*(i+1)], mb,
					   		&cols[k][nb*i], mb,
					   		&cols[k][nb*(i+1)], mb);
			}
		}
	}
}