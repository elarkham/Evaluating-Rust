// pi4.c (Uses Posix threads on Linux, Sept 12)
// child threads put partial sum in an array
// thread_join used to have main thread wait
// main thread sums values in array to get final sum

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

// how many intervals to divide [0,1] into
#define INTERVALS 50000000

// width of an interval
#define WIDTH (1.0/INTERVALS)

// the maximum number of child threads that can be used
#define MAX_T 24

// array for storing thread IDs
pthread_t pt[MAX_T];

// child threads drop values into the array
double partialSums[MAX_T];

// number of intervals per thread
int chunk;

// processor IDs less than split have one extra interval
int split;

// prints usage message if command line arguments are not what is expected
// program expects a single argument: the number of child threads to create
void usage(char *);

// prints an error message and terminates the program
void error(char *);

// this is the work executed by each child thread
void * work(void *);

int main (int argc, char *argv[])
{
   long i;
   int n; // number of child threads to create */

   if (argc !=2) usage("pi numofthreads\n");

   n = atoi(argv[1]);

   if (n <= 0) usage("number of threads must be > 0");

   if (n > MAX_T) usage("too many threads requested");

   printf("%d (child) threads used\n", n);

   chunk = INTERVALS / n;
   split = INTERVALS % n;
   if (split == 0)
   {
      split = n;
      chunk -= 1;
   }

   // create n threads
   for (i=0; i < n; i++)
   {
      /* create threads; DANGER: thread logical id (int) passed as "void *" */
      if (pthread_create(&pt[i], NULL, work, (void *) i) != 0)
         error("error in thread create");
   }

   double sum = 0.0;

   // wait for each thread to finish
   for (i=0; i < n; i++)
   {
     if (pthread_join(pt[i], NULL))
     {
       error("error in thread join");
     }
     sum += partialSums[i];
   }
   sum *= 1.0/INTERVALS;

   printf ("Estimation of pi is %14.12f\n", sum);

   return 0;
}

void * work(void * in)
{
   int i;
   int low;                 // first interval to be processed
   int high;                // first interval *not* to be processed
   double localSum = 0.0;   // sum for intervals being processed
   double x;                // mid-point of an interval
   long id = (long) in;     // logical thread id (0..n-1)

   if (id < split)
   {
      low = (id * (chunk + 1));
      high = low + (chunk + 1);
   }
   else
   {
      low = (split * (chunk + 1)) + ((id - split) * chunk);
      high = low + chunk;
   }

   x = (low+0.5)*WIDTH;
   for (i = low; i < high; i++)
   {
      localSum += (4.0/(1.0+x*x));
      x += WIDTH;
   }

   partialSums[id] = localSum;

   return 0;
}

void error(char *str)
{
    perror(str);
    exit(-1);
}

void usage(char *str)
{
   fprintf(stderr, "usage: %s\n",str);
   exit(-1);
}

