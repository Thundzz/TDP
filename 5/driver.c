#include "util.h"
#include "cblas.h"
#include "mylapack.h"

#include <assert.h>
#include <math.h>

#define MATSIZE 6
#define ROWSIZE 3
#define COLSIZE 4
#define EPSILON 1e-5

static int test_total= 0;
static int test_passed= 0;
static int test_failed= 0;

int test_dscal()
{
	int alpha = 3;
	double * A = alloc(MATSIZE, 1);
	double * B = alloc(MATSIZE, 1);
	MATRIX_init_test(MATSIZE, 1, A, 1);
	MATRIX_init_test(MATSIZE, 1, B, 1);
	cblas_dscal(MATSIZE, alpha, A, 1);
	for (int i = 0; i < MATSIZE; ++i)
	{
		assert((alpha*B[i]) == A[i]);		
	}

	test_passed ++;
	return 0;
}

int test_dgetf2_square()
{
	double * L = alloc(MATSIZE, MATSIZE);
	double * U = alloc(MATSIZE, MATSIZE);
	double * A = alloc(MATSIZE, MATSIZE);

	MATRIX_init_lower(MATSIZE, L, MATSIZE);
	MATRIX_init_upper(MATSIZE, U, MATSIZE);

#ifdef VERBOSE
	MATRIX_affiche(MATSIZE, MATSIZE, U, MATSIZE, stdout);
	MATRIX_affiche(MATSIZE, MATSIZE, L, MATSIZE, stdout);
#endif
	cblas_dgemm_scalaire(MATSIZE, MATSIZE, MATSIZE,1.0,
							  L, MATSIZE,
				 			  U, MATSIZE,
                 			  A, MATSIZE);
#ifdef VERBOSE
	MATRIX_affiche(MATSIZE, MATSIZE, A, MATSIZE, stdout);
#endif
	LAPACKE_dgetf2(0, MATSIZE, MATSIZE , A, MATSIZE, NULL );
#ifdef VERBOSE
	MATRIX_affiche(MATSIZE, MATSIZE, A, MATSIZE, stdout);
#endif
	for (int i = 0; i < MATSIZE; ++i)
	{
		for (int j = 0; j < MATSIZE; ++j)
		{
			if(j>=i)
				assert(A[i+j*MATSIZE] == U[i+j*MATSIZE]);
			else
				assert(A[i+j*MATSIZE] == L[i+j*MATSIZE]);
		}
	}
	test_passed ++;
	return 0;
}

int test_dgetf2_general()
{
	double * L = alloc(MATSIZE, MATSIZE);
	double * U = alloc(MATSIZE, MATSIZE);
	double * A = alloc(MATSIZE, MATSIZE);

	MATRIX_init_lower(MATSIZE, L, MATSIZE);
	MATRIX_init_upper(MATSIZE, U, MATSIZE);
#ifdef VERBOSE
	MATRIX_affiche(MATSIZE, MATSIZE, U, MATSIZE, stdout);
	MATRIX_affiche(MATSIZE, MATSIZE, L, MATSIZE, stdout);
#endif
	cblas_dgemm_scalaire(MATSIZE, MATSIZE, MATSIZE,1.0,
							  L, MATSIZE,
				 			  U, MATSIZE,
                 			  A, MATSIZE);
#ifdef VERBOSE
	MATRIX_affiche(MATSIZE, MATSIZE, A, MATSIZE, stdout);
#endif
	LAPACKE_dgetf2(0, COLSIZE, ROWSIZE , A, MATSIZE, NULL );

#ifdef VERBOSE
	MATRIX_affiche(COLSIZE, ROWSIZE, A, MATSIZE, stdout);
#endif

	for (int i = 0; i < COLSIZE; ++i)
	{
		for (int j = 0; j < ROWSIZE; ++j)
		{
			if(j>=i)
				assert(A[i+j*MATSIZE] == U[i+j*MATSIZE]);
			else
				assert(A[i+j*MATSIZE] == L[i+j*MATSIZE]);
		}
	}
	test_passed ++;
	return 0;
}

