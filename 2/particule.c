#include "particule.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

double dt = 1000.0;
static int SEEDED =0;

unsigned long mix(unsigned long a, unsigned long b, unsigned long c)
{
    a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);
    return c;
}

pset * pset_alloc(int nb_par){
	pset * set = (pset *)malloc(sizeof(pset));
	if(set == NULL)
	{
		fprintf(stderr, "Can't allocate memory for the set creation.\n");
		exit(EXIT_FAILURE);
	}
	set->m = malloc(nb_par * sizeof(double));
	set->pos = malloc(2* nb_par * sizeof(double));
	set->spd = malloc(2* nb_par * sizeof(double));
	set->acc = malloc(2* nb_par * sizeof(double));
	set->nb = nb_par;
	return set;
}

void pset_free(pset * set){
	free(set->pos);
	free(set->spd);
	free(set->acc);
	free(set->m);
	free(set);
}

void pset_print(pset * set)
{
	int i;
	int size = set->nb;
	for (i = 0; i < size; ++i)
	{
		printf("#Particule numéro : %d, de masse %g\n", i, set->m[i]);
		printf("\tx:%g y:%g\n", set->pos[i], set->pos[i+ size]);
		printf("\tvx:%g vy:%g\n",set->spd[i], set->spd[i+ size]);
		printf("\tax:%g ay:%g\n",set->acc[i], set->acc[i+ size]);
	}
}

void pset_init_rand(pset * s)
{
	int i;
	int size = s->nb;
	if(!SEEDED)
	{
		unsigned long seed = mix(clock(), time(NULL), getpid());
		srand(seed);
		SEEDED++;
	}
	for (i = 0; i < size; i++)
	{
		s->m[i] = 1.0e10;
		s->pos[i] = MIN_RAND + rand()%(MAX_RAND-MIN_RAND);
		s->pos[i+size] = MIN_RAND + rand()%(MAX_RAND-MIN_RAND);
		s->spd[i] = 0;
		s->spd[i+size] = 0;
		s->acc[i] = 0;
		s->acc[i+size] = 0;
	}
}

/** Calcule la distance entre deux particules données par leurs
 * coordonnées cartésiennes.
 */
double distance(double x1, double y1, double x2, double y2)
{
	return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}

/** Prend en argument les masses de deux particules, la distance entre
 * ces particules et retourne l'intensité de la force gravitationnelle
 * entre ces deux particules
 */
double intensity(double m1, double m2, double d)
{
	return (CONST_GRAV * m1*m2 / (d*d*d));
}

void f_grav(pset * s1, pset* s2)
{
	// TODO : Si s1 et s2 sont en réalité le même ensemble
	// Alors il y a un problème au moment du calcul de l'intensité 
	// pour la même particule !

	int i, j;
	double d, inten;
	int size = s1->nb, size2= s2->nb;
	for (i = 0; i < size; ++i)
	{
		double force[2] = {0.0, 0.0};
		for (j = 0; j < size2; ++j)
		{
			d = distance(s1->pos[i], s1->pos[i+ size],
						 s2->pos[i], s2->pos[i+ size2]);
			inten = intensity(s1->m[i], s2->m[i], d);
			force[0]+= inten *(s2->pos[i] - s1->pos[i]); 
			force[1]+= inten *(s2->pos[i+size2] - s1->pos[i+size]);
		}
		s1->acc[i] =force[0]/ s1->m[i];
		s1->acc[i+size] = force[1]/ s1->m[i];
	}
}

void update_spd(pset * s){
	int size = s->nb;
	for (int i = 0; i < size; ++i)
	{
		s->spd[i]+= dt* s->acc[i];
		s->spd[i + size]+= dt* s->acc[i+ size];
	}
}

void update_pos(pset * s){
	int size = s->nb;
	for (int i = 0; i < size; ++i)
	{
		s->pos[i]+= dt* s->spd[i] + dt*dt/2 * s->acc[i];
		s->pos[i + size]+= dt* s->spd[i+ size] + dt*dt/2 * s->acc[i+size];
	}
}

void pset_step(pset * s)
{
	update_spd(s);
	update_pos(s);
}