/* plot1() - Level One vector reading 
*
* Michael Shapiro,
* U.S. Army Construction Engineering
* Research Laboratory
*
*/

#include "gis.h"
#include "Vect.h"
#include "display.h"

extern int quiet;

int plot1 (
    char *name,char *mapset,
    struct line_pnts *Points)
{
    int i;
    struct Map_info Map;
    double *x, *y;

    Vect_set_open_level (1);
    if (1 > Vect_open_old (&Map, name, mapset))
	G_fatal_error ("Failed opening vector file");

    G_setup_plot (
	D_get_d_north(), D_get_d_south(), D_get_d_west(), D_get_d_east(),
	D_move_abs, D_cont_abs);

	if (!quiet)
			fprintf (stdout,"Plotting ... "); fflush (stdout);
    while (1)
    {
        switch (Vect_read_next_line (&Map, Points))
	{
	case -1:
	    Vect_close (&Map);
	    fprintf (stderr, "\nERROR: vector file [%s] - can't read\n", name);
	    return -1;
	case -2: /* EOF */
		if (!quiet)
			fprintf (stdout,"Done\n");
	    Vect_close (&Map);
	    return  0;
	}

	x = Points->x;
	y = Points->y;

	for(i=1; i < Points->n_points; i++)
	{
	    G_plot_line(x[0], y[0], x[1], y[1]);
	    x++;
	    y++;
	}
    }

    return 0;
}