int test_dgetf2_piv()
{
	int size = 9;
	double A[] =  {7, -6, -11, 3, 7, 2, -11, 10, -2 };
	double res[] = {-11, 0.545455, -0.636364, 2, 5.90909, 0.723077, -2, 11.0909,  -20.2923};
	int piv[3] = {0}; 
#ifdef VERBOSE
	MATRIX_affiche(3, 3, A, 3, stdout);
#endif
	LAPACKE_dgetf2_piv(0, 3, 3 , A, 3, piv );
#ifdef VERBOSE
	MATRIX_affiche(3, 3, A, 3, stdout);
#endif
	for (int i = 0; i < size; ++i)
	{
		assert(fabs(A[i] - res[i]) <= EPSILON);
		i++;
	}
	test_passed ++;
	return 0;
}

int test_dgetrf_piv()
{
	int size = 9;
	double A[] =  {7, -6, -11, 3, 7, 2, -11, 10, -2 };
	double res[] = {-11, 0.545455, -0.636364, 2, 5.90909, 0.723077, -2, 11.0909,  -20.2923};
	int piv[3] = {0}; 
#ifdef VERBOSE
	MATRIX_affiche(3, 3, A, 3, stdout);
#endif
	LAPACKE_dgetrf_piv(0, 3, 3 , A, 3, piv );
#ifdef VERBOSE
	MATRIX_affiche(3, 3, A, 3, stdout);
#endif
	for (int i = 0; i < size; ++i)
	{
		assert(fabs(A[i] - res[i]) <= EPSILON);
		i++;
	}
	test_passed ++;
	return 0;
}

int test_dtrsm(){

	double * A = alloc(MATSIZE, MATSIZE);
	double * B = alloc(MATSIZE, 1);
	double * C = alloc(MATSIZE, 1);
	MATRIX_init_id(MATSIZE, MATSIZE, A, MATSIZE);
	MATRIX_init_id(MATSIZE, 1, B, MATSIZE);
	MATRIX_init_zero(MATSIZE, 1, C, MATSIZE);
#ifdef VERBOSE
	MATRIX_affiche(MATSIZE, MATSIZE, A, MATSIZE, stdout);
	MATRIX_affiche(MATSIZE, 1, B, MATSIZE, stdout);
#endif
	LAPACKE_dgetf2(0, MATSIZE, MATSIZE , A, MATSIZE, NULL );
#ifdef VERBOSE
	MATRIX_affiche(MATSIZE, MATSIZE, A, MATSIZE, stdout);
#endif
	LAPACKE_dtrsm(LAPACKE_LOWER, LAPACKE_UNIT, MATSIZE, MATSIZE,
 				1.0, A, MATSIZE, B, MATSIZE);
	LAPACKE_dtrsm(LAPACKE_UPPER, LAPACKE_NUNIT, MATSIZE, MATSIZE,
  				1.0, A, MATSIZE, B, MATSIZE);
#ifdef VERBOSE
	MATRIX_affiche(MATSIZE, 1, B, MATSIZE, stdout);
	MATRIX_affiche(MATSIZE, 1, C, MATSIZE, stdout);
#endif
	cblas_dgemm_scalaire(MATSIZE, MATSIZE, MATSIZE,1.0,
							  A, MATSIZE,
				 			  B, MATSIZE,
                 			  C, MATSIZE);
	for (int i = 0; i < MATSIZE; ++i)
	{
		assert(B[i] == C[i]);	
	}
	test_passed ++;
	return 0;
}

