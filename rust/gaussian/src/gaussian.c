/* This C program completely solves a system of linear equations using
   (1) gaussian elimination with partial pivoting and (2) back substitution.
   (90.08.27) (modified by pjh 95.10.01 for cs780) */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define N       512
#define EPSILON 1.0e-20

double a[N][N+1];
int    pivot[N];
int    marked[N];

/* cook up a system where ans[i] will be i */
void initialize_system()
{
   int i, j, seed;

   srand(N);
   for (i = 0; i < N; i++) {
      marked[i] = 0;
      seed = i*i;
      a[i][N] = 0.0;
      for (j = 0; j < N; j++) {
         a[i][j] = rand();
         a[i][N] += j * a[i][j];
      }
   }
}

/* reduce the matrix using partial pivoting */
bool gaussian_elimination()
{
   int    i, j, k, picked;
   double tmp;

   for (i = 0; i < N - 1 ; i++) {
      tmp = 0.0;
      for (j = 0; j < N; j++) {
         if (!marked[j] && (fabs(a[j][i]) > tmp)) {
            tmp = fabs(a[j][i]);
            picked = j;
         }
      }
      marked[picked] = 1;    /* Mark pivot row */
      pivot[picked] = i;     /* Remember permuted position */

      if (fabs(a[picked][i]) < EPSILON) {
         printf ("Exits on iteration %d\n", i);
         return false;
      }

      for (j = 0; j < N; j++)
         if (!marked[j]) {
            tmp = a[j][i] / a[picked][i];
            for (k = i; k < N+1; k++)
               a[j][k] = a[j][k] - a[picked][k] * tmp;
         }
   }
   for (i = 0; i < N; i++)
      if (!marked[i]) pivot[i] = N-1;
   return true;
}

/* solve the (logical) upper triangular matrix */
void back_substitution()
{
   double coeff;
   int    i, j;

   for (i = N-1; i >= 0; i--) {
      for (j = 0; pivot[j] != i; j++) ;
      coeff = a[j][N] / a[j][i];
      for (j = 0; j < N; j++)
         if (pivot[j] < i)
            a[j][N] -= coeff * a[j][i];
   }
}

/* ans[i] should be (roughly) i */
/* to get ans I still need to divide by the non-zero column's coefficient */
/* might end up with -0.0 so I must special-case that */
void check_results()
{
   int i;

   for (i = 0; i < N; i++)
   {
      char buf1[100], buf2[100];
      double ans;

      ans = a[i][N]/a[i][pivot[i]];
      if ((pivot[i] == 0) && (ans < 0.0)) ans = -ans;
      sprintf(buf1, "%10.6f", (double) pivot[i]);
      sprintf(buf2, "%10.6f", ans);
      /* printf("%s == %s?\n", buf1, buf2); */
      assert (strcmp(buf1, buf2) == 0);
   }

   printf("Done.\n");
}

int main()
{
   int solution;

   initialize_system();
   solution = gaussian_elimination();
   if (solution) {
      back_substitution();
      check_results();
   } else printf ("No solution?\n");
   return 0;
}
