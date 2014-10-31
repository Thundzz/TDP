#include "particule.h"
#include <stdio.h>

#define NBITER 10

int main()
{
	pset *s = pset_alloc(1);
	pset *s2= pset_alloc(1);
	pset_init_rand(s);
	pset_init_rand(s2);

	pset_print(s);
	pset_print(s2);	

	FILE * fichier =fopen("deux.txt", "w+");

	for (int i = 0; i < NBITER ; ++i)
	{
		f_grav(s, s2);
		f_grav(s2, s);
		pset_step(s);
		pset_step(s2);
		fprintf(fichier, 
			"%d %g %g %g %g\n",
			i, s->pos[0], s->pos[1], s2->pos[0], s2->pos[1]);
	}

	pset_print(s);
	pset_print(s2);	
	

	fclose(fichier);
	pset_free(s);
	pset_free(s2);
	return 0;
}