#include "particule.h"
#include <stdio.h>

#define NBITER 20

int main()
{
	double dt = 1000.0;
	pset *s = pset_alloc(1);
	pset *s2= pset_alloc(1);
	pset_init_rand(s);
	pset_init_rand(s2);

	pset_print(s);
	pset_print(s2);	

	FILE * fichier =fopen("datafile", "w+");
	fprintf(fichier, "#Iteration X1 Y1 X2 Y2\n");
	for (int i = 0; i < NBITER ; ++i)
	{
		f_grav(s, s2);
		f_grav(s2, s);
		pset_step(s, dt);
		pset_step(s2, dt);
		fprintf(fichier, 
			"%d %g %g %g %g",
			i, s->pos[0], s->pos[1], s2->pos[0], s2->pos[1]);

		if(i!= NBITER -1)
			fprintf(fichier, "\n\n\n");
	}

	pset_print(s);
	pset_print(s2);	
	

	fclose(fichier);
	pset_free(s);
	pset_free(s2);
	return 0;
}