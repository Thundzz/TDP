#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mpi.h"
#include "matrix.h"

// void cblas_dgemm_scalaire(const int Nb, const double *A, const double *B, double *C)
// {
// 	int i, j, k;
// 	for (i = 0; i < Nb; ++i)
// 	{
// 		for (j = 0; j < Nb; ++j)
// 		{	
// 			for (k = 0; k < Nb; ++k)
// 			{
// 				C[i*Nb +k] += A[i*Nb +k]* B[j*Nb +k];
// 			}
// 		}
// 	}
// }



// void proc()
// {
// 	int k;
// 	for (k = 0; k < N; k+=Nb)
// 	{	
// 		/* If I am i+k%N proc of the line
// 		 * 	Bcast_line(A[i][i+k%N]) 
// 		 * Else
// 		 * 	recv(A) from the i+k%N proc of the line
// 		 */
// 		cblas_dgemm_scalaire(Nb, A, B, C);  //Cij = A[i][i+k%N]*B[i+k%N][j]
// 		/* send(B) to top neighbour
// 		 */
// 	}
// }

double* partition_matrix(double *a,
						int N, int np_r, int np_c, 
						MPI_Datatype *type_block)
{
	MPI_Datatype type_block_tmp;

	int NB_r = N/np_c; //Nb of lines per block
	int NB_c = N/np_r; //Nb of columns per block
	int np = np_r * np_c; //Nb of procs

	double* b = malloc(NB_r*NB_c*sizeof(double));

	MPI_Type_vector(NB_r, NB_c, N, MPI_DOUBLE, &type_block_tmp);
	MPI_Type_create_resized(type_block_tmp, 0, sizeof(double), type_block);
	MPI_Type_commit(type_block);

 	int counts[np];
	int disps[np];
	for (int i=0; i<np_c; i++) {
        for (int j=0; j<np_r; j++) {
            disps[i*np_r+j] = i*N*NB_r+j*NB_c;
            counts [i*np_r+j] = 1;
        }
    }
    MPI_Scatterv(a, counts, disps, *type_block, b, NB_r*NB_c, MPI_DOUBLE, 0, MPI_COMM_WORLD);	

    return b;
}

void create_grid(int myrank, int np_row, int np_col,
					MPI_Comm* comm_row, MPI_Comm* comm_col)
{
	MPI_Comm comm_grid; 

	int dims[2] = {np_row, np_col};
	int coords[2]; // coords[0] = i, coords[1] = j
	int periods[2];
	int reorder;

	int grid_rank;
	int subdivision[2];

	periods[0] = 0 ; 
	periods[1] = 1 ;
	reorder = 1 ;
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &comm_grid);

	MPI_Cart_coords(comm_grid, myrank, 2, coords); //Outputs the i,j coordinates of the process
	MPI_Cart_rank(comm_grid, coords, &grid_rank);  //Outputs the rank of the process

	subdivision[0] = 1;
	subdivision[1] = 0;
 	MPI_Cart_sub (comm_grid,subdivision,comm_col); // Communicator between lines
 	subdivision[0] = 0;
	subdivision[1] = 1; 
 	MPI_Cart_sub (comm_grid,subdivision,comm_row); // Communicator between row
}

int main(int argc, char** argv) {
	int np; 
	int myrank; 
	int N;

	MPI_Init( NULL, NULL ); 
	MPI_Comm_rank( MPI_COMM_WORLD, &myrank); 
	MPI_Comm_size(MPI_COMM_WORLD, &np);

	if(argc != 3)
	{
		if(myrank == 0){
			printf("Syntax: ./a.out 'nb_procs_per_row' 'nb_procs_per_col'\n");
		}
		MPI_Finalize();
		return -1;
	}

	int np_r = atoi(argv[1]); //Number of procs per row
	int np_c = atoi(argv[2]); //Number of procs per column

	if(np != (np_r * np_c))
	{
		if(myrank == 0){
			printf("Number of processes must be equal to the product of the two args\n");
		}
		MPI_Finalize();
		return -1;
	}

	//Global matrix
	matrix a;
	//double a[N*N];
	if(myrank == 0)
	{
		N = matrix_load(&a, "mat_grid.dat");
		// for (int i = 0; i < N*N; ++i)
		// {
		// 	a[i] = i;
		// }
	}
	MPI_Bcast (&N, 1, MPI_INT , 0, MPI_COMM_WORLD); 
	
	if(myrank == 0)
	{
		printf("=== Global matrix of dim %d===\n", N);
		for (int ii=0; ii<N; ii++) {
	        for (int jj=0; jj<N; jj++) {
	            printf("%g ", a.content[ii*N+jj]);
	        }
	        printf("\n");
	    }
	}
	//End of Global matrix

	if(N % np_r != 0 || N % np_c != 0)
	{
		if(myrank == 0){
			printf("Error: Grid dimensions must divide matrix dimensions\n");
			matrix_free(&a);
		}
		MPI_Finalize();
		return -1;
	}

	double* b;
	MPI_Datatype type_block;
	b = partition_matrix(a.content, N, np_r, np_c, &type_block);

 	MPI_Comm comm_row, comm_col; 
 	create_grid(myrank, np_r, np_c, &comm_row, &comm_col);

	//MPI_Bcast (b, NB*NB, MPI_DOUBLE , 0, comm_col); 

	for (int k = 0; k < np; ++k)
	{
		if(myrank == k)
		{
			printf("=== This is proc %d ===\n", k);
			for (int i=0; i<N/np_c; i++) {
		        for (int j=0; j<N/np_r; j++) {
		            printf("%g ", b[i*N/np_r+j]);
		        }
		        printf("\n");
		    }
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}

	if(myrank == 0)
		matrix_free(&a);
	free(b);
	MPI_Finalize();
	return 0;
}