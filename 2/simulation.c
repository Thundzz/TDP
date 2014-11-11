#include <stdio.h>
#include "particule.h"
#include "mpi.h"

#define NB_PARTICLES 1
#define NB_ITER 10


int main(void)
{
	/* Initialisation des constantes MPI */
	int myrank, nb_processes;
  	MPI_Init( NULL, NULL ); 
  	MPI_Comm_rank( MPI_COMM_WORLD, &myrank ); 
	MPI_Comm_size( MPI_COMM_WORLD, &nb_processes);
	
	/* Initialisation du set de particules propre au processus. */
	pset *s = pset_alloc(NB_PARTICLES);
	pset_init_rand(s);

	/* Initialisation du fichier dans lequel écrira ce processus. */
	pset_print(s);
	char file_name[100];
	sprintf(file_name, "datafile%d", myrank);
	FILE * fichier =fopen(file_name, "w+");


	for (int i = 0; i < NB_ITER; ++i)
	{
		for (int i = 0; i < nb_processes; ++i)
		{
			/* Communicate and 
			 * Compute the force with the current process process. */
			//MPI_send(s, someone);
			//MPI_rcv(other, someone_else);
			//f_grav(s, other);
		}
		/* Update the positions of the particles. */
		pset_step(s);
	}

	/* Libération des ressources propres à ce processus.*/
	pset_free(s);
	fclose(fichier);

  	MPI_Finalize();
  	return 0;
}