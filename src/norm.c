#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "immintrin.h"
#include "stdint.h"
#include "../include/norm.h"
#include "../include/utils.h"
#include <stdio.h>

//Sequential implementation
float norm(float *U, int n)
{
    float result = 0;
    for (uint64_t i = 0; i < n; i++)
    {
        result += sqrt(fabsf(U[i]));
    }

    return (result);
}

//Vectorized implementation
float vect_norm(float *U, int n)
{

    float result = 0;
    uint64_t resultingN = n / 8;

    __m256 uPtr;

    __m256 vU = _mm256_setzero_ps();

    /*
    Given that : float x = [sign (1 bit) | exponent (8bit) | fraction (23bit)]
    We can change the sign of the float by altering the sign bit.
    We do it using a mask and a simple AND NOT logical operation (no arithmetics required)
    */
    const __m256 signMask = _mm256_castsi256_ps(_mm256_set1_epi32(0x80000000));

    for (size_t i = 0; i < resultingN; i++)
    {
        //We use _mm256_loadu_ps to support un-aligned memory data as input (U)
        uPtr = _mm256_loadu_ps(U + i * 8);
        vU = _mm256_add_ps(vU, _mm256_sqrt_ps(_mm256_andnot_ps(signMask, uPtr)));
    }

    float intermediateSums[8] __attribute__((aligned(32)));

    _mm256_store_ps(intermediateSums, vU);

    for (uint16_t i = 0; i < 8; i++)
    {
        result += intermediateSums[i];
    }

    return result;
}

//Multithreaded implementation

// * Thread routine for scalars
void *thread_function(void *threadarg)
{
    // local variables
    float s = 0;
    uint64_t i;

    // Association  between shared variables and their correspondances
    thread_data *thread_pointer_data;
    thread_pointer_data = (thread_data *)threadarg;
    // body of the thread
    for (i = thread_pointer_data->deb; i < thread_pointer_data->fin; i++)
    {
        s += sqrt(fabsf(thread_pointer_data->U[i]));
    }
    thread_pointer_data->res = s;
    pthread_exit(NULL);
    return 0;
}

// * Thread routine for vectors
void *thread_function_vect(void *threadarg)
{
    // local variables
    float s = 0;
    uint64_t i;

    // Association  between shared variables and their correspondances
    thread_data *thread_pointer_data;
    thread_pointer_data = (thread_data *)threadarg;
    // body of the thread
    /*
    Here we're just re-using the previously implemented vect_norm by taking the starting address of the input
    vector (here U + deb) and specifying the overall length to use (fin-debut+1)
    */
    float *localInput = thread_pointer_data->U;
    s = vect_norm(localInput + thread_pointer_data->deb, (thread_pointer_data->fin - thread_pointer_data->deb + 1));

    thread_pointer_data->res = s;
    pthread_exit(NULL);
    return 0;
}

// * Wrapping function for multithreaded & hybrid norm
float multi_thr_norm(float *U, int n, int nb_threads, int mode)
{

    uint32_t sliceLength = n / nb_threads;

    pthread_t *thread_ptr = (pthread_t *)malloc(nb_threads * sizeof(pthread_t));

    thread_data *th_array = (thread_data *)malloc(nb_threads * sizeof(thread_data));

    //Setting pthread attributes for the threads to spawn
    pthread_attr_t attr;

    pthread_attr_init(&attr);

    //Setting stacksize
    size_t mystacksize = (n + 100) * sizeof(float);

    pthread_attr_setstacksize(&attr, mystacksize);

    float s = 0;
    for (uint16_t i = 0; i < nb_threads; i++)
    {
        //Filling thread's associated structure with appropriate data
        th_array[i].id = i;
        th_array[i].U = U;
        th_array[i].deb = i * sliceLength;
        th_array[i].fin = (i + 1) * sliceLength;

        // create and launch the thread
        if (mode)
        {
            pthread_create(&thread_ptr[i], &attr, thread_function_vect, (void *)&th_array[i]);
        }
        else
        {
            pthread_create(&thread_ptr[i], &attr, thread_function, (void *)&th_array[i]);
        }
    }

    // Joining threads (Barrier Synchronization)
    for (uint16_t i = 0; i < nb_threads; i++)
    {
        pthread_join(thread_ptr[i], NULL);
    }

    // Summing the intermediate results
    for (uint16_t i = 0; i < nb_threads; i++)
    {
        s += th_array[i].res;
    }

    // Destroying attribute object & freeing allocated memory
    pthread_attr_destroy(&attr);
    free(thread_ptr);
    free(th_array);

    return s;
}