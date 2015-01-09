#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <pthread.h>

//#define PRINT_ALIVE
#define BS 1000
#define THREADNUM 4

pthread_mutex_t locks [THREADNUM];
int counter[THREADNUM];

typedef struct params {
	int maxloop;
	int ldboard;
	int *board;
	int ldnbngb;
	int num_alive;
	int * nbngb;
	int me;
} Params;


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
    printf("loop %d\n", loop);
    for (i=0; i<N; i++) {
		for (j=0; j<N; j++) {
		    if ( cell( i, j ) == 1)
			printf("X");
		    else
			printf(" ");
		}
	printf("\n");
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

void * thread_f(void * p)
{
	Params * params = (Params *) p;
	int maxloop= params->maxloop;
	int ldboard= params->ldboard;
	int * board  = params->board ;
	int * nbngb = params ->nbngb;
	int ldnbngb= params->ldnbngb;
	int num_alive= params->num_alive;
	int me = params->me;

	int start = me*BS/THREADNUM +1;
	int end;
	if (me == THREADNUM)
		end = BS;
	else
		end = (me+1)*BS/THREADNUM;

	int neighbour[2] = {(me-1+THREADNUM)%THREADNUM, (me+1)%THREADNUM};

	int loop, i, j;
	for (loop = 1; loop <= maxloop; loop++) {
		cell(   0, 0   ) = cell(BS, BS);
		cell(   0, BS+1) = cell(BS,  1);
		cell(BS+1, 0   ) = cell( 1, BS);
		cell(BS+1, BS+1) = cell( 1,  1);

		for (i = start; i <= end; i++) {
		    cell(   i,    0) = cell( i, BS);
		    cell(   i, BS+1) = cell( i,  1);
		    cell(   0,    i) = cell(BS,  i);
		    cell(BS+1,    i) = cell( 1,  i);
		}

		for (j = start; j <= end; j++) {
			for (i = 1; i <= BS; i++) {
			ngb( i, j ) =
			    cell( i-1, j-1 ) + cell( i, j-1 ) + cell( i+1, j-1 ) +
			    cell( i-1, j   ) +                  cell( i+1, j   ) +
			    cell( i-1, j+1 ) + cell( i, j+1 ) + cell( i+1, j+1 );
		   }
		}

		for (int i = 0; i < 2; ++i)
		{
			printf("thread %d inc %d\n", me, neighbour[i]);
			pthread_mutex_lock(&locks[neighbour[i]]);
			counter[neighbour[i]]++;
			pthread_mutex_unlock(&locks[neighbour[i]]);
		}

		while(counter[me]>= 2)
		{
			// printf("thread %d counter %d\n", me, counter[me]);
		}
		counter[me]-= 2;
		printf("Je suis Charlie %d\n", me);
		num_alive = 0;
		for (j = start; j <= end; j++) {
			for (i = 1; i <= BS; i++) {
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

		#ifdef OUTPUT_BOARD
	    output_board( BS, &(cell(1, 1)), ldboard, 0 );
		#endif
		#ifdef PRINT_ALIVE
		printf("%d \n", num_alive);
	    #endif

		for (int i = 0; i < 2; ++i)
		{
			pthread_mutex_lock(&locks[neighbour[i]]);
			printf("thread %d inc %d\n", me, neighbour[i]);
			counter[neighbour[i]]++;
			pthread_mutex_unlock(&locks[neighbour[i]]);
		}

		while(counter[me]>= 2)
		{
			//printf("thread %d counter %d \n", me, counter[me]);
		}
		counter[me]-= 2;

    }
    return NULL;
}

int main(int argc, char* argv[])
{
    int num_alive, maxloop;
    int ldboard, ldnbngb;
    double t1, t2;
    double temps;
 
    int *board;
    int *nbngb;

    if (argc < 2) {
	maxloop = 10;
    } else {
	maxloop = atoi(argv[1]);
    }
    num_alive = 0;

    for (int i = 0; i < THREADNUM; ++i)
    {
    	pthread_mutex_init ( &locks[i], NULL);
    }


    /* Leading dimension of the board array */
    ldboard = BS + 2;
    /* Leading dimension of the neigbour counters array */
    ldnbngb = BS;

    board = malloc( ldboard * ldboard * sizeof(int) );
    nbngb = malloc( ldnbngb * ldnbngb * sizeof(int) );

    num_alive = generate_initial_board( BS, &(cell(1, 1)), ldboard );

	#ifdef OUTPUT_BOARD
    output_board( BS, &(cell(1, 1)), ldboard, 0 );
	#endif
    printf("Starting number of living cells = %d\n", num_alive);

	pthread_t workers[THREADNUM];
	Params p[THREADNUM];
    
    t1 = mytimer();
    for (int i = 0; i < THREADNUM; ++i)
    {	
    	p[i].maxloop= maxloop;
		p[i].ldboard= ldboard;
		p[i].board  = board ;
		p[i].ldnbngb= ldnbngb;
		p[i].nbngb = nbngb;
		p[i].num_alive= num_alive;
    	p[i].me = i;
    	counter[i]= 0;
    	pthread_create(&workers[i], NULL, thread_f, (void *) &p[i]);
    }
    for (int i = 0; i < THREADNUM; ++i)
    {
    	pthread_join(workers[i], NULL);

    }
    t2 = mytimer();

    temps = t2 - t1;
    printf("Final number of living cells = %d\n", num_alive);
    printf("time=%.2lf ms\n",(double)temps * 1.e3);
    #ifdef OUTPUT_BOARD
    output_board( BS, &(cell(1, 1)), ldboard, maxloop);
    #endif
    free(board);
    free(nbngb);
    return EXIT_SUCCESS;
}

