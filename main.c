#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <malloc.h>
#include "immintrin.h"
#include "stdint.h"
#include "include/norm.h"
#include "include/utils.h"
#include <unistd.h>

int main(int argc, char **argv)
{
    int numberOfProcessors = sysconf(_SC_NPROCESSORS_ONLN);

    //Number of shifts of 1 for input size (length = 2^nbShifts)
    int nbShifts = 23;
    int nbThreads = numberOfProcessors;

    if (argc > 1)
    {
        sscanf(argv[1], "%d", &nbShifts); // 1st Arg : Size of data

        if (argc == 3)
        {
            sscanf(argv[2], "%d", &nbThreads); // 2nd Arg : Number of threads
        }
    }

    printf("Number Of Processors is = %d \n", numberOfProcessors);

    printf("Length of data is 2^%d \n", nbShifts);

    float resultSeq = 0;
    float resultVec = 0;
    float resultThr = 0;
    float resultHybr = 0;

    double startSeq, endSeq;
    double startVec, endVec;
    double startThr, endThr;
    double startHybrid, endHybrid;

    /* Intializes random number generator */
    time_t t;
    srand((unsigned)time(&t));

    /* Initializing input vector */
    uint64_t n = 1 << nbShifts;
    float sum = 0;

    //Aligned Vs Not Aligned : Here both will work (no SEG error) because we made sure we have an aligned
    //load of register vectors in our vect_norm function.

    // float *input = (float *)_mm_malloc(n * sizeof(float), 32);
    float *input = (float *)malloc(n * sizeof(float));

    for (size_t i = 0; i < n; i++)
    {
        input[i] = (float)(2 * (rand() % 2) - 1) * (float)(rand() % 10) / ((float)(100 + rand() % 100) + 1);
    }

    /* Base Sequential Implementation of norm */

    startSeq = now();
    resultSeq = norm(input, n);
    endSeq = now();

    /* Vectorized Implementation of norm */
    startVec = now();
    resultVec = vect_norm(input, n);
    endVec = now();

    /* multithreaded Implementation of norm */
    startThr = now();
    resultThr = multi_thr_norm(input, n, nbThreads, 0);
    endThr = now();

    /* multithreaded & Vectorized Implementation of norm */
    startHybrid = now();
    resultHybr = multi_thr_norm(input, n, nbThreads, 1);
    endHybrid = now();

    /* BENCHMARKING & COMPARING PERFORMANCES */

    printf("\n-----COMPARING ACCURACIES-----\n\n");

    printf("Result of sequential norm is = %f\n", resultSeq);
    printf("Result of vectorized norm (8 single precision floats / vector register) is = %f \n", resultVec);
    printf("Result of multithreaded norm is = %f \n", resultThr);
    printf("Result of hybrid norm is = %f \n", resultHybr);

    printf("\n-----COMPARING PERFORMANCES-----\n\n");

    printf("Duration of the sequential execution : %lf (s) \n", endSeq - startSeq);

    printf("Acceleration rate sequential / vectorized (8 single precision floats vector registers) = %f \n",
           (startSeq - endSeq) / (startVec - endVec));
    printf("Acceleration rate sequential / multithreaded (%d threads) = %f \n",
           nbThreads,
           (startSeq - endSeq) / (startThr - endThr));
    printf("Acceleration rate sequential / hybrid (%d threads and 8 single precision floats vector registers) = %f \n",
           nbThreads,
           (startSeq - endSeq) / (startHybrid - endHybrid));

    // _mm_free(input);
    free(input);

    return 0;
}