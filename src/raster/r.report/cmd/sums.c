#include "global.h"

/* within group totals:
    *ns is the first stat
	(updated upon return to point to next stat)
     nl is the layer number (or level)
*/

double
area_sum (ns,nl)
    int *ns;
{
    double area;
    int k,n;

    k = n = *ns;
    area = 0.0;

    if(nl >= 0)
    {
	while(n < nstats && same_cats(k,n,nl))
	    area += Gstats[n++].area;
    }
    else
    {
	while(n < nstats)
	    area += Gstats[n++].area;
    }

    *ns = n;
    return area;
}

long
count_sum (ns,nl)
    int *ns;
{
    long count;
    int k,n;

    k = n = *ns;
    count = 0;

    if (nl >= 0)
    {
	while(n < nstats && same_cats(k,n,nl))
	    count += Gstats[n++].count;
    }
    else
    {
	while(n < nstats)
	    count += Gstats[n++].count;
    }

    *ns = n;
    return count;
}

same_cats (a,b,nl)
{
    long *cat_a,*cat_b;

    cat_a = Gstats[a].cats;
    cat_b = Gstats[b].cats;

    while (nl-- >= 0)
	if (*cat_a++ != *cat_b++)
	    return 0;
    return 1;
}
