#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <pthread.h>

// #define PRINT_ALIVE
// #define OUTPUT_BOARD
#define BENCH

pthread_cond_t* cond;
pthread_mutex_t* locks;
int* counter;
int* counter2;
int* counter3;
int* num_alive_local;

int BS = 1200;
int num_threads = 2;

typedef struct params {
	int maxloop;
	int ldboard;
	int *board;
	int ldnbngb;
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

/*
*	This function increments the neighbours processes' counter by one
*	If their counter reaches 2, then send a signal to awake them
*/
void unlock_neighbours(int* neighbour, 
						int* counter, pthread_cond_t* cond, pthread_mutex_t* locks)
{
	for (int i = 0; i < 2; ++i)
	{
		pthread_mutex_lock(&locks[neighbour[i]]);
		counter[neighbour[i]]++;
		if(counter[neighbour[i]] == 2)
		{
			pthread_cond_signal(&cond[neighbour[i]]);
		}
		pthread_mutex_unlock(&locks[neighbour[i]]);
	}
}

/*
*	This function locks the process while his 2 neighbours finishes their job. 
*/
void lock_self(int me, int* counter, pthread_cond_t* cond, pthread_mutex_t* locks)
{
	pthread_mutex_lock(&locks[me]);
	while(counter[me] != 2)
	{
		pthread_cond_wait(&cond[me], &locks[me]);	
	}
	counter[me] = 0;
	pthread_mutex_unlock(&locks[me]);	
}

void * thread_f(void * p)
{
	Params * params = (Params *) p;
	int maxloop= params->maxloop;
	int ldboard= params->ldboard;
	int * board  = params->board ;
	int * nbngb = params ->nbngb;
	int ldnbngb= params->ldnbngb;
	int me = params->me;

	int start = me*BS/num_threads +1;
	int end;
	if (me == num_threads-1)
		end = BS;
	else
		end = (me+1)*BS/num_threads;
	int neighbour[2] = {(me-1+num_threads)%num_threads, (me+1)%num_threads};

	int loop, i, j;
	for (loop = 1; loop <= maxloop; loop++) 
	{
		if(me == num_threads-1)
		{
			cell(BS+1, 0   ) = cell( 1, BS);
			cell(BS+1, BS+1) = cell( 1,  1);
			for (i = 1; i <= BS; i++)
		    	cell(   i, BS+1) = cell( i,  1);
		}
		if(me == 0)
		{
			cell(   0, 0   ) = cell(BS, BS);
			cell(   0, BS+1) = cell(BS,  1);	
			for (i = 1; i <= BS; i++)
				cell(   i,    0) = cell( i, BS);
		}
		for (i = start; i <= end; i++) {
			cell(   0,    i) = cell(BS,  i);
			cell(BS+1,    i) = cell( 1,  i);
		}

		unlock_neighbours(neighbour, counter, cond, locks);
		lock_self(me, counter, cond, locks);
		int k = 0;
		j = start;
		do{
			for (i = 1; i <= BS; i++) {
				ngb( i, j ) =
			    	cell( i-1, j-1 ) + cell( i, j-1 ) + cell( i+1, j-1 ) +
			    	cell( i-1, j   ) +                  cell( i+1, j   ) +
			   		cell( i-1, j+1 ) + cell( i, j+1 ) + cell( i+1, j+1 );
			}
			j=end; k++;
		}while(k<=1);

		unlock_neighbours(neighbour, counter2, cond, locks);

		for (j = start+1; j <= end-1; j++) {
			for (i = 1; i <= BS; i++) {
			ngb( i, j ) =
			    cell( i-1, j-1 ) + cell( i, j-1 ) + cell( i+1, j-1 ) +
			    cell( i-1, j   ) +                  cell( i+1, j   ) +
			    cell( i-1, j+1 ) + cell( i, j+1 ) + cell( i+1, j+1 );
		   }
		}
		lock_self(me, counter2, cond, locks);
		
		num_alive_local[me] = 0;
		k = 0;
		j = start;
		do{
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
					num_alive_local[me] ++;
				}
			}
			j=end; k++;
		}while(k<=1);

		unlock_neighbours(neighbour, counter3, cond, locks);

		for (j = start+1; j <= end-1; j++) {
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
					num_alive_local[me] ++;
				}
			}
		}
		lock_self(me, counter3, cond, locks);

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
    } 
    else if (argc >= 2){
		maxloop = atoi(argv[1]);
		if(argc > 2)
			BS = atoi(argv[2]);
		if(argc > 3){
			num_threads = atoi(argv[3]); 
    	}
    }
    num_alive = 0;

	cond = malloc(num_threads*sizeof(pthread_cond_t));
	locks = malloc(num_threads*sizeof(pthread_mutex_t));
	counter = malloc(num_threads*sizeof(int));
	counter2 = malloc(num_threads*sizeof(int));
	counter3 = malloc(num_threads*sizeof(int));
	num_alive_local = malloc(num_threads*sizeof(int));

    for (int i = 0; i < num_threads; ++i)
    {
    	pthread_cond_init( &cond[i], NULL);
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

	pthread_t workers[num_threads];
	Params p[num_threads];
    
    t1 = mytimer();
    for (int i = 0; i < num_threads; ++i)
    {	
    	p[i].maxloop= maxloop;
		p[i].ldboard= ldboard;
		p[i].board  = board ;
		p[i].ldnbngb= ldnbngb;
		p[i].nbngb = nbngb;
    	p[i].me = i;
    	counter[i]= 0;
    	counter2[i]= 0;
    	counter3[i]= 0;
    	num_alive_local[i] = 0;
    }

    for (int i = 0; i < num_threads; ++i)
    {	
       	pthread_create(&workers[i], NULL, thread_f, (void *) &p[i]);
    }

    num_alive = 0;
    for (int i = 0; i < num_threads; ++i)
    {
    	pthread_join(workers[i], NULL);
    	num_alive += num_alive_local[i];
    }
    t2 = mytimer();

    temps = t2 - t1;
    printf("Final number of living cells = %d\n", num_alive);
    printf("time=%.2lf ms\n",(double)temps * 1.e3);
    #ifdef BENCH
		char fname [40];
		sprintf(fname, "time_pthread_%d.dat", num_threads);
    	FILE* f=fopen(fname, "w");
    	if (f != NULL)
    		fprintf(f,"%.2lf", temps*1.e3);
    	fclose(f);
    #endif
    #ifdef OUTPUT_BOARD
    output_board( BS, &(cell(1, 1)), ldboard, maxloop);
    #endif

	free(cond);
	free(locks);
	free(counter);
	free(counter2);
	free(counter3);
	free(num_alive_local);

    free(board);
    free(nbngb);
    return EXIT_SUCCESS;
}

