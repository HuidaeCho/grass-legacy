#include<stdio.h>
#include<stdlib.h>
#include<math.h>

/* could probably use some cleanup/optimization */

double *durbins_exact (x, n)
  double *x;
  int n;
{
  static double y[2];
  double *xcopy, sumx = 0.0, sumx2 = 0.0, s2, *b, *c, *g, *z, sqrt2;
  double normp();
  int i, j, dcmp();

  if ((b = (double *) malloc (n * sizeof (double))) == NULL)
    fprintf (stderr, "Memory error in durbins_exact\n"), exit (-1);
  if ((c = (double *) malloc ((n + 1) * sizeof (double))) == NULL)
    fprintf (stderr, "Memory error in durbins_exact\n"), exit (-1);
  if ((g = (double *) malloc ((n + 1) * sizeof (double))) == NULL)
    fprintf (stderr, "Memory error in durbins_exact\n"), exit (-1);
  if ((z = (double *) malloc (n * sizeof (double))) == NULL)
    fprintf (stderr, "Memory error in durbins_exact\n"), exit (-1);
  if ((xcopy = (double *) malloc (n * sizeof (double))) == NULL)
    fprintf (stderr, "Memory error in durbins_exact\n"), exit (-1);

  sqrt2 = sqrt ((double) 2.0);
  for (i = 0; i < n; ++i)
  {
    xcopy[i] = x[i];
    sumx += x[i];
    sumx2 += x[i] * x[i];
  }
  s2 = sqrt ((sumx2 - sumx * sumx / n) / (n - 1));
  for (i = 0; i < n; ++i)
  {
    xcopy[i] = (xcopy[i] - sumx / n) / s2;
    b[i] = 0.5 + normp (xcopy[i] / sqrt2) / 2.0;
  }
  qsort (b, n, sizeof (double), dcmp);
  for (i = 1; i < n; ++i)
    c[i] = b[i] - b[i - 1];
  c[0] = b[0];
  c[n] = 1 - b[n - 1];
  qsort (c, n + 1, sizeof (double), dcmp);
  for (j = 1; j <= n; ++j)
    g[j] = (n + 1 - j) * (c[j] - c[j - 1]);
  g[0] = (n + 1) * c[0];
  g[n] = c[n] - c[n - 1];

  for (i = 0; i < n; ++i)
  {
    z[i] = 0.0;
    for (j = 0; j <= i; ++j)
      z[i] += g[j];
    z[i] = (i + 1.0) / n - z[i];
  }

  qsort (z, n, sizeof (double), dcmp);
  y[0] = z[n - 1];
  y[1] = sqrt((double)n)*z[n - 1];

#ifdef NOISY
  fprintf (stdout,"  TEST7  DRB(N) =%10.4f\n", y[0]);
#endif				/* NOISY */
  free (b);
  free (c);
  free (g);
  free (xcopy);
  free (z);
  return y;
}
