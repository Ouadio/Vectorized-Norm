#include <stdlib.h>
#include "utils.h"
#include <time.h>
#include "sys/time.h"

double now()
{
    struct timeval t;
    double f_t;
    gettimeofday(&t, NULL);
    f_t = t.tv_usec;
    f_t = f_t / ((double)1E6); //Seconds
    f_t += t.tv_sec;
    return (f_t);
}