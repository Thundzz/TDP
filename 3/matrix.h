#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>

typedef struct Matrix{
	double * content;
	int size;
} matrix;

/* Dans toute la suite, les matrices
 seront allouées au format column major*/


/** Prend en argument un nom de fichier.
 *  Charge le fichier passé en retourne une matrice m allouée
 *  dynamiquement.
 *  Le fichier passé doit être au format :
 *  <Taille de la matrice = N> 
 *  <a00> <a01> <a02> ... <a0N>
 *  <a10> <a11> <a12> ... <a1N>
 *	...
 */
matrix * matrix_load(const char * filename);

/** Prend en argument un nom de fichier et une matrice,
 * et sauvegarde cette matrice dans le fichier en question
 * au format donné précédemment.
 */
void matrix_store(const char * filename, matrix * m);


/** Affiche la matrice m passée en paramètre sur le flux stream.
 */
void matrix_print(FILE* stream, matrix *  m);

/** Libère une matrice allouée à l'aide de matrix_load
 */
void matrix_free(matrix * m);

/** Répartit la matrice m entre tous les processus MPI
 * disponibles.
 */
void matrix_dispatch(matrix* m);



#endif