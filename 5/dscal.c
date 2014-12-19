

void cblas_dscal(const int N, const double alpha, double *X, const int incX){
	int i;
	for (i = 0; i < N; i++)
	{
		X[i * incX]= alpha * X[i * incX];
	}
}