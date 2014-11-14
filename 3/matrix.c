#include "matrix.h"

#include <stdio.h>
#include <stdlib.h>

matrix * matrix_load(const char * filename){
	FILE * file = fopen(filename, "r");
	if(file == NULL){
		fprintf(stderr, "%s: %s\n", filename, "Error while opening the matrix file.");
		exit(EXIT_FAILURE);
	}
	int size;
	matrix * m = malloc(sizeof(matrix));
	fscanf(file, "%d\n", &size);
	m->size = size;
	m->content = (double *) malloc(size*size* sizeof(double));

	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			fscanf(file, "%lf", &m->content[i+size*j]);
		}
	}
	fclose(file);
	return m;
}
void matrix_free(matrix * m){
	free (m->content);
	free (m);	
}

void matrix_print(FILE* stream, matrix * m)
{
	int size = m->size;
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			fprintf(stream, "%g ", m->content[i+size*j]);
		}
		fprintf(stream, "\n");
	}
}

void matrix_store(const char * filename, matrix * m){
	FILE * file = fopen(filename, "w+");
	if(file == NULL){
		fprintf(stderr, "%s: %s\n", filename, "Error while opening the write file.");
		exit(EXIT_FAILURE);
	}
	int size = m->size ;
	fprintf(file, "%d\n", size);
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			fprintf(file, "%lf ", m->content[i+size*j]);
		}
		fprintf(file, "\n");
	}
	fclose(file);
}

void matrix_dispatch(matrix* m);

