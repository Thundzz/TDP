#include "util.h"
#include "cblas.h"
#include "assert.h"
#include "mylapack.h"

#define MATSIZE 6
#define ROWSIZE 3
#define COLSIZE 4


static int test_total= 0;
static int test_passed= 0;
static int test_failed= 0;

int test_dscal()
{
	double * A = alloc(MATSIZE, MATSIZE);
	MATRIX_init_test(MATSIZE, MATSIZE, A, MATSIZE);
	MATRIX_affiche(MATSIZE, MATSIZE, A, MATSIZE, stdout);
	cblas_dscal(MATSIZE, 3, A, 1);
	MATRIX_affiche(MATSIZE, MATSIZE, A, MATSIZE, stdout);
}

int test_dgetf2_square()
{
	double * L = alloc(MATSIZE, MATSIZE);
	double * U = alloc(MATSIZE, MATSIZE);
	double * A = alloc(MATSIZE, MATSIZE);

	MATRIX_init_lower(MATSIZE, L, MATSIZE);
	MATRIX_init_upper(MATSIZE, U, MATSIZE);


	MATRIX_affiche(MATSIZE, MATSIZE, U, MATSIZE, stdout);
	MATRIX_affiche(MATSIZE, MATSIZE, L, MATSIZE, stdout);

	cblas_dgemm_scalaire(MATSIZE, MATSIZE, MATSIZE,1.0,
							  L, MATSIZE,
				 			  U, MATSIZE,
                 			  A, MATSIZE);

	MATRIX_affiche(MATSIZE, MATSIZE, A, MATSIZE, stdout);
	
	LAPACKE_dgetf2(NULL, MATSIZE, MATSIZE , A, MATSIZE, NULL );

	MATRIX_affiche(MATSIZE, MATSIZE, A, MATSIZE, stdout);
	return 0;
}

int test_dgetf2_general()
{
	double * L = alloc(MATSIZE, MATSIZE);
	double * U = alloc(MATSIZE, MATSIZE);
	double * A = alloc(MATSIZE, MATSIZE);

	MATRIX_init_lower(MATSIZE, L, MATSIZE);
	MATRIX_init_upper(MATSIZE, U, MATSIZE);

	MATRIX_affiche(MATSIZE, MATSIZE, U, MATSIZE, stdout);
	MATRIX_affiche(MATSIZE, MATSIZE, L, MATSIZE, stdout);

	cblas_dgemm_scalaire(MATSIZE, MATSIZE, MATSIZE,1.0,
							  L, MATSIZE,
				 			  U, MATSIZE,
                 			  A, MATSIZE);

	MATRIX_affiche(MATSIZE, MATSIZE, A, MATSIZE, stdout);
	
	LAPACKE_dgetf2(NULL, COLSIZE, ROWSIZE , A, MATSIZE, NULL );

	MATRIX_affiche(COLSIZE, ROWSIZE, A, MATSIZE, stdout);
	return 0;
}


int test_dtrsm(){

	double * A = alloc(MATSIZE, MATSIZE);
	double * B = alloc(MATSIZE, 1);
	double * C = alloc(MATSIZE, 1);
	MATRIX_init_id(MATSIZE, MATSIZE, A, MATSIZE);
	MATRIX_init_id(MATSIZE, 1, B, MATSIZE);

	MATRIX_affiche(MATSIZE, MATSIZE, A, MATSIZE, stdout);
	MATRIX_affiche(MATSIZE, 1, B, MATSIZE, stdout);

	LAPACKE_dgetf2(NULL, MATSIZE, MATSIZE , A, MATSIZE, NULL );
	
	MATRIX_affiche(MATSIZE, MATSIZE, A, MATSIZE, stdout);

	LAPACKE_dtrsm(LAPACKE_LOWER, LAPACKE_UNIT, MATSIZE, MATSIZE,
 				1.0, A, MATSIZE, B, MATSIZE);
	// LAPACKE_dtrsm(LAPACKE_UPPER, LAPACKE_NUNIT, MATSIZE, MATSIZE,
 // 				1.0, A, MATSIZE, B, MATSIZE);

	MATRIX_affiche(MATSIZE, 1, B, MATSIZE, stdout);

	cblas_dgemm_scalaire(MATSIZE, MATSIZE, MATSIZE,1.0,
							  A, MATSIZE,
				 			  B, MATSIZE,
                 			  C, MATSIZE);

	MATRIX_affiche(MATSIZE, 1, C, MATSIZE, stdout);
}

int main(void)
{
	//test_dscal();
	//test_dgetf2_square();
	//test_dgetf2_general();
	test_dtrsm();
	printf("%d tests run. (%d Passed, %d Failed)\n", test_total,
	 test_passed, test_failed);
	return 0;
}