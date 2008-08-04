#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <grass/gis.h>
#include <grass/glocale.h>
#include "method.h"

#define MEM  1024

/* function prototypes */
static int m_var(double *, int, double *);


int
o_var(char *basemap, char *covermap, char *outputmap, int usecats,
      struct Categories *cats)
{
    char command[1024];
    FILE *stats, *reclass;
    int first, mem, i, count;
    long basecat, covercat, catb, catc;
    double value, vari, x;
    double *tab;


    mem = MEM * sizeof(double);
    tab = (double *)G_malloc(mem);

    sprintf(command, "r.stats -cn input=\"%s,%s\" fs=space", basemap,
	    covermap);
    stats = popen(command, "r");

    sprintf(command, "r.reclass i=\"%s\" o=\"%s\"", basemap, outputmap);
    reclass = popen(command, "w");


    first = 1;
    while (read_stats(stats, &basecat, &covercat, &value)) {
	if (first) {
	    first = 0;
	    catb = basecat;
	    catc = covercat;
	    i = 0;
	    count = 0;
	}

	if (basecat != catb) {
	    m_var(tab, count, &vari);
	    fprintf(reclass, "%ld = %ld %f\n", catb, catb, vari);
	    /*fprintf (stdout, "1. %ld = %ld %f\n", catb, catb, vari); */
	    catb = basecat;
	    catc = covercat;
	    count = 0;
	}

	if (usecats)
	    sscanf(G_get_cat((CELL) covercat, cats), "%lf", &x);
	else
	    x = covercat;

	for (i = 0; i < value; i++) {
	    if (count * sizeof(double) >= mem) {
		mem += MEM * sizeof(double);
		tab = (double *)G_realloc(tab, mem);
		/* fprintf(stderr,"MALLOC: %d KB needed\n",(int)(mem/1024)); */
	    }
	    tab[count++] = x;
	}

    }

    if (first) {
	catb = catc = 0;
    }

    m_var(tab, count, &vari);
    fprintf(reclass, "%ld = %ld %f\n", catb, catb, vari);
    G_debug(5, "2. %ld = %ld %f", catb, catb, vari);

    pclose(stats);
    pclose(reclass);

    return (0);
}


/***********************************************************************
*
*  Given an array of data[1...n], this routine returns its variance.
*
************************************************************************/

static int m_var(double *data, int n, double *vari)
{
    double ave, ep, s;
    int i;

    if (n < 1) {
	G_warning(_("o_var: No data in array"));
	return (1);
    }

    *vari = 0.0;
    ep = 0;
    s = 0.0;


    for (i = 0; i < n; i++)	/* First pass to get the mean     */
	s += data[i];
    ave = s / n;

    for (i = 0; i < n; i++) {
	s = data[i] - ave;
	/*fprintf(stderr,"s: %lf  data[i]: %lf  ave: %lf  n: %d\n",s,data[i],ave,n);  */
	*vari += s * s;
	ep += s;
    }

    *vari = (*vari - ep * ep / n) / (n - 1);

    return (0);
}
