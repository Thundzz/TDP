#ifndef DTCALC_H
#define DTCALC_H

#include "particule.h"

/** Calcule le dt pour un couple de particules pour que la différence 
 *	entre la position à t+dt et la position à t de chacun de ces deux atomes ne dépasse pas 10%
 *  de la distance les séparant à l'instant t.
 *  Le premier argument est le minimum des dt précédemment calculés. 
 *	La fonction retourne le minimum entre ce dernier et le dt qui vient d'être calculé.
 */
double dt_update_calc(double dt, double dist, double spdx1, double spdy1, double accx1, double accy1,
					double spdx2, double spdy2, double accx2, double accy2);

/** Retourne le minimum des dt pour chaque couple d'atomes de deux ensembles s1 et s2  
 */
double dt_update(double dt, pset *s1, pset *s2);

#endif