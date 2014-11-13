#include <stdio.h>
#include <math.h>
#include <float.h>
#include <mpi.h>
#include "particule.h"
#include "dtcalc.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

/** Calcule la solution réelle positive d'une équation du second degré : 
 *	ax*x + bx + c = 0, avec a strictement positif, b positif et c négatif.
 *  Retourne DBL_MAX pour le cas ou aucune racine réelle positive existe 
 *	ou si a et b sont nuls pour ne pas influencer la valeur de dtmin
 */

double quad_eq_positive_sol(double a, double b, double c)
{
	double x1;
	double x2;

	if (a == 0.0 && b == 0.0)
		return DBL_MAX;
	else if (a == 0)
		return -c/b; //Toujours positif

	double delta = b*b - 4.0*a*c;
	if (delta > 0)
	{
		x1 = (-b + sqrt(delta)) / (2.0*a);
		x2 = (b + sqrt(delta)) / (2.0*a); 
		if (x1 > 0 && x2 > 0)
			return MIN(x1, x2);
		else if(x1 < 0 && x2 < 0)
			return DBL_MAX;
		else 
			return MAX(x1, x2);
	}
	else
	{
		return DBL_MAX; //Le cas delta = 0 donne x = -b/(2*a) toujours négatif
	}
}

double dt_local_update_calc(double dist, double spdx, double spdy, 
							double accx, double accy)	
{
	double acc = sqrt(accx*accx + accy*accy);
	double spd = sqrt(spdx*spdx + spdy*spdy);

	return quad_eq_positive_sol(0.5*acc, spd, -0.1*dist);
}

double dt_local_update(double defdt, pset *s)
{
	int i;
	double spdx, spdy, accx, accy, dist;
	double newdt = defdt;
	int size = s->nb;

	for (i = 0; i < size; ++i)
	{
		spdx = s->spd[i];
		spdy = s->spd[i+size];
		accx = s->acc[i];
		accy = s->acc[i+size];
		dist = s->dmin[i];
		if (!(s->m[i] ==0))
			newdt = MIN(newdt, dt_local_update_calc(dist, spdx, spdy, accx, accy));
	}
	return newdt;
}

void dt_global_update(double* locdt)
{
	double globdt;
	MPI_Allreduce(locdt, &globdt, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
	*locdt = globdt;
}