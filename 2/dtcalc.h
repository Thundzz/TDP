#ifndef DTCALC_H
#define DTCALC_H

#include "particule.h"

double dt_update_calc(double dt, double dist, double spdx1, double spdy1, double accx1, double accy1,
					double spdx2, double spdy2, double accx2, double accy2);
double dt_update(double dt, pset *s1, pset *s2);

#endif