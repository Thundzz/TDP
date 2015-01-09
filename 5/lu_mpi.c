#include "util.h"
#include "mpi.h"
#include "comm.h"
#include "cblas.h"
#include "mylapack.h"
#include <stdio.h>
#include <stdlib.h>

void lu_mpi(int myrank, int nb_processes, int mat_size)
{
	/*Initialize global matrix on proc 0*/
	int nb_cols = mat_size/BSZ;
	int col_size = mat_size*BSZ;

	double* A = NULL;
	double* L = NULL;
	double* U = NULL;

	if(myrank == 0)
	{
		A = malloc(mat_size*mat_size*sizeof(double));
		L = malloc(mat_size*mat_size*sizeof(double));
		U = malloc(mat_size*mat_size*sizeof(double));

		MATRIX_init_lower(mat_size, L, mat_size);
		MATRIX_init_upper(mat_size, U, mat_size);
		cblas_dgemm_scalaire(mat_size, mat_size, mat_size,1.0,
								  L, mat_size,
					 			  U, mat_size,
	                 			  A, mat_size);
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

	double start, end;

	lu_scatter_columns(myrank, nb_processes, A, nb_cols, col_size, local, cols, proc_num_cols);
	start = MPI_Wtime();
	lu_mpi_process(myrank, nb_processes, cols, proc_num_cols, nb_cols, mat_size, BSZ);
	end = MPI_Wtime() - start;
	lu_gather_columns(myrank,A, nb_cols, col_size,
						cols, local, proc_num_cols, type_column);


	MPI_Allreduce(MPI_IN_PLACE, &end, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

	if(myrank == 0)
	{
		char timefile[20];
		sprintf(timefile, "time%d.dat", nb_processes);
		FILE* f= fopen(timefile, "w");
		if(f != NULL)
		{
			fprintf(f, "%g\n", end);
		}
		fclose(f);

		free(L);
		free(U);
		free(A);
	}

}

int main(int argc, char** argv)
{
	int myrank, nb_processes, matsize;
	MPI_Init( NULL, NULL ); 
	MPI_Comm_rank( MPI_COMM_WORLD, &myrank ); 
	MPI_Comm_size( MPI_COMM_WORLD, &nb_processes);

	if(argc != 2)
	{
		if(myrank == 0)
			printf("Please set the size of the matrix\n");
		MPI_Finalize();
		return -1;
	}

	else
	{
		matsize = atoi(argv[1]);
		lu_mpi(myrank, nb_processes, matsize);
	}

	MPI_Finalize();
	return 0;
}