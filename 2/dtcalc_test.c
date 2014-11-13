#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "particule.h"
#include "dtcalc.h"

#define NBITER 100
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

int main()
{
	double defdt = 10000000.0;
	double dt;
	double x1,y1,x2,y2, x1new, x2new, y1new, y2new, gap, move1, move2;

	FILE * fichier =fopen("datafile", "w+");
	fprintf(fichier, "#Iteration X1 Y1 X2 Y2\n");
	pset *s = pset_alloc(1);
	pset *s2= pset_alloc(1);
	double *dtmin = (double*)malloc(1*sizeof(double));
	pset_init_rand(s);
	pset_init_rand(s2);

	pset_print(s);
	pset_print(s2);	
	printf("default dt = %g\n", defdt);

	int size1 = s->nb;
	int size2 = s2->nb;
	x1 = s->pos[0];
	y1 = s->pos[0+size1];
	x2 = s2->pos[0];
	y2 = s2->pos[0+size2];
	gap = distance(x1,y1,x2,y2);
	dtmin[0] = gap;

	f_grav(s, s2); //a(t=0)
	f_grav(s2, s);
	fprintf(fichier, 
		"%d %g %g %g %g",
		0, x1, y1, x2, y2);

 	for (int i = 0; i < NBITER ; ++i)
 	{
		dt = MIN(dt_local_update(defdt, s, dtmin), 
				dt_local_update(defdt, s2, dtmin)); //t+dt

		printf("dt = %g\n", dt);
		update_pos(s, dt); //m(t+dt)
		update_pos(s2, dt);
		x1new = s->pos[0];
		y1new = s->pos[0+size1];
		x2new = s2->pos[0];
		y2new = s2->pos[0+size2];
		
		fprintf(fichier, 
			"%d %g %g %g %g",
			i+1, x1new, y1new, x2new, y2new);
		if(i!= NBITER -1)
			fprintf(fichier, "\n\n\n");

		move1 = distance(x1, y1, x1new, y1new); 
		move2 = distance(x2, y2, x2new, y2new);
		printf("gap: %g\n", gap);
		printf("s1 move %g (%g%% of gap)\n", move1, 100.0*fabs(move1/gap));
		printf("s2 move %g (%g%% of gap)\n", move2, 100.0*fabs(move2/gap));
		x1 = x1new;
		x2 = x2new;
		y1 = y1new;
		y2 = y2new;
		gap = distance(x1new,y1new,x2new,y2new);
		dtmin[0] = gap;

		update_spd(s, dt); //v(t+dt) 
		update_spd(s2, dt);
		f_grav(s, s2); //a(t+dt)
		f_grav(s2, s);
 		
 		pset_print(s);
		pset_print(s2);	
 	}
	fclose(fichier);
	pset_free(s);
	pset_free(s2);
	return 0;
}