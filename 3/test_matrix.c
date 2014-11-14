#include "matrix.h"
#include "stdio.h"

#define IDENTITY_FILE "mat_identity.dat"

int main(void)
{
	matrix * m = matrix_load(IDENTITY_FILE);
	matrix_print(stdout, m);
	matrix_free(m);
	return 0;
}