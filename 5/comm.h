#ifndef __COMM_H__
#define __COMM_H__

#include "mpi.h"

void create_column_type(MPI_Datatype * type_column, int col_size);

int get_proc_num_cols(int myrank, int nb_cols, int nb_processes);

void MATRIX_dispatch(int myrank, int nb_processes, double* A, int nb_cols, int col_size,
					double** cols, int proc_num_cols, int* local, 
					MPI_Datatype type_column);

void MATRIX_collect(int myrank, double* A, int nb_cols, int col_size,
					double** cols, int proc_num_cols, int* local, 
					MPI_Datatype type_column);

#endif