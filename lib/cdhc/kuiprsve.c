#include<stdio.h>
#include<math.h>

double *kuipers_v_exp (x, n)
  double *x;
  int n;
{
  static double y[2];
  double *d,r, *dmax_exp();

  d = dmax_exp (x, n);
  r = sqrt ((double) n);
  y[1] = d[0] + d[1];
  y[0] = (y[1] - 0.2 / n) * (r + 0.35 / r + 0.24);
#ifdef NOISY
  fprintf (stdout,"  TEST18 KV(E)  =%10.4f\n", y[0]);
#endif				/* NOISY */
  return y;
}
