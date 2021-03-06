#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <mpi.h>
#include <math.h>

// #define PRINT_ALIVE
// #define OUTPUT_BOARD
#define BENCH

#define cell( _i_, _j_ ) board[ ldboard * (_j_) + (_i_) ]
#define ngb( _i_, _j_ )  nbngb[ ldnbngb * ((_j_) - 1) + ((_i_) - 1 ) ]

//Neighbors table indexes...
#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3
#define UPPERLEFT 4
#define UPPERRIGHT 5
#define LOWERRIGHT 6
#define LOWERLEFT 7

//Edge type
#define LINE_EDGE 100
#define COLUMN_EDGE 200

int BS = 1200;

inline double mytimer(void)
{
    struct timeval tp;
    gettimeofday( &tp, NULL );
    return tp.tv_sec + 1e-6 * tp.tv_usec;
}

void output_board(int N, int *board, int ldboard, int loop)
{
    int i,j;
    fprintf(stderr, "loop %d\n", loop);
    for (i=0; i<N; i++) {
		for (j=0; j<N; j++) {
		    if ( cell( i, j ) == 1)
			fprintf(stderr, "X");
		    else
			fprintf(stderr, " ");
		}
	fprintf(stderr,"\n");
    }
}

/**
 * This function generates the iniatl board with one row and one
 * column of living cells in the middle of the board
 */
int generate_initial_board(int N, int *board, int ldboard)
{
    int i, j, num_alive = 0;

    for (i = 1; i <= N; i++) {
		for (j = 1; j <= N; j++) {
		    if (i == N/2 || j == N/2) {
				cell(i, j) = 1;
				num_alive ++;
		    }
		    else {
				cell(i, j) = 0;
		    }
		}
    }

    return num_alive;
}

/**
 * This function generates the list of the neighbors' ranks 
 */
void generate_neighbors_table(int* neighs, MPI_Comm grid, int myrank)
{
 	int displ;
 	int index;
 	int coords[2];

	index = 1;
	displ = 1;
	MPI_Cart_shift(grid, index, displ, &neighs[LEFT], &neighs[RIGHT]);
	index = 0;
	MPI_Cart_shift(grid, index, displ, &neighs[UP], &neighs[DOWN]);
	MPI_Cart_coords(grid, myrank, 2, coords);
	coords[0]--;
	coords[1]--;
	MPI_Cart_rank(grid, coords, &neighs[UPPERLEFT]);
	coords[1]+=2;
	MPI_Cart_rank(grid, coords, &neighs[UPPERRIGHT]);
	coords[0]+=2;
	MPI_Cart_rank(grid, coords, &neighs[LOWERRIGHT]);
	coords[1]-=2;
	MPI_Cart_rank(grid, coords, &neighs[LOWERLEFT]);
}

/**
 * This function launches the asynchronous communications 
 */
void do_comms(MPI_Request* rq, int* neighs, MPI_Comm grid, int block_size, 
				int* board, int ldboard, MPI_Datatype block_line)
{
	MPI_Isend(&cell(1, 1), block_size, MPI_INT, neighs[LEFT], 0, grid, &rq[0]); //To the left
	MPI_Irecv(&cell(1, block_size+1), block_size, MPI_INT, neighs[RIGHT], 0, grid, &rq[0]); 

	MPI_Isend(&cell(1, block_size), block_size, MPI_INT, neighs[RIGHT], 0, grid, &rq[1]);
	MPI_Irecv(&cell(1, 0), block_size, MPI_INT, neighs[LEFT], 0, grid, &rq[1]);	//To the right

	MPI_Isend(&cell(1, block_size), 1, MPI_INT, neighs[UPPERRIGHT], 0, grid, &rq[2]);
	MPI_Irecv(&cell(block_size+1, 0), 1, MPI_INT, neighs[LOWERLEFT], 0, grid, &rq[2]); //To the upperright
		
	MPI_Isend(&cell(block_size, block_size), 1, MPI_INT, neighs[LOWERRIGHT], 0, grid, &rq[3]);
	MPI_Irecv(&cell(0, 0), 1, MPI_INT, neighs[UPPERLEFT], 0, grid, &rq[3]); //To the lowerright
		
	MPI_Isend(&cell(1, 1), 1, MPI_INT, neighs[UPPERLEFT], 0, grid, &rq[4]);
	MPI_Irecv(&cell(block_size+1, block_size+1), 1, MPI_INT, neighs[LOWERRIGHT], 0, grid, &rq[4]);	//To the upperleft
		
	MPI_Isend(&cell(block_size, 1), 1, MPI_INT,neighs[LOWERLEFT], 0, grid, &rq[5]);
	MPI_Irecv(&cell(0, block_size+1), 1, MPI_INT, neighs[UPPERRIGHT], 0, grid, &rq[5]);//To the lowerleft.
		
	MPI_Isend(&cell(block_size, 1), 1, block_line,neighs[DOWN], 0, grid, &rq[6]);
	MPI_Irecv(&cell(0, 1), 1, block_line, neighs[UP], 0, grid, &rq[6]); //To lower
		
	MPI_Isend(&cell(1, 1), 1, block_line,neighs[UP], 0, grid, &rq[7]);
	MPI_Irecv(&cell(block_size+1, 1), 1, block_line, neighs[DOWN], 0, grid, &rq[7]);	//To upper
}

