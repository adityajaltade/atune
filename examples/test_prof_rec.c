#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include "profiler.h"

#define NO_RUNS 100000

#if PROF==1
#define PROFILING
#endif

void random_func()
{
#ifdef PROFILING
    BEGIN_PROFILING(random);
#endif
    char *temp;
    temp = (char*) malloc (10000);
    free(temp);
}

int rec_func(int i)
{
#ifdef PROFILING
//    BEGIN_PROFILING(recursive);
#endif
    char *temp;
    if(i>0)
    {
        temp = (char*) malloc(10000);
        rec_func(--i);
        free (temp);
    }
}

int main()
{
    int i=0;
    char *temp;
#ifdef PROFILING
    BEGIN_PROFILING(main);
#endif
    for(;i<NO_RUNS;++i)
    {
        random_func();
    }
    BEGIN_PROFILING(main2);
    rec_func(100000);
    return 0;
}
