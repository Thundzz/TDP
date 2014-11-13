#ifndef PARTICULE_H
#define PARTICULE_H


#define CONST_GRAV 6.6742e-11
#define MIN_RAND 0
#define MAX_RAND 1000

struct p_set {
	int nb;
	double * m;
	double * acc;
	double * spd;
	double * pos;
	double * dmin;
};

typedef struct p_set pset;


/** Alloue un ensemble de nb_par particules.
 **/
pset * pset_alloc(int nb_par);


/** Copie le contenu du pset origin dans le pset dest
 **/
void pset_copy(pset * origin, pset * dest);

/** Désallloue un ensemble de particules.
**/
void pset_free(pset * set);



/** Initialise aléatoirement un set
**/
void pset_init_rand(pset * s);

/** Initialise un ensemble de particule contenant
 * une seule particule "soleil", de masse 1e10, 
 * et placée en position (x=0, y=0)
 **/
void pset_init_sun(pset * sun);

/** Suppose que le le pset primary contient une seule particule.
* Initialise l'ensemble de de particules satellites de façon à ce que 
* chaque particule de satellites orbite autour de la particule
* de primary.
*/
void pset_init_orbit(pset * primary, pset *satellites);
/** Affiche les infos sur les particules d'un ensemble
**/
void pset_print(pset * s);

/** Calcule la distance entre deux particules données par leurs
 * coordonnées cartésiennes.
 */
double distance(double x1, double y1, double x2, double y2);

/** Calcule la force gravitationnelle entre les
 * deux ensembles de particules passées en paramètre, et met à jour
 * les accélérations.
 */
void f_grav(pset * s1, pset* s2);

/** Met à jour les vitesses de toutes les particules de l'ensemble p
 *  en supposant que la valeur de l'accélération est correcte.
 **/
void update_spd(pset * s, double dt);


/** Met à jour les positions de toutes les particules de l'ensemble p
 *  en supposant que la valeur de la vitesse est correcte.
 **/
void update_pos(pset* s, double dt);


/** Réalise un pas d'itération sur tous les éléments de l'ensemble s:
* La position de chaque particule est mise à jour correctement, en 
* supposant que la valeur actuelle de l'accéléréation a déjà été
* calculée. i.e que f_grav a déjà été executée avec tous
* les autres ensembles de particules.
**/
void pset_step(pset * s, double dt);


#endif