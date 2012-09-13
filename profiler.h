//Base idea from: http://locklessinc.com/articles/profiling/

/*
  Author: Aditya Jaltade
  Created: 18 Feb 2012
  Last Modified: Sun, 26 February 2012 02:17:02 IST
  Notes: Use gcc4.1+
*/

// This library prints out the time in terms of tsc ticks...
// Time spent = (tsc ticks) * 1000 / (CPU freq in MHz)

#ifndef __PROFILER_H__
#define __PROFILER_H__

#if PROF==1
#define PROFILE
#endif

#ifdef PROFILE
#define ARG1  readtsc()
#else
#define ARG1  0
#endif

#define BEGIN_PROFILING(PROBE_NAME) struct profiler_data PROBE_NAME##func_prof = { ARG1 , #PROBE_NAME , __LINE__};

#define END_PROFILING(PROBE_NAME) profiler_func_exit(&PROBE_NAME##func_prof);

#ifdef __x86_64__
static __inline__ unsigned long long readtsc(void)
{
    unsigned hi, lo;
    __asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((unsigned long long) lo) | (((unsigned long long) hi) << 32);
}

#else

static __inline__ unsigned long long readtsc(void)
{
    unsigned long long int x;
    __asm__ volatile ("rdtsc" : "=A" (x));
    return x;
}
#endif

#define MAX_PROF_LINE 65535

struct profiler_data
{
    unsigned long long time;
    const char *func_name;
    unsigned line;
};

struct profiler_entry
{
    unsigned long long time;
    const char *name;
    int func_count;
    unsigned long long min;
    unsigned long long max;
    //can add min, max and median time as well here.
};

/*
  Global array holding all profiler data
*/
static struct profiler_entry profiler[MAX_PROF_LINE] = {{0, NULL, 0, 0, 0}};

/*
  This function is called, when the function to be profiled exits.
  This is specified using ((cleanup))
*/
static void profiler_func_exit(struct profiler_data *pd)
{

#ifndef PROFILE
    return ;
#endif

    unsigned long long x = readtsc();

    unsigned long long func_time = x - pd->time;

// TODO - something better than this here... Line cannot be trusted!
    struct profiler_entry *pe = &profiler[pd->line];
    if (func_time < pe->min)
        pe->min = func_time;
    if(func_time > pe->max)
        pe->max = func_time;
    if(pe->func_count == 0)
        pe->min = pe->max = func_time;
    /* Compile-time test for problems */
    //if (pd->line >= MAX_PROF_LINE) exit(0);

    __sync_add_and_fetch(&pe->func_count, 1);

    /* Save name for later */
    pe->name = pd->func_name;


    /* Collate time */
    __sync_add_and_fetch(&pe->time, x - pd->time);

}

/*
  Comparator for qsort
*/
static int profiler_entry_comparator(const void *p1, const void *p2)
{
    const struct profiler_entry *pt1 = p1;
    const struct profiler_entry *pt2 = p2;

    if (pt1->time > pt2->time) return -1;
    return (pt1->time < pt2->time);
}

/*
  This function gets called, *after* main exits.
  gcc specific.
*/
static __attribute__((destructor)) void profiler_print_data(void)
{

#ifndef PROFILE
    return ;
#endif

    int i, j = 0;

    unsigned long long total = 0;

    /* Compact entries */
    for (i = 0; i < MAX_PROF_LINE; i++)
    {
        if (!profiler[i].name) continue;

        profiler[j] = profiler[i];
        j++;
    }

    /* Sort them */
    qsort(profiler, j, sizeof(struct profiler_entry), profiler_entry_comparator);

    printf("Profile results:\n");
    printf("Probe name\tAvg Ticks\tCount\tMin Ticks\tMax Ticks\n");
    for (i = 0; i < j; i++)
    {
        printf("%s:\t\t%llu\t%d\t%llu\t%llu\n", profiler[i].name, profiler[i].time ,profiler[i].func_count, profiler[i].min , profiler[i].max );
    }
}
#endif
