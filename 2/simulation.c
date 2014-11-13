#include <stdio.h>
#include "particule.h"
#include "dtcalc.h"
#include "mpi.h"

#define NB_PARTICLES 2
#define NB_ITER 400

/* Fonction auxilliaire servant à initialiser le type Particule MPI*/
void init_mpi_pset_type(MPI_Datatype * MPI_PSET, pset * p)
{
	MPI_Datatype type[3] = { MPI_INT , MPI_DOUBLE, MPI_DOUBLE}; 
  	int blocklen[3] = {1, NB_PARTICLES, 2*NB_PARTICLES}; 
	MPI_Aint i1,i2 ; 
	MPI_Aint disp[6];

	MPI_Get_address(p, &i1); 
	MPI_Get_address(&p->nb, &i2); disp[0] = i2-i1;
	MPI_Get_address(&p->m[0], &i2); disp[1] = i2-i1 ; 
	MPI_Get_address(&p->pos[0], &i2); disp[2] = i2-i1;

	MPI_Type_struct(3, blocklen, disp, type, MPI_PSET); 
	MPI_Type_commit(MPI_PSET);
}

void swap(pset* a, pset * b) {
    pset temp = *a;
    *a = *b;
    *b = temp;
}

int main(void)
{
	double defdt = 500;
	double dt = defdt;
	/* Initialisation des constantes MPI */
	int myrank, nb_processes;
  	MPI_Init( NULL, NULL ); 
  	MPI_Request send_req, recv_req;
  	MPI_Status send_stat, recv_stat;
  	MPI_Comm_rank( MPI_COMM_WORLD, &myrank ); 
	MPI_Comm_size( MPI_COMM_WORLD, &nb_processes);

	/* Génération d'un soleil */
	pset * sun = pset_alloc(NB_PARTICLES);
	pset_init_sun(sun);

	/* Initialisation du set de particules propre au processus. */
	pset * s;
	if(myrank == 0)
	{
		s =sun;
	}
	else
	{
		s = pset_alloc(NB_PARTICLES);
		pset_init_orbit(sun, s);
	}
	/* Initialisation des buffers.*/
	pset * calc_buf = pset_alloc(NB_PARTICLES);
	pset * comm_buf = pset_alloc(NB_PARTICLES);

	/* Initialisation des types pset MPI pour le set local et les buffers.*/
  	MPI_Datatype MPI_PSET;
	init_mpi_pset_type(&MPI_PSET, s);

  	MPI_Datatype MPI_CALC_BUF;
	init_mpi_pset_type(&MPI_CALC_BUF, calc_buf);

	MPI_Datatype MPI_COMM_BUF;
	init_mpi_pset_type(&MPI_COMM_BUF, comm_buf);	


	/* Initialisation du fichier dans lequel écrira ce processus. */
	pset_print(s);
	char file_name[100];
	sprintf(file_name, "datafile%d", myrank);
	FILE * fichier =fopen(file_name, "w+");

	/* Définition des processus voisin suivant et précédent*/
	int next_proc=  (myrank+1) % nb_processes ;
	int prev_proc= (((myrank-1 ) % nb_processes) +nb_processes) %nb_processes;

	for (int i = 0; i < NB_ITER; ++i)
	{
		/* On copie le contenu de l'ensemble local dans le buffer d'envoi */
		pset_copy(s, calc_buf);

		for (int j = 0; j < nb_processes; ++j)
		{
			/* Envoi du buffer de calcul actuel. */
			MPI_Send_init(calc_buf, 1, MPI_CALC_BUF,
						 next_proc, 3, MPI_COMM_WORLD, &send_req);

			/* Récéption du buffer de calcul suivant*/
			MPI_Recv_init(comm_buf, 1, MPI_COMM_BUF,
						  prev_proc, 3, MPI_COMM_WORLD, &recv_req);

			/* Début des communications */
			MPI_Start(&send_req);
			MPI_Start(&recv_req);

			/* Calcul de la force */
			f_grav(s, calc_buf);
			dt = dt_local_update(defdt, s);
			
			/* On attend la fin des communications */
			MPI_Wait(&send_req, &send_stat);
			MPI_Wait(&recv_req, &recv_stat);

			swap(calc_buf, comm_buf);
		}
		/* Mise à jour des positions des particules. */
		dt_global_update(&dt);

		pset_step(s, dt);

		/* Enregistrement de la position actuelle des particules
		 * locales dans un fichier */
		fprintf(fichier, "%d ", i);
		for (int i = 0; i < NB_PARTICLES; ++i)
		{
			fprintf(fichier, 
					"%g %g ",
					s->pos[i], s->pos[i+NB_PARTICLES]);
		}
		if(i!= NB_ITER -1)
			fprintf(fichier, "\n\n\n");
	}
	
	/* Libération des ressources propres à ce processus.*/
	pset_free(s);
	fclose(fichier);

  	MPI_Finalize();
  	return 0;
}