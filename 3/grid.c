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

void proc(int N, int Nb, 
			double* bl_a, double* bl_b, double* bl_c,
			int myrank, int np, int np_c,
			MPI_Comm comm_grid, MPI_Comm comm_col, MPI_Comm comm_row)
{
	int k;
	double my_a[Nb*Nb];
	int coords[2];
	MPI_Status st;

	for (int i = 0; i < Nb*Nb; ++i)
	{
		my_a[i] = bl_a[i];
	}

	MPI_Cart_coords(comm_grid, myrank, 2, coords);
	int sndto = (((coords[0]-1)%np_c) +np_c) %np_c;
	int recvfrom = (coords[0]+1)%np_c;
	int myrow = coords[0];
	int mycol = coords[1];

	for (k = 0; k < np_c; k++)
	{	
		/* If I am i+k%N proc of the line
		 * 	Bcast_line(A[i][i+k%N]) 
		 * Else
		 * 	recv(A) from the i+k%N proc of the line
		 */

		if(mycol == (myrow+k)%np_c)
		{
			for (int i = 0; i < Nb*Nb; ++i)
			{
				bl_a[i] = my_a[i];
			}
		}
		MPI_Bcast(bl_a, N*N, MPI_DOUBLE, (myrow+k)%np_c, comm_row);


	// for (int l = 0; l < np; ++l)
	// {
	// 	if(myrank == l)
	// 	{
	// 		printf("=== This is k=%d on proc %d line %d===\n", k, l, myrow);
	// 		for (int i=0; i<Nb; i++) {
	// 	        for (int j=0; j<Nb; j++) {
	// 	            printf("%g ", bl_a[i*Nb+j]);
	// 	        }
	// 	        printf("\n");
	// 	    }
	// 	}
	// 	MPI_Barrier(MPI_COMM_WORLD);
	// }
	
	cblas_dgemm_scalaire(Nb, bl_a, bl_b, bl_c);  //Cij = A[i][i+k%N]*B[i+k%N][j]
		/* send(B) to upper neighbour
		 */
	MPI_Sendrecv_replace(bl_b, Nb*Nb, MPI_DOUBLE, sndto, 0, 
						recvfrom, 0, comm_col, &st);
	}
}

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
					MPI_Comm* comm_grid, MPI_Comm* comm_row, MPI_Comm* comm_col)
{
	int dims[2] = {np_r, np_c};
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

