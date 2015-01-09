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
		if(proc_num_cols % 2 == 0 && myrank <= r-1)
			proc_num_cols ++;
		else if(proc_num_cols % 2 != 0 && myrank >= r)
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