#ifndef PARTICULE_H
#define PARTICULE_H


#define CONST_GRAV 6.6742e-11


struct p_set {

	int nb;
	double * m;
	double * acc;
	double * spd;
	double * pos;
};

typedef struct p_set pset;


/** Alloue un ensemble de nb_par particules.
 **/
pset * pset_alloc(int nb_par);

/** Désallloue un ensemble de particules.
**/
void pset_free(pset * s);

/** Calcule la force gravitationnelle entre les
 * deux ensembles de particules passées en paramètre, et met à jour
 * les accélérations.
 */
void f_grav(pset * s1, pset* s2);

/** Met à jour les vitesses de toutes les particules de l'ensemble p
 *  en supposant que la valeur de l'accélération est correcte.
 **/
void update_spd(pset * s);


/** Met à jour les positions de toutes les particules de l'ensemble p
 *  en supposant que la valeur de la vitesse est correcte.
 **/
void update_pos(pset* s);


#endif