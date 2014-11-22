#ifndef GRID_H
#define GRID_H

void proc(int N, int Nb, 
			double* bl_a, double* bl_b, double* bl_c,
			int myrank, int np, int np_c,
			MPI_Comm comm_grid, MPI_Comm comm_col, MPI_Comm comm_row);

/** Coupe la matrice en blocs et la répartit sur la grille
 *  a est la matrice initiale
 *	np_r définit le nombre de processus sur chaque ligne
 *	np_c le nombre de processus sur chaque colonne
 *
 *	Crée le datatype MPI correspondant à un bloc
 *	Retourne un bloc -qui dépend évidemment du processus appelant- 
 **/
double* partition_matrix(double *a,
						int N, int np_r, int np_c, 
						MPI_Datatype *type_block);


/** Etablit la grille de communication entre les processus 
 *  
 *	np_r définit le nombre de processus sur chaque ligne
 *	np_c le nombre de processus sur chaque colonne
 *  
 *	Crée deux communicateurs : 
 *	comm_row qui permet aux processus de communiquer avec
 *	les autres processus sur la même ligne
 *	comm_col sur la même colonne
 **/
void create_grid(int myrank, int np_r, int np_c,
					MPI_Comm* comm_grid, MPI_Comm* comm_row, MPI_Comm* comm_col);

#endif