int test_example()
{
	double * A = alloc(MATSIZE, MATSIZE);
	double * b = alloc(MATSIZE, 1);
	double * x = alloc(MATSIZE, 1);	

	MATRIX_init_example_A(MATSIZE, MATSIZE, A, MATSIZE);
	MATRIX_init_example_b(MATSIZE,1, b, 1);
	MATRIX_init_example_x(MATSIZE,1, x, 1);
#ifdef VERBOSE
	MATRIX_affiche(MATSIZE, MATSIZE, A, MATSIZE, stdout);
	MATRIX_affiche(MATSIZE, 1, b, MATSIZE, stdout);
#endif

	LAPACKE_dgesv( MATSIZE, 1,  A,  MATSIZE , NULL, b, 1);
#ifdef VERBOSE
	MATRIX_affiche(MATSIZE, 1, b, MATSIZE, stdout);
	MATRIX_affiche(MATSIZE, 1, x, MATSIZE, stdout);
#endif	

	test_passed ++;
	return 0;
}

int test_example_multiple()
 {
 	double * A = alloc(MATSIZE, MATSIZE);
	double * b = alloc(MATSIZE, MATSIZE);
	double * x = alloc(MATSIZE, MATSIZE);	

	MATRIX_init_example_A(MATSIZE, MATSIZE, A, MATSIZE);
	MATRIX_init_example_b(MATSIZE, MATSIZE, b, MATSIZE);
	MATRIX_init_example_x(MATSIZE, MATSIZE, x, MATSIZE);
#ifdef VERBOSE
	MATRIX_affiche(MATSIZE, MATSIZE, A, MATSIZE, stdout);
	MATRIX_affiche(MATSIZE, MATSIZE, b, MATSIZE, stdout);
#endif
	LAPACKE_dgesv( MATSIZE, MATSIZE,  A,  MATSIZE , NULL, b, MATSIZE);
#ifdef VERBOSE
	MATRIX_affiche(MATSIZE, MATSIZE, b, MATSIZE, stdout);
	MATRIX_affiche(MATSIZE, MATSIZE, x, MATSIZE, stdout);
#endif

	for (int i = 0; i < MATSIZE; ++i)
	{
		for (int j = 0; j < MATSIZE; ++j)
		{
				assert(b[i+j*MATSIZE] == x[i+j*MATSIZE]);
		}
	}

	test_passed ++;
	return 0;	
 }

 int test_dgetrf()
{
	double * L = alloc(MATSIZE, MATSIZE);
	double * U = alloc(MATSIZE, MATSIZE);
	double * A = alloc(MATSIZE, MATSIZE);

	MATRIX_init_lower(MATSIZE, L, MATSIZE);
	MATRIX_init_upper(MATSIZE, U, MATSIZE);
#ifdef VERBOSE
	MATRIX_affiche(MATSIZE, MATSIZE, U, MATSIZE, stdout);
	MATRIX_affiche(MATSIZE, MATSIZE, L, MATSIZE, stdout);
#endif
	cblas_dgemm_scalaire(MATSIZE, MATSIZE, MATSIZE,1.0,
							  L, MATSIZE,
				 			  U, MATSIZE,
                 			  A, MATSIZE);
#ifdef VERBOSE
	MATRIX_affiche(MATSIZE, MATSIZE, A, MATSIZE, stdout);
#endif
	LAPACKE_dgetrf(0, MATSIZE, MATSIZE , A, MATSIZE, NULL );

#ifdef VERBOSE
	MATRIX_affiche(MATSIZE, MATSIZE, A, MATSIZE, stdout);
#endif

	for (int i = 0; i < COLSIZE; ++i)
	{
		for (int j = 0; j < ROWSIZE; ++j)
		{
			if(j>=i)
				assert(A[i+j*MATSIZE] == U[i+j*MATSIZE]);
			else
				assert(A[i+j*MATSIZE] == L[i+j*MATSIZE]);
		}
	}
	test_passed ++;
	return 0;
}

int main(void)
{
	test_dscal();
	test_dgetf2_square();
	test_dgetf2_general();
	test_dtrsm();
	test_example();
	test_example_multiple();
	test_dgetrf();
	test_dgetf2_piv();
	test_dgetrf_piv();

	test_total = test_passed + test_failed;
	printf(ANSI_COLOR_GREEN"%d tests run and passed.\n"ANSI_COLOR_RESET, test_passed);
	return 0;
}