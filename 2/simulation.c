#include <stdio.h>
#include "particule.h"
#include "mpi.h"

#define NB_PARTICLES 1
#define NB_ITER 1

/* Fonction auxilliaire servant à initialiser le type Particule MPI*/
void init_mpi_pset_type(MPI_Datatype * MPI_PSET)
{
	MPI_Datatype type[5] = { MPI_INT , MPI_DOUBLE, MPI_DOUBLE,
							 MPI_DOUBLE, MPI_DOUBLE }; 
  	int blocklen[5] = {1, NB_PARTICLES, 2*NB_PARTICLES, 
  					   2*NB_PARTICLES, 2*NB_PARTICLES}; 
	MPI_Aint i1,i2 ; 
	MPI_Aint disp[5];
	pset * p = pset_alloc(NB_PARTICLES);
	MPI_Get_address(p, &i1); 
	MPI_Get_address(&p->nb, &i2); disp[0] = i2-i1;
	MPI_Get_address(&p->m[0], &i2); disp[1] = i2-i1 ; 
	MPI_Get_address(&p->acc[0], &i2); disp[2] = i2-i1 ; 
	MPI_Get_address(&p->spd[0], &i2); disp[3] = i2-i1 ; 
	MPI_Get_address(&p->pos[0], &i2); disp[4] = i2-i1;

	MPI_Type_struct(5, blocklen, disp, type, MPI_PSET); 
	MPI_Type_commit(MPI_PSET);
}
void swap(pset* a, pset * b) {
    pset temp = *a;
    *a = *b;
    *b = temp;
}


int main(void)
{
	double dt = 1000.0;

	/* Initialisation des constantes MPI */
	int myrank, nb_processes;
  	MPI_Init( NULL, NULL ); 
  	MPI_Request send_req, recv_req;
  	MPI_Status send_stat, recv_stat;
  	MPI_Comm_rank( MPI_COMM_WORLD, &myrank ); 
	MPI_Comm_size( MPI_COMM_WORLD, &nb_processes);

	/* Initialisation du type pset MPI avec une fonction auxilliaire*/
  	MPI_Datatype MPI_PSET = NULL;
  	init_mpi_pset_type(&MPI_PSET);


	/* Initialisation du set de particules propre au processus. */
	pset * s = pset_alloc(NB_PARTICLES);
	pset_init_rand(s);

	/* Initialisation des buffers.*/
	pset * calc_buf = pset_alloc(NB_PARTICLES);
	pset * comm_buf = pset_alloc(NB_PARTICLES);
	*calc_buf = *s;

	/* Initialisation du fichier dans lequel écrira ce processus. */
	pset_print(s);
	char file_name[100];
	sprintf(file_name, "datafile%d", myrank);
	FILE * fichier =fopen(file_name, "w+");


	/* définition des processus voisin suivant et précédent*/
	int next_proc =  (myrank+1) % nb_processes ;
	int prev_proc = (((myrank-1 ) % nb_processes) +nb_processes) %nb_processes;
	
	for (int i = 0; i < NB_ITER; ++i)
	{
		for (int j = 0; j < nb_processes; ++j)
		{
			/* Envoi du buffer de calcul actuel. */
			MPI_Send_init(calc_buf, 1, MPI_PSET,
						 next_proc, 3, MPI_COMM_WORLD, &send_req);

			/* Récéption du buffer de calcul suivant*/
			MPI_Recv_init(comm_buf, 1, MPI_PSET,
						  prev_proc, 3, MPI_COMM_WORLD, &recv_req);

			/* Début des communications */
			MPI_Start(&send_req);
			MPI_Start(&recv_req);

			/* Calcul de la force */
			f_grav(s, calc_buf);

			/* On attend la fin des communications */
			MPI_Wait(&send_req, &send_stat);
			MPI_Wait(&recv_req, &recv_stat);

			/* Debug stuff */
			if(myrank ==0){
				printf("--------------------\nReceived buffer:\n");
				pset_print(comm_buf);
				printf("End received buffer:\n--------------------\n");
			}
			swap(calc_buf, comm_buf);
		}
		/* Mise à jour des positions des particules. */
		pset_step(s, dt);

		/* Enregistrement de la position actuelle de la particule dans un fichier */
		fprintf(fichier, 
			"%d %g %g\n\n",
			i, s->pos[0], s->pos[0+NB_PARTICLES]);
	}
	
	/* Libération des ressources propres à ce processus.*/
	pset_free(s);
	fclose(fichier);

  	MPI_Finalize();
  	return 0;
}