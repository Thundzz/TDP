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

	MPI_Init( NULL, NULL ); 
	MPI_Comm_rank( MPI_COMM_WORLD, &myrank); 
	MPI_Comm_size(MPI_COMM_WORLD, &nb_procs);
	 
	int dims[2] = {3,3};
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

	//Fill the processes according to their position

	int W;
	if (myrank == 1)
		W = 0;
	else
 		W = (int)myrank+10;

	subdivision[0] = 1;
	subdivision[1] = 0; 
 	MPI_Cart_sub (comm_grid,subdivision,&comm_col); // Communicator between lines
 	subdivision[0] = 0;
	subdivision[1] = 1; 
 	MPI_Cart_sub (comm_grid,subdivision,&comm_row); // Communicator between row

	//MPI_Scatter (V, 1, MPI_INT ,&W,1, MPI_INT ,1, comm_row);
	MPI_Bcast (&W, 1, MPI_INT ,1, comm_row); 

	printf("Rang %2d Coordonnées (%d,%d) : W %d.\n",myrank,coords[0],coords[1],W); 

	MPI_Finalize();
	return 0;
}