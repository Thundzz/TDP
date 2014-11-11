#include <stdio.h>
#include <math.h>
#include <float.h>
#include "particule.h"
#include "dtcalc.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

void mindist_update(pset *s1, pset *s2)
{
}

double quad_eq_positive_sol(double a, double b, double c)
{
	double x1;
	double x2;

	if (a == 0.0 && b == 0.0)
		return DBL_MAX;
	else if (a == 0)
		return -c/b;

	double delta = b*b - 4.0*a*c;
	if (delta > 0)
	{
		x1 = (-b + sqrt(delta)) / (2.0*a);
		x2 = (b + sqrt(delta)) / (2.0*a); 
		if (x1 > 0)
			return x1;
		else if(x2 > 0)
			return x2;
		else 
			return DBL_MAX;
	}
	else if(delta == 0)
		return - b/(2.0*a);
	else
	{
		return DBL_MAX; 
	}
}

double dt_update_calc(double dt, double dist, double spdx1, double spdy1, double accx1, double accy1,
					double spdx2, double spdy2, double accx2, double accy2)
{
	double newdt;
	double newdt1;
	double newdt2;

	double acc1 = sqrt(accx1*accx1 + accy1*accy1);
	double acc2 = sqrt(accx2*accx2 + accy2*accy2);
	double spd1 = sqrt(spdx1*spdx1 + spdy1*spdy1);
	double spd2 = sqrt(spdx2*spdx2 + spdy2*spdy2);

	newdt1 = quad_eq_positive_sol(0.5*acc1, spd1, -0.1*dist);
	newdt2 = quad_eq_positive_sol(0.5*acc2, spd2, -0.1*dist);

	newdt = MIN(dt, MIN(newdt1, newdt2));
	return newdt;
}

double dt_update(double dt, pset *s1, pset *s2)
{
	int i, j;
	double x1, y1, x2, y2, dist;
	double newdt = dt;
	int size1 = s1->nb;
	int size2 = s2->nb;

	for (i = 0; i < size1; ++i)
	{
		for (j = 0; j < size2; ++j)
		{
			x1 = s1->pos[i];
			x2 = s2->pos[j];
			y1 = s1->pos[i+size1];
			y2 = s2->pos[j+size2];
			dist = distance(x1, y1, x2, y2);
			newdt = dt_update_calc(dt, dist,
								s1->spd[i], s1->spd[i+size1], s1->acc[i], s1->acc[i+size1],
								s2->spd[i], s2->spd[i+size2], s2->acc[i], s2->acc[i+size2]);
		}
	}
	return newdt;
}