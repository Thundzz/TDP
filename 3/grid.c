#include <stdlib.h>
#include <mpi.h>

#include "grid.h"
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

void create_grid(int myrank, int np_r, int np_c,
					MPI_Comm* comm_row, MPI_Comm* comm_col)
{
	MPI_Comm comm_grid; 

	int dims[2] = {np_r, np_c};
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

