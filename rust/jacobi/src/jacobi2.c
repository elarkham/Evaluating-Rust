/*
 * This serial program implements the Jacobi algorithm to find the
 * steady-state temperature distribution on an insulated
 * two-dimensional plate, given constant boundary conditions.
 *
 * This version was created in March 2017, to compare to a Go version.
 * The big change was to use double type. Also made the "change" variable
 * to be a scalar rather than an array.
 */

#include <math.h>
#include <stdio.h>

#define SIZE    1280
#define TEMP    50.0
#define EPSILON 0.1

double new[SIZE][SIZE];
double old[SIZE][SIZE];

int main ()
{
  double maxerr;
  double change; 
  int   cool_cells;
  int   i,j;

     for (i = 0; i < SIZE; i++)
     {                             /* north, east, and west boundaries */
         old[0][i] = new[0][i] = 0.0;
         old[i][0] = new[i][0] = 0.0;
         old[i][SIZE-1] = new[i][SIZE-1] = 0.0;
     }
     for (i = 1; i < SIZE-1; i++)
     {                            /* south boundary */
         old[SIZE-1][i] = new[SIZE-1][i] = 100.0;
     }
     for (i = 1; i < SIZE-1; i++) /* inner core */
        for (j = 1; j < SIZE-1; j++)
           new[i][j] = 50.0;

     do {               /* Compute steady-state temperatures */
        maxerr = 0.0;
        for (i = 1; i < SIZE-1; i++)
           for (j = 1; j < SIZE-1; j++)
           {
              old[i][j] = new[i][j];
           }
        for (i = 1; i < SIZE-1; i++)
           for (j = 1; j < SIZE-1; j++)
           {
              new[i][j] = (old[i-1][j] + old[i+1][j] +
                     old[i][j+1] + old[i][j-1]) / 4.0;
              change = fabs(old[i][j]-new[i][j]);
              if (maxerr < change) maxerr =  change; 
           }
     } while (maxerr > EPSILON);

     cool_cells = 0;
     for (i = 0; i < SIZE; i++)
        for (j = 0; j < SIZE; j++)
           if (new[i][j] < TEMP) cool_cells++;

     printf ("There are %d cells cooler than %5.2f degrees\n",
        cool_cells, TEMP);

     return 0;
}
