#include <stdio.h>
#include <string.h>
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


int main(int argc, char** argv) {
	int nb_procs; 
	int myrank; 
	MPI_Comm comm_grid, comm_row, comm_col; 
	MPI_Datatype type_block;

	MPI_Init( NULL, NULL ); 
	MPI_Comm_rank( MPI_COMM_WORLD, &myrank); 
	MPI_Comm_size(MPI_COMM_WORLD, &nb_procs);
	
	//double * a;
	//if (myrank == 0)
	//	a = matrix_load();
	int N = 16;
	//Global matrix
	double a[N];
	if(myrank == 0)
	{
		for (int i = 0; i < N; ++i)
		{
			a[i] = i;
		}
	}
	//Local matrix
	double b[4];
    for (int j=0; j<4; j++) 
    	b[j] = 0;

	MPI_Type_vector(2, 2, N, MPI_DOUBLE, type_block_tmp);
	MPI_Type_create_resized(type_block_tmp, 0, 2*sizeof(double), &type_block);
	MPI_Type_commit(&type_block);

	int dims[2] = {2,2};
	int coords[2]; // coords[0] = i, coords[1] = j
	int periods[2];
	int reorder;

	int grid_rank;
	int subdivision[2];

	//Receive matrix size from file
	//dims[0] = dims[1] = matrix_size/nb_procs + (matrix_size%nb_procs != 0); // Ceil round

	periods[0] = 0 ; 
	periods[1] = 1 ;
	reorder = 1 ;
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &comm_grid);

	MPI_Cart_coords(comm_grid, myrank, 2, coords); //Outputs the i,j coordinates of the process
	MPI_Cart_rank(comm_grid, coords, &grid_rank);  //Outputs the rank of the process

	subdivision[0] = 1;
	subdivision[1] = 0;
 	MPI_Cart_sub (comm_grid,subdivision,&comm_col); // Communicator between lines
 	subdivision[0] = 0;
	subdivision[1] = 1; 
 	MPI_Cart_sub (comm_grid,subdivision,&comm_row); // Communicator between row

 	int counts[4];
	int disps[4];
	for (int ii=0; ii<2; ii++) {
        for (int jj=0; jj<2; jj++) {
            disps[ii*2+jj] = ii*4*2+jj*2;
            counts [ii*2+jj] = 1;
        }
    }

	MPI_Scatterv(a, counts, disps, type_block, b, 4, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	// MPI_Bcast (&W, 1, MPI_INT ,1, comm_row); 

	for (int ii=0; ii<2; ii++) {
        for (int jj=0; jj<2; jj++) {
            disps[ii*2+jj] = ii*4*2+jj*2;
            counts [ii*2+jj] = 1;
        }
    }

	MPI_Finalize();
	return 0;
}