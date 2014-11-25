#include <stdlib.h>
#include <mpi.h>

#include "grid.h"
#include "matrix.h"

void cblas_dgemm_scalaire(const int N,
	const double *A, const double *B, double *C)
{
	int i, j, k;
	for (i = 0; i < N; ++i)
	{
		for (j = 0; j < N; ++j)
		{	
			for (k = 0; k < N; ++k)
			{
				C[i*N +j] += A[i*N +k]* B[k*N +j];
			}
		}
	}
}

void prod_matrix(int N, int Nb, int myrank,
	double* bl_a, double* bl_b, double* bl_c,
	MPI_Comm comm_grid, MPI_Comm comm_col, MPI_Comm comm_row)
{
	int k;
	double my_a[Nb*Nb];
	int coords[2];
	MPI_Status st;

	int gd = N/Nb;
	for (int i = 0; i < Nb*Nb; ++i)
	{
		my_a[i] = bl_a[i];
	}

	MPI_Cart_coords(comm_grid, myrank, 2, coords);
	int sndto = (((coords[0]-1)%gd) +gd) %gd;
	int recvfrom = (coords[0]+1)%gd;
	int myrow = coords[0];
	int mycol = coords[1];

	for (k = 0; k < gd; k++)
	{	
		/* If I am i+k%N proc of the line
		 * 	Bcast_line(A[i][i+k%N]) 
		 * Else
		 * 	recv(A) from the i+k%N proc of the line
		 */

		 if(mycol == (myrow+k)%gd)
		 {
		 	for (int i = 0; i < Nb*Nb; ++i)
		 	{
		 		bl_a[i] = my_a[i];
		 	}
		 }
		MPI_Bcast(bl_a, N*N, MPI_DOUBLE, (myrow+k)%gd, comm_row);

		cblas_dgemm_scalaire(Nb, bl_a, bl_b, bl_c);  //Cij = A[i][i+k%N]*B[i+k%N][j]
			/* send(B) to upper neighbour
			 */
		MPI_Sendrecv_replace(bl_b, Nb*Nb, MPI_DOUBLE, sndto, 0, 
			recvfrom, 0, comm_col, &st);
	}
}

double* partition_matrix(double *a,
	int N, int gd, 
	MPI_Datatype *type_block)
{
	MPI_Datatype type_block_tmp;

	int NB = N/gd;

	double* b = malloc(NB*NB*sizeof(double));

	MPI_Type_vector(NB, NB, N, MPI_DOUBLE, &type_block_tmp);
	MPI_Type_create_resized(type_block_tmp, 0, sizeof(double), type_block);
	MPI_Type_commit(type_block);

	int counts[gd*gd];
	int disps[gd*gd];
	for (int i=0; i<gd; i++) {
		for (int j=0; j<gd; j++) {
			disps[i*gd+j] = i*N*NB+j*NB;
			counts [i*gd+j] = 1;
		}
	}
	MPI_Scatterv(a, counts, disps, *type_block, b, NB*NB, MPI_DOUBLE, 0, MPI_COMM_WORLD);	

	return b;
}

matrix gather_matrix(double* c,
	int N, int gd, 
	MPI_Datatype * type_block)
{
	matrix res;
	matrix_zero(&res, N);
	
	int NB = N/gd;

	int counts[gd*gd];
	int disps[gd*gd];
	for (int i=0; i<gd; i++) {
		for (int j=0; j<gd; j++) {
			disps[i*gd+j] = i*N*NB+j*NB;
			counts [i*gd+j] = 1;
		}
	}
	MPI_Gatherv(c, NB*NB, MPI_DOUBLE,
		res.content, counts, disps, *type_block,
		0, MPI_COMM_WORLD);

	return res;
}

void create_grid(int myrank, int gd,
	MPI_Comm* comm_grid, MPI_Comm* comm_row, MPI_Comm* comm_col)
{
	int dims[2] = {gd, gd};
	int coords[2]; // coords[0] = i, coords[1] = j
	int periods[2];
	int reorder;

	int grid_rank;
	int subdivision[2];

	periods[0] = 0 ; 
	periods[1] = 1 ;
	reorder = 1 ;
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, comm_grid);

	MPI_Cart_coords(*comm_grid, myrank, 2, coords); //Outputs the i,j coordinates of the process
	MPI_Cart_rank(*comm_grid, coords, &grid_rank);  //Outputs the rank of the process

	subdivision[0] = 1;
	subdivision[1] = 0;
 	MPI_Cart_sub (*comm_grid,subdivision,comm_col); // Communicator between lines
 	subdivision[0] = 0;
 	subdivision[1] = 1; 
 	MPI_Cart_sub (*comm_grid,subdivision,comm_row); // Communicator between row
}

