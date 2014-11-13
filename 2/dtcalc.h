#ifndef DTCALC_H
#define DTCALC_H

#include "particule.h"

/** Calcule le dt lié a un atome et la distance à son voisin le plus proche
 */
double dt_local_update_calc(double dist, double spdx, double spdy, 
							double accx, double accy);

/** Retourne le minimum local des dt liés aux atomes d'un set
 */
double dt_local_update(double defdt, pset *s);

/** Récupère le nouveau dt global par réduction
 */
void dt_global_update(double* locdt);

#endif