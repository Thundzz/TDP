#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "cblas.h"
#include "util.h"


#define UNUSED(x) (void)(x)
#define BLOCK_SIZE 40

/*A[i][j] === j*lda + i*/

void cblas_dgemm_scalaire_ikj(const int M,
				 const double *A, const int lda,
				 const double *B, const int ldb,
                 double *C, const int ldc)
{
	int i, j, k;
	for (i = 0; i < M; ++i)
	{
		for (k = 0; k < M; ++k)
		{
			for (j = 0; j < M; ++j)
			{
				//C[i][j] += A[k][i]*B[k][j];
				C[j*ldc +i] += A[i*lda +k]* B[j*ldb +k];
			}
		}
	}
}

void cblas_dgemm_scalaire_kij(const int M,
				 const double *A, const int lda,
				 const double *B, const int ldb,
                 double *C, const int ldc)
{
	int i, j, k;
	for (k = 0; k < M; ++k)
	{
		for (i = 0; i < M; ++i)
		{
			for (j = 0; j < M; ++j)
			{
				//C[i][j] += A[k][i]*B[k][j];
				C[j*ldc +i] += A[i*lda +k]* B[j*ldb +k];
			}
		}
	}

}

void cblas_dgemm_scalaire_ijk(const int M,
				 const double *A, const int lda,
				 const double *B, const int ldb,
                 double *C, const int ldc)
{
	int i, j, k;
	for (i = 0; i < M; ++i)
	{
		for (j = 0; j < M; ++j)
		{	
			for (k = 0; k < M; ++k)
			{
				//C[i][j] += A[k][i]*B[k][j];
				C[j*ldc +i] += A[i*lda +k]* B[j*ldb +k];
			}
		}
	}
}

void cblas_dgemm_scalaire_jik(const int M,
				 const double *A, const int lda,
				 const double *B, const int ldb,
                 double *C, const int ldc)
{
	int i, j, k;
	for (j = 0; j < M; ++j)
	{
		for (i = 0; i < M; ++i)
		{
			for (k = 0; k < M; ++k)
			{
				//C[i][j] += A[k][i]*B[k][j];
				C[j*ldc +i] += A[i*lda +k]* B[j*ldb +k];
			}
		}
	}
}

//C = C + alpha*A*B
void cblas_dgemm_scalaire(const int M, const int K, const int N,
							  const double alpha, const double *A, const int lda,
				 			  const double *B, const int ldb,
                 			        double *C, const int ldc)
{
	int i, j, k;
	register double tmp;  //Stocke tmp dans un registre
	for (i = 0; i < M; ++i)
	{
		for (j = 0; j < N; ++j)
		{	
			tmp = 0;  
			for (k = 0; k < K; ++k)
			{
				tmp += A[k*lda +i]* B[j*ldb +k];
			}
			C[j*ldc +i] += tmp * alpha;
		}
	}
}

/*
	A needs to be K*N for this function to work
	Version bloc de dgemm
*/
void cblas_dgemm_block(const int M, const int K, const int N,
					   const double alpha, const double * A, const int lda,
					   const double * B, const int ldb,
					   		 double * C, const int ldc)
{
	int M2, N2, K2, i, j, k;
	for (i = 0; i < M; i +=BLOCK_SIZE)
	{
		for (j = 0; j < N; j +=BLOCK_SIZE)
		{
			for (k = 0; k < K; k +=BLOCK_SIZE)
			{
				// Ajustement de la taille du bloc pour ne pas déborder
				M2 = MIN(BLOCK_SIZE, M-i);
				N2 = MIN(BLOCK_SIZE, N-j);
				K2 = MIN(BLOCK_SIZE, K-k);
				cblas_dgemm_scalaire(M2, K2, N2, alpha,
									 A+ i*lda +k, lda,
				 					 B+ j*ldb +k, ldb,
				 					 C+ j*ldc +i, ldc);
			}
		}
	}
}

