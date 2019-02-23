#include "global.h"

print_report(unit1, unit2)
{
    int ns,nl,nx;
    char num[30];
    int len, new;
    long *cats, *prev;
    double total_area;
    int first;
    int i;
    int need_format;
    int with_stats;
    char *cp;
    char *print_label();
    int dots;

/* examine units, determine output format */
    for (i = unit1; i <= unit2; i++)
    {
	need_format = 1;
	unit[i].label[0] = "";
	unit[i].label[1] = "";

	switch (unit[i].type)
	{
	case CELL_COUNTS:
	    need_format = 0;
	    unit[i].len = 5;
	    unit[i].label[0] = " cell";
	    unit[i].label[1] = "count";
	    for (ns = 0; ns < nstats; ns++)
	    {
		sprintf (num, "%ld", Gstats[ns].count);
		len = strlen (num);
		if (len > unit[i].len)
		    unit[i].len = len;
	    }
	    break;

	case PERCENT_COVER:
	    need_format = 0;
	    unit[i].dp = 2;
	    unit[i].len = 5;
	    unit[i].label[0] = "  %  ";
	    unit[i].label[1] = "cover";

	    total_area = 0.0; /* compute total area */
	    for (ns = 0; ns < nstats; ns++)
		total_area += Gstats[ns].area;
	    unit[i].factor = 100.0 / total_area;

	    break;

	case SQ_METERS:
	    unit[i].label[0] = "square";
	    unit[i].label[1] = "meters";
	    unit[i].factor   = 1.0;
	    break;

	case SQ_KILOMETERS:
	    unit[i].label[0] = "  square  ";
	    unit[i].label[1] = "kilometers";
	    unit[i].factor   = 1.0e-6;
	    break;

	case ACRES:
	    unit[i].label[0] = "";
	    unit[i].label[1] = "acres";
	    unit[i].factor   = 2.471e-4;
	    break;

	case HECTARES:
	    unit[i].label[0] = "";
	    unit[i].label[1] = "hectares";
	    unit[i].factor   = 1.0e-4;
	    break;

	case SQ_MILES:
	    unit[i].label[0] = "square";
	    unit[i].label[1] = " miles";
	    unit[i].factor   = 3.861e-7;
	    break;

	default:
	    printf ("Unit %d not yet supported\n", unit[i].type);
	    exit(1);
	}
	if (need_format)
	{
	    unit[i].dp = 6;
	    unit[i].len = 10;
	    unit[i].eformat = 0;
	    for (ns = 0; ns < nstats; ns++)
		format_parms (Gstats[ns].area*unit[i].factor,
		    unit[i].len, &unit[i].eformat, &unit[i].dp);
	}
    }

/* figure out how big the category numbers are when printed */
    for (nl = 0; nl < nlayers; nl++)
	layers[nl].nlen = 0;

    for (ns = 0; ns < nstats; ns++)
    {
	cats = Gstats[ns].cats;
	for (nl = 0; nl < nlayers; nl++)
	{
	    sprintf(num, "%ld", cats[nl]);
	    len = strlen (num);
	    if (len > layers[nl].nlen)
		layers[nl].nlen = len;
	}
    }

/* compute maximum category description lengths */
    len = width-2;
    for (i = unit1; i <= unit2; i++)
	len -= (unit[i].len + 1);
    for (nl = 0; nl < nlayers; nl++)
    {
	len -= (layers[nl].nlen + 1);
	layers[nl].clen = len;
    }

/* print the report */

    header(unit1, unit2);

    new = 1;
    for (ns = 0; ns < nstats; ns++)
    {
	cats  = Gstats[ns].cats;

/* determine the number of lines needed to print the cat labels 
 * by pretending to print the labels and coauting the number of
 * print calls needed
 */

	if (length > 0)
	{
	    i = 0;
	    for (nl = 0; nl < nlayers; nl++)
	    {
		cp = G_get_cat ((CELL) cats[nl], &layers[nl].labels);
		while (cp)
		{
		    i++;
		    cp = print_label (cp, layers[nl].clen, 0, 0);
		}
	    }
    /* if we don't have enough lines, go to a new page */
	    if (nlines <= i+2)
	    {
		trailer();
		header(unit1, unit2);
		new = 1;
	    }
	}

/* print the report */
	for (nl = 0; nl < nlayers; nl++)
	{
	    if (new || (prev[nl] != cats[nl]))
	    {
		first = 1;
		new = 1;
		cp = G_get_cat ((CELL) cats[nl], &layers[nl].labels);
		while(cp)
		{
		    for (nx = 0 ; nx < nl ; nx++)
			printf ("|%*s", layers[nx].nlen, "");
		    if (first)
			printf ("|%*d|", layers[nl].nlen, cats[nl]);
		    else
			printf ("|%*s|", layers[nl].nlen, "");
		    with_stats = nunits && first && nl == nlayers - 1;
		    cp = print_label (cp, layers[nl].clen, 1, with_stats?2:0);
		    if (with_stats)
		    {
			for (i = unit1; i <= unit2; i++)
				print_unit(i,ns,nl);
		    }
		    else
		    {
			for (i = unit1; i <= unit2; i++)
			    printf ("|%*s", unit[i].len, "");
		    }
		    printf ("|");
		    newline();
		    first = 0;
		}
	    }
	}
	new = 0;
	prev = cats;
    }
    trailer();
}
