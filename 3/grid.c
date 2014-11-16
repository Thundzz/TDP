#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mpi.h"

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
						int N, int np_row, int np_col, 
						MPI_Datatype *type_block)
{
	MPI_Datatype type_block_tmp;

	int NB_r = N/np_row; //Nb of lines per block
	int NB_c = N/np_col; //Nb of columns per block
	int np = np_row * np_col; //Nb of procs

	double* b = malloc(NB_r*NB_c*sizeof(double));

	MPI_Type_vector(NB_r, NB_c, N, MPI_DOUBLE, &type_block_tmp);
	MPI_Type_create_resized(type_block_tmp, 0, sizeof(double), type_block);
	MPI_Type_commit(type_block);

 	int counts[np];
	int disps[np];
	for (int i=0; i<np_row; i++) {
        for (int j=0; j<np_col; j++) {
            disps[i*np_col+j] = i*N*NB_r+j*NB_c;
            counts [i*np_col+j] = 1;
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

	MPI_Init( NULL, NULL ); 
	MPI_Comm_rank( MPI_COMM_WORLD, &myrank); 
	MPI_Comm_size(MPI_COMM_WORLD, &np);

	//Global matrix
	int N = 8;
	double a[N*N];
	if(myrank == 0)
	{
		for (int i = 0; i < N*N; ++i)
		{
			a[i] = i;
		}
	}

	if(myrank == 0)
	{
		printf("=== Global matrix ===\n");
		for (int ii=0; ii<N; ii++) {
	        for (int jj=0; jj<N; jj++) {
	            printf("%g ", a[ii*N+jj]);
	        }
	        printf("\n");
	    }
	}
	//End of Global matrix

	int NB = sqrt(N*N/np); //Dimension of a block
	
	double* b;
	MPI_Datatype type_block;
	b = partition_matrix(a, N, N/NB, N/NB, &type_block);

 	MPI_Comm comm_row, comm_col; 
 	create_grid(myrank, N/NB, N/NB, &comm_row, &comm_col);

	//MPI_Bcast (b, NB*NB, MPI_DOUBLE , 0, comm_col); 

	for (int i = 0; i < np; ++i)
	{
		if(myrank == i)
		{
			printf("=== This is proc %d ===\n", i);
			for (int ii=0; ii<NB; ii++) {
		        for (int jj=0; jj<NB; jj++) {
		            printf("%g ", b[ii*NB+jj]);
		        }
		        printf("\n");
		    }
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}

	free(b);
	MPI_Finalize();
	return 0;
}