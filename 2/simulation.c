#include <stdio.h>
#include "particule.h"
#include "mpi.h"

#define NB_PARTICLES 1
#define NB_ITER 10

/* Fonction auxilliaire servant à initialiser le type Particule MPI*/
void init_mpi_pset_type(MPI_Datatype * MPI_PSET)
{
	MPI_Datatype type[5] = { MPI_INT , MPI_DOUBLE, MPI_DOUBLE,
							 MPI_DOUBLE, MPI_DOUBLE }; 
  	int blocklen[5] = {1, NB_PARTICLES, 2*NB_PARTICLES, 
  					   2*NB_PARTICLES, 2*NB_PARTICLES}; 
	MPI_Aint i1,i2 ; 
	MPI_Aint disp[5];
	pset p;
	MPI_Get_address(&p, &i1); 
	MPI_Get_address(&p.nb, &i2); disp[0] = i2-i1;
	MPI_Get_address(&p.m[0], &i2); disp[1] = i2-i1 ; 
	MPI_Get_address(&p.acc[0], &i2); disp[2] = i2-i1 ; 
	MPI_Get_address(&p.spd[0], &i2); disp[3] = i2-i1 ; 
	MPI_Get_address(&p.pos[0], &i2); disp[4] = i2-i1;

	MPI_Type_struct(1, blocklen, disp, type, MPI_PSET); 
	MPI_Type_commit(MPI_PSET);
}

int main(void)
{
	/* Initialisation des constantes MPI */
	int myrank, nb_processes;
  	MPI_Init( NULL, NULL ); 
  	MPI_Request send_req, recv_req;
  	MPI_Status recv_stat;
  	MPI_Comm_rank( MPI_COMM_WORLD, &myrank ); 
	MPI_Comm_size( MPI_COMM_WORLD, &nb_processes);

	/* Initialisation du type pset MPI avec une fonction auxilliaire*/
  	MPI_Datatype MPI_PSET = NULL;
  	init_mpi_pset_type(&MPI_PSET);


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
			/* Communication avec un autre processus et calcul
			 * de la force */
			MPI_Send_init(s /*à changer*/, 1, MPI_PSET, 0/*à changer*/, 99, MPI_COMM_WORLD, &send_req);
			MPI_Recv_init(s /*à changer*/, 1, MPI_PSET, 0/*à changer*/, 99, MPI_COMM_WORLD, &recv_req);
			MPI_Start(&send_req);
			f_grav(s, s /*à changer*/);
			MPI_Wait(&recv_req, &recv_stat);
		}
		/* Mise à jour des positions des particules. */
		pset_step(s);
	}
	
	/* Libération des ressources propres à ce processus.*/
	pset_free(s);
	fclose(fichier);

  	MPI_Finalize();
  	return 0;
}