/**
 * This function computes an edge of ngb block
 */
void calc_ngb_edges(int k, int edgetype, int block_size, int* board, int ldboard, int* nbngb, int ldnbngb)
{
	if (edgetype == LINE_EDGE)
	{
		int i = k;
		for (int j = 1; j <= block_size; j++) {
			ngb( i, j ) =
			    cell( i-1, j-1 ) + cell( i, j-1 ) + cell( i+1, j-1 ) +
			    cell( i-1, j   ) +                  cell( i+1, j   ) +
			    cell( i-1, j+1 ) + cell( i, j+1 ) + cell( i+1, j+1 );
		}
	}
	if (edgetype == COLUMN_EDGE)
	{
		int j = k;
		for (int i = 1; i <= block_size; i++) {
			ngb( i, j ) =
			    cell( i-1, j-1 ) + cell( i, j-1 ) + cell( i+1, j-1 ) +
			    cell( i-1, j   ) +                  cell( i+1, j   ) +
			    cell( i-1, j+1 ) + cell( i, j+1 ) + cell( i+1, j+1 );
		}
	}
}


int main(int argc, char* argv[])
{	
	MPI_Init(NULL, NULL);
	int myrank, nb_processes;
  	MPI_Comm_rank( MPI_COMM_WORLD, &myrank ); 
	MPI_Comm_size( MPI_COMM_WORLD, &nb_processes);
    int PROCSPERLINE=sqrt(nb_processes);
    int PROCSPERCOL =sqrt(nb_processes);
	if(nb_processes != PROCSPERCOL*PROCSPERLINE){
		fprintf(stderr, "Erreur, mettez un nombre carré de procs.\n");
		MPI_Finalize();
		exit(EXIT_FAILURE);
	}
    int dims[2] = {PROCSPERLINE, PROCSPERCOL};
    int periods[2] = {1, 1};
    MPI_Comm grid;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods,0, &grid);
  	MPI_Comm_rank( grid, &myrank ); 
    int i, j, loop, num_alive, maxloop;
    int ldboard, ldnbngb, ldglobboard;
    double t1, t2;
    double temps, real_time;
	int *globboard= NULL; 
	int *globboard2= NULL; 

    int *board;
    int *nbngb;


    if (argc < 2) {
	maxloop = 10;
    } else if (argc >= 2){
	maxloop = atoi(argv[1]);
	if(argc > 2)
	BS = atoi(argv[2]);
    }
    num_alive = 0;

    /*Leading dimension of the global board array*/
	ldglobboard = BS+ 2;
    /* Leading dimension of the board array */
    ldboard = BS/PROCSPERLINE + 2;
    /* Leading dimension of the neigbour counters array */
    ldnbngb = BS/sqrt(nb_processes);

    board = malloc( ldboard * ldboard * sizeof(int) );
    nbngb = malloc( ldnbngb * ldnbngb * sizeof(int) );

    if(myrank == 0){
	    globboard = malloc(ldglobboard*ldglobboard * sizeof(int));
	    globboard2 = malloc(ldglobboard*ldglobboard * sizeof(int)); 
    	num_alive = generate_initial_board( BS, &globboard[1+ldglobboard] , ldglobboard );
	    #ifdef OUTPUT_BOARD
	    	output_board( BS, &globboard[1+ldglobboard], ldglobboard, 0 );
    	#endif
    	fprintf(stderr, "Starting number of living cells = %d\n", num_alive);
	}
	MPI_Datatype block2, block;
	MPI_Type_vector(ldboard-2, ldboard-2, ldglobboard, MPI_INT, &block2);
	MPI_Type_create_resized(block2, 0, sizeof(int), &block);
	MPI_Type_commit(&block);

	MPI_Datatype sub_block2, sub_block;
	MPI_Type_vector(ldboard-2, ldboard-2, ldboard, MPI_INT, &sub_block2);
	MPI_Type_create_resized(sub_block2, 0, sizeof(int), &sub_block);
	MPI_Type_commit(&sub_block);

	int * counts = (int*) malloc(nb_processes*sizeof(int));
	int * displs = (int*) malloc(nb_processes*sizeof(int));
	// Définition des déplacements pour chaque proc
	for (int i = 0; i < PROCSPERLINE; ++i)
	{
		for (int j = 0; j < PROCSPERCOL; ++j)
		{
			counts[i+j*PROCSPERCOL]= 1;
			displs[i+j*PROCSPERCOL]= i*ldglobboard*(ldboard-2)+j*(ldboard-2);
		}
	}

	MPI_Scatterv(&globboard[1+ldglobboard], counts, displs, block, &board[ldboard+1], 1,
				sub_block,0, grid);

	int neighs[8];
	generate_neighbors_table(neighs, grid, myrank);

    t1 = mytimer();
    int block_size = ldboard-2;
	MPI_Datatype block_line;
	MPI_Type_vector(block_size, 1, ldboard, MPI_INT, &block_line);
	MPI_Type_commit(&block_line);
	
	MPI_Status st;
	MPI_Request rq[8];
    for (loop = 1; loop <= maxloop; loop++) {
		//// Start async. comms ////
		do_comms(rq, neighs, grid, block_size, board, ldboard, block_line);

		//// Compute inner cells ////
		for (j = 2; j <= block_size-1; j++) {
			for (i = 2; i <= block_size-1; i++) {
			ngb( i, j ) =
			    cell( i-1, j-1 ) + cell( i, j-1 ) + cell( i+1, j-1 ) +
			    cell( i-1, j   ) +                  cell( i+1, j   ) +
			    cell( i-1, j+1 ) + cell( i, j+1 ) + cell( i+1, j+1 );
		    }
		}

		//// Computes cells on the edges ////

		//First column needs data from the left
		MPI_Wait(&rq[1], &st);
		MPI_Wait(&rq[2], &st);
		MPI_Wait(&rq[3], &st);
		calc_ngb_edges(1, COLUMN_EDGE, block_size, board, ldboard, nbngb, ldnbngb);
		
		//First line needs data from upper
		MPI_Wait(&rq[5], &st);
		MPI_Wait(&rq[6], &st);
		calc_ngb_edges(1, LINE_EDGE, block_size, board, ldboard, nbngb, ldnbngb);

		//Last column needs data from the right
		MPI_Wait(&rq[0], &st);
		MPI_Wait(&rq[4], &st);
		calc_ngb_edges(block_size, COLUMN_EDGE, block_size, board, ldboard, nbngb, ldnbngb);

		//Last line needs data from lower
		MPI_Wait(&rq[7], &st);
		calc_ngb_edges(block_size, LINE_EDGE, block_size, board, ldboard, nbngb, ldnbngb);


		//// Refresh cells ////
		num_alive = 0;
		for (j = 1; j <= block_size; j++) {
			for (i = 1; i <= block_size; i++) {
				if ( (ngb( i, j ) < 2) || 
				     (ngb( i, j ) > 3) ) {
				    cell(i, j) = 0;
				}
				else {
				    if ((ngb( i, j )) == 3)
					cell(i, j) = 1;
				}
				if (cell(i, j) == 1) {
				    num_alive ++;
				}
		    }
		}

	#ifdef PRINT_ALIVE
		printf("%d \n", num_alive);
	#endif
    }
	MPI_Gatherv(&board[ldboard+1], 1, sub_block,
                &globboard2[1+ldglobboard], counts, displs,
                block, 0, grid);

    t2 = mytimer();

    temps = t2 - t1;
    MPI_Allreduce(&temps, &real_time, 1,MPI_DOUBLE, MPI_MAX, grid);
    MPI_Allreduce(MPI_IN_PLACE, &num_alive, 1, MPI_INT, MPI_SUM, grid);
    if(myrank == 0)
    {
    	printf("Final number of living cells = %d\n", num_alive);
    	printf("time=%.2lf ms\n",(double)temps * 1.e3);
	    #ifdef BENCH
			char fname [40];
			sprintf(fname, "time_mpi_async_%d.dat", nb_processes);
    		FILE* f=fopen(fname, "w");
    		if (f != NULL)
    			fprintf(f,"%.2lf", temps*1.e3);
    		fclose(f);
    	#endif	
		#ifdef OUTPUT_BOARD
			output_board( BS, &globboard2[1+ldglobboard], ldglobboard, maxloop);
		#endif
	}

    free(counts);
    free(displs);
    free(board);
    free(nbngb);
	MPI_Finalize();
    return EXIT_SUCCESS;
}

