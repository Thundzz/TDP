#include "cblas.h"
#include "mylapack.h"
#include "util.h"
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define N 12
#define NB 3

void create_column_type(MPI_Datatype * type_column)
{
	MPI_Type_vector(1, N*NB, 1, MPI_DOUBLE, type_column);
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
		else if(proc_num_cols % 2 != 0 && myrank > r)
			proc_num_cols ++;
	}
	return proc_num_cols;
}

void MATRIX_dispatch(int myrank, int nb_processes, double* A, 
					double** cols, int proc_num_cols, int* local, 
					MPI_Datatype type_column)
{
	int nb_cols = N/NB;

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
				MPI_Send(A+i*(N*NB), 1, type_column, sndto, i, MPI_COMM_WORLD);
			}
			else //local column
			{
				for (int j = 0; j < N*NB; ++j)
				{
					cols[next_local_id][j] = A[i*(N*NB)+j];
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
			cols[recvd_cols][N*NB] = st.MPI_TAG;
		}
	}
}

void MATRIX_collect(int myrank, double* A, 
					double** cols, int proc_num_cols, int* local, 
					MPI_Datatype type_column)
{
	int nb_cols = N/NB;
	
	if(myrank == 0)
	{
		int next_local_id = 0;
		for (int colnum = 0; colnum < nb_cols; colnum++)	
		{	
			if(local[next_local_id] == colnum) //Local column
			{
				for (int j = 0; j < N*NB; ++j)
					A[colnum*(N*NB)+j] = cols[next_local_id][j];
				next_local_id++;
			}
			else
			{
				MPI_Status st;
				MPI_Recv(A+colnum*(N*NB), 1, type_column, MPI_ANY_SOURCE, colnum, MPI_COMM_WORLD, &st);
			}
		}
	}
	else
	{
		int colnum;
		for (int i = 0; i < proc_num_cols; ++i)
		{
			colnum = cols[i][N*NB];
			MPI_Send(cols[i], 1, type_column, 0, colnum, MPI_COMM_WORLD);
		}
	}
}

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

	/*Creation of a MPI Type column*/
	MPI_Datatype type_column;
	create_column_type(&type_column);		

	int nb_cols = N/NB;
	int col_size = N*NB;
	int proc_num_cols = get_proc_num_cols(myrank, nb_cols, nb_processes);
	double** cols = malloc(proc_num_cols*sizeof(double*));
	for (int i = 0; i < proc_num_cols; ++i)
	{
		cols[i] = malloc((col_size+1)*sizeof(double));
	}
	int* local = malloc(proc_num_cols*sizeof(int));


	MATRIX_dispatch(myrank, nb_processes, A, cols, proc_num_cols, local, type_column);
	/*Calculations...*/
	for (int i = 0; i < proc_num_cols; ++i)
	{
		for (int j = 0; j < N*NB; ++j)
		{
			cols[i][j] = myrank;
		}
	}
	MATRIX_collect(myrank, A, cols, proc_num_cols, local, type_column);
	
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