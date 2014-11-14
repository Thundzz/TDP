#include "matrix.h"
#include "stdio.h"

#define LOAD_FILE "mat_identity.dat"
#define WRITE_FILE "not_identity.dat"



int main(void)
{
	matrix * m = matrix_load(LOAD_FILE);
	matrix_print(stdout, m);
	matrix_store(WRITE_FILE, m);
	matrix_free(m);
	return 0;
}