void cblas_dgemm(const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_TRANSPOSE TransB, const int M, const int N,
                 const int K, const double alpha, const double *A,
                 const int lda, const double *B, const int ldb,
                 const double beta, double *C, const int ldc)
{
	UNUSED(beta);
	if (Order == CblasRowMajor)
	{
		fprintf(stderr, "CblasRowMajor not supported. Reorder your matrices and use CblasColMajor instead\n");
		return;
	}
	if (TransA == CblasNoTrans || TransB == CblasTrans)
	{
		fprintf(stderr, "This implementation only supports C = C+alpha*trans(A)*B. Nothing has been done.\n");
		return;
	}

	cblas_dgemm_block(M, K, N, 
						alpha, A, lda,
					   	B, ldb,
					   	C, ldc);	
}


struct params{
	int M;
	int K;
	int N;
	double alpha;
	const double * A;
	int lda;
	const double * B;
	int ldb;
	double * C;
	int ldc;
	int end;
	int start;
};

void* thread_f(void* params)
{
	struct params *p = (struct params*) params;
	int M = p->M;
	int K = p->K;
	int N = p->N;
	double alpha = p->alpha;
	const double * A = p->A;
	int lda = p->lda;
	const double * B = p->B;
	int ldb = p->ldb;
	double * C = p->C;
	int ldc = p->ldc;
	int end = p->end;
	int start = p->start;
	//printf("start:%d, end:%d\n", start, end);
	int M2, N2, K2, i, j, k;
	for (i = 0; i < M; i +=BLOCK_SIZE)
	{
		for (j = start; j < end; j +=BLOCK_SIZE)
		{
			for (k = 0; k < K; k +=BLOCK_SIZE)
			{
				M2 = MIN(BLOCK_SIZE, M-i);
				N2 = MIN(BLOCK_SIZE, N-j+start);
				K2 = MIN(BLOCK_SIZE, K-k);

				cblas_dgemm_scalaire(M2, K2, N2, alpha,
									 A+ i*lda +k, lda,
				 					 B+ j*ldb +k, ldb,
				 					 C+ j*ldc +i, ldc);
			}
		}
	}
	return 0;
}

/*dgemm version bloc, parallèle. Découpe les colonnes en nbthreads régions
**Chaque région est ensuite calculée par un thread séparé 
*/
void cblas_dgemm_block_parallel(const int M, const int K, const int N,
					   			const double alpha, const double * A, const int lda,
					   			const double * B, const int ldb,
					   		 		  double * C, const int ldc)
{
	int nbthreads;
	if (getenv("MYLIB_NUM_THREADS") == NULL)
		nbthreads = 5; //Par défaut, 5 threads
	else
		nbthreads = atoi(getenv("MYLIB_NUM_THREADS"));
	if (nbthreads > N)
		nbthreads = N;

	int region = N/nbthreads;

	struct params p;
	p.M = M;
	p.K = K;
	p.N = region;
	p.alpha = alpha;
	p.A = A;
	p.lda = lda;
	p.B = B;
	p.ldb = ldb;
	p.C = C;
	p.ldc = ldc;

	pthread_t *threads = malloc(nbthreads * sizeof(pthread_t));
	struct params *ptab = malloc(nbthreads * sizeof(struct params));

	int i;
	for (i = 0; i<nbthreads; i++)
	{ 
		//Découpage de N en nbthreads parts
		ptab[i] = p;
		ptab[i].start = i*region;
		if (i != nbthreads-1)
		{
			ptab[i].end = (i+1)*region;
		}		
		else
		{
			ptab[i].end = (i+1)*region+N%region;
			ptab[i].N = region+N%region;
		}
		pthread_create(&threads[i], NULL, thread_f, &ptab[i]);
	}
	for (i = 0; i<nbthreads; i++)
	{
		pthread_join(threads[i], NULL);
	}
	free(ptab);
	free(threads);
}

void cblas_pthread(const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_TRANSPOSE TransB, const int M, const int N,
                 const int K, const double alpha, const double *A,
                 const int lda, const double *B, const int ldb,
                 const double beta, double *C, const int ldc)
{
	UNUSED(beta);
	if (Order == CblasRowMajor)
	{
		fprintf(stderr, "CblasRowMajor not supported. Reorder your matrices and use CblasColMajor instead\n");
		return;
	}
	if (TransA == CblasNoTrans || TransB == CblasTrans)
	{
		fprintf(stderr, "This implementation only supports C = C+alpha*trans(A)*B. Nothing has been done.\n");
		return;
	}

	cblas_dgemm_block_parallel(M, K, N, 
								alpha, A, lda,
					   			B, ldb,
					   			C, ldc);	
}