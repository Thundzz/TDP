#include "mpi.h"

void create_column_type(MPI_Datatype * type_column);

int get_proc_num_cols(int myrank, int nb_cols, int nb_processes);

void MATRIX_dispatch(int myrank, int nb_processes, double* A, 
					double** cols, int proc_num_cols, int* local, 
					MPI_Datatype type_column);

void MATRIX_collect(int myrank, double* A, 
					double** cols, int proc_num_cols, int* local, 
					MPI_Datatype type_column);