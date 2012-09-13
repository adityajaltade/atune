aprof
=====

A simple profiler in C which gives the output as the number of ticks between any
two arbitrary points in code.

Usage:
------------
1. Include "profiler.h" in your code.
2. You need to add probe points within your code to enable profiling for that
portion of the code.
You can specify a start point using - BEGIN_PROFILING(Probe1);
and an endpoint using          - END_PROFILING(Probe1);
    eg :
  BEGIN_PROFILING(malloc10000);
  char *temp = (char *) malloc (10000*sizeof(char));
  free (temp);
  END_PROFILING(malloc10000);
The above would count the number ticks of the processor tsc that elapse by
malloc'ing a block of 10000 chars and then free'ing them.
3. Compile the program with the PROF defined.
    eg: gcc file1.c -o file1 -DPROF=1
4. Run your program to get the output.

Limitations:
------------
1. May not work well with recursive calls.
2. Single threaded.

TODO:
------------
1. Support recursive function calls. (Already underway)
2. Support multi-threading.
2. Formatting output
3. There seems to be some code duplication in the way the macros are defined,
cleanup is required.
4. Output is available only at the end of the program execution and is printed
on the console. This will surely mess up the expected outputs of piped
programs. Need to provide a way to gather this data in a third location and make
available on demand.
