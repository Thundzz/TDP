#ifndef GRID_H
#define GRID_H

#include "mpi.h"
#include "matrix.h"

/** Produit matriciel scalaire sur deux matrices carrées A et B de
 *  taille N.
 *  Résultat inscrit dans la matrice C
 **/ 
void cblas_dgemm_scalaire(const int N,
	const double *A, const double *B, double *C);

/** Réalise le produit matriciel par blocs
 **/
void prod_matrix(int N, int Nb, int myrank,
			double* bl_a, double* bl_b, double* bl_c,
			MPI_Comm comm_grid, MPI_Comm comm_col, MPI_Comm comm_row);

/** Coupe la matrice en Nb blocs et la répartit sur la grille
 *  a est la matrice initiale
 *
 *	Crée le datatype MPI correspondant à un bloc
 *	Retourne un bloc -qui dépend évidemment du processus appelant- 
 **/
double* partition_matrix(double *a,
						int N, int gd,
						MPI_Datatype *type_block);

/** Réassemble la matrice finale
 **/
matrix gather_matrix(double* c,
					int N, int gd, 
					MPI_Datatype * type_block);

/** Etablit la grille de communication de taille 
 *	gd*gd entre les processus 
 *  
 *	Crée deux communicateurs : 
 *	comm_row qui permet aux processus de communiquer avec
 *	les autres processus sur la même ligne
 *	comm_col sur la même colonne
 **/
void create_grid(int myrank, int gd,
					MPI_Comm* comm_grid, MPI_Comm* comm_row, MPI_Comm* comm_col);

#endif
