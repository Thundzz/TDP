#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <mpi.h>
#include <math.h>
//#define PRINT_ALIVE
// #define OUTPUT_BOARD

#define BS 10

#define cell( _i_, _j_ ) board[ ldboard * (_j_) + (_i_) ]
#define ngb( _i_, _j_ )  nbngb[ ldnbngb * ((_j_) - 1) + ((_i_) - 1 ) ]

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

int main(int argc, char* argv[])
{	
	MPI_Init(NULL, NULL);
	int myrank, nb_processes;
  	MPI_Comm_rank( MPI_COMM_WORLD, &myrank ); 
	MPI_Comm_size( MPI_COMM_WORLD, &nb_processes);
    
    int i, j, loop, num_alive, maxloop;
    int ldboard, ldnbngb, ldglobboard;
    double t1, t2;
    double temps, real_time;
	int *globboard; 
    int *board;
    int *nbngb;



    if (argc < 2) {
	maxloop = 10;
    } else {
	maxloop = atoi(argv[1]);
    }
    num_alive = 0;

    /*Leading dimension of the global board array*/
	ldglobboard = BS+ 2;
    /* Leading dimension of the board array */
    int PROCSPERLINE=sqrt(nb_processes);
    int PROCSPERCOL =sqrt(nb_processes);
    ldboard = BS/PROCSPERLINE + 2;
    /* Leading dimension of the neigbour counters array */
    ldnbngb = BS/sqrt(nb_processes);

    board = malloc( ldboard * ldboard * sizeof(int) );
    nbngb = malloc( ldnbngb * ldnbngb * sizeof(int) );
    globboard = malloc(ldglobboard*ldglobboard * sizeof(int));

    if(myrank == 0){
    	num_alive = generate_initial_board( BS, globboard , ldglobboard );
	    output_board( BS, globboard+1 + ldglobboard, ldglobboard, 0 );
    	fprintf(stderr, "Starting number of living cells = %d\n", num_alive);
	}

	MPI_Datatype block2, block;
	MPI_Type_vector(ldboard, ldboard, ldglobboard, MPI_INT, &block2);
	MPI_Type_create_resized(block2, 0, sizeof(int), &block);
	MPI_Type_commit(&block);
	int * counts = (int*) malloc(nb_processes*sizeof(int));
	int * displs = (int*) malloc(nb_processes*sizeof(int));
	if(nb_processes != PROCSPERCOL*PROCSPERLINE){
		fprintf(stderr, "Erreur, mettez un nombre carré de procs.\n");
		MPI_Finalize();
		exit(EXIT_FAILURE);
	}
	// Définition des déplacements pour chaque proc
	for (int i = 0; i < PROCSPERLINE; ++i)
	{
		for (int j = 0; j < PROCSPERCOL; ++j)
		{
			counts[i+j*PROCSPERCOL]= 1;
			displs[i+j*PROCSPERCOL]= i*ldglobboard*(ldboard-2)+j*(ldboard-2);
		}
	}
	MPI_Scatterv(globboard, counts, displs, block, board, ldboard*ldboard,
				MPI_INT,0, MPI_COMM_WORLD);
	
	for (int i = 0; i < nb_processes; ++i)
	{
		if(myrank == i){
		   	output_board( ldboard-2, board+1 +ldboard, ldboard, 0 );
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}

    t1 = mytimer();
    int block_size = ldboard-2;
    for (loop = 1; loop <= maxloop; loop++) {
	cell(   0, 0   ) = cell(block_size, block_size);
	cell(   0, block_size+1) = cell(block_size,  1);
	cell(block_size+1, 0   ) = cell( 1, block_size);
	cell(block_size+1, block_size+1) = cell( 1,  1);

	for (i = 1; i <= block_size; i++) {
	    cell(   i,    0) = cell( i, block_size);
	    cell(   i, block_size+1) = cell( i,  1);
	    cell(   0,    i) = cell(block_size,  i);
	    cell(block_size+1,    i) = cell( 1,  i);
	}

	for (j = 1; j <= block_size; j++) {
		for (i = 1; i <= block_size; i++) {
		ngb( i, j ) =
		    cell( i-1, j-1 ) + cell( i, j-1 ) + cell( i+1, j-1 ) +
		    cell( i-1, j   ) +                  cell( i+1, j   ) +
		    cell( i-1, j+1 ) + cell( i, j+1 ) + cell( i+1, j+1 );
	    }
	}

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

    t2 = mytimer();

    temps = t2 - t1;
    MPI_Allreduce(&temps, &real_time, 1,MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    MPI_Allreduce(&num_alive, &num_alive, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    if(myrank == 0)
    {
    	printf("Final number of living cells = %d\n", num_alive);
    	printf("time=%.2lf ms\n",(double)temps * 1.e3);
	}
    free(counts);
    free(displs);
    free(board);
    free(nbngb);
	MPI_Finalize();
    return EXIT_SUCCESS;
}

