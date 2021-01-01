#ifndef NORM_H
#define NORM_H

float norm(float *U, int n);
float vect_norm(float *U, int n);


typedef struct
{
	unsigned int id;
	float *U;
	long deb;
	long fin;
    float res;
} thread_data;


void *thread_function(void *threadarg);
void *thread_function_vect(void *threadarg);

float normPar(float *U, int n, int nb_threads, int mode);

#endif