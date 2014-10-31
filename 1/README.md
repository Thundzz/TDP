TDP
===

#Compilation
* make pour compiler
* ./benchmark_ddot "nom_de_fichier" pour effectuer le benchmark de ddot
* Pour faire un benchmark d'une des versions du dgemm :
	1. Modifier les sources en remplaçant dans la fonction test_dgemm() du fichier benchmark_dgemm.c, la fonction cblas_dgemm_block_parallel() par l'une des autres fonctions dgemm de dgemm.c
	2. Modifier le nom du fichier de sortie
	3. Recompiler
	4. Executer ./benchmark_dgemm

#Tracer les résultats
* Charger dans gnuplot les fichiers .plt...
