#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"


void matrix_zero(matrix* m, int size){
	m->size = size;
	m->content = (double *) malloc(size*size* sizeof(double));

	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			m->content[i+size*j] = 0;
		}
	}
}

int matrix_load(matrix* m, const char * filename){
	FILE * file = fopen(filename, "r");
	if(file == NULL){
		fprintf(stderr, "%s: %s\n", filename,
			    "Error while opening the matrix file.");
		exit(EXIT_FAILURE);
	}
	int size, scanned;
	//m = malloc(sizeof(matrix));
	scanned = fscanf(file, "%d\n", &size);
	m->size = size;
	m->content = (double *) malloc(size*size* sizeof(double));

	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			scanned= fscanf(file, "%lf", &m->content[i+size*j]);
			if (scanned !=1)
				fprintf(stderr, "%s: Warning. Problem while parsing the matrix file.\n", filename);
		}
	}
	fclose(file);
	return size;
}
void matrix_free(matrix * m){
	free (m->content);
	//free (m);	
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

