/*
*    functions - calc_begin_angle(), and calc_end_angle()  
*    used to calculate the angle of a line to a node.
*    returns -  (float)angle	(-PI ... +PI)
*    returns -  (float)(-9)  if only 1 point
*/

#include <stdio.h>
#include "dig_structs.h"
#include    <math.h>

static double d_atan2 ();

float
dig_calc_begin_angle (points, thresh)
    struct line_pnts *points;
    double thresh;
{
    double last_x;
    double last_y;
    double *xptr;
    double *yptr;
    double fabs();
    double atan2();
    int short_line;
    int i;
    int n_points;
    double *xarray;
    double *yarray;

    /* temporary way to set up use of struct line_pnts */
    n_points = points->n_points;
    xarray = points->x;
    yarray = points->y;

    last_x = *xarray;
    last_y = *yarray;
    xptr = xarray + 1;
    yptr = yarray + 1;

    if (n_points == 1)
	return ((float) -9.);
/*DEBUG fprintf (stderr, "Thresh = %lf\n", thresh); */

    short_line = 1;
    for (i=1; i < n_points; i++) /* Search for next different coord */
    {
	if ( (thresh < fabs(*xptr - last_x) ) ||
	     (thresh < fabs(*yptr - last_y) ) )
	{
	    short_line = 0;
	    break;
	}
	xptr++;  yptr++;
    }

    /* if entire line is w/in threshold, get angle from end points */
    if (short_line)
    {
	return ((float) d_atan2 (yarray[n_points-1]-last_y, xarray[n_points-1]-last_x));
    }

    return ((float) d_atan2 (*yptr-last_y, *xptr-last_x));

}    /*  calc_begin_angle()  */



float
dig_calc_end_angle(points, thresh)
    struct line_pnts *points;
    double thresh;
{
    double last_x;
    double last_y;
    double *xptr;
    double *yptr;
    double fabs();
    double atan2();
    int short_line;
    int i;
    int n_points;
    double *xarray;
    double *yarray;


    short_line = 1;

    xarray = points->x;
    yarray = points->y;
    n_points = points->n_points;

    if (n_points == 1)
	return ((float) -9.);

    last_x = *(xarray + n_points - 1);
    last_y = *(yarray + n_points - 1);
    xptr = xarray + n_points - 2;
    yptr = yarray + n_points - 2;
    for(i=n_points-2; i>=0; i--)
    {
	if ( (thresh < fabs(*xptr - last_x) ) ||
	     (thresh < fabs(*yptr - last_y) ) )
	{
	    short_line = 0;
	    break;
	}
	xptr--; yptr--;
    }

    /* if entire line is w/in threshold, get angle from end points */
    if (short_line)
	return ((float) d_atan2 (yarray[0]-last_y, xarray[0]-last_x));

    return( (float)d_atan2(*yptr-last_y, *xptr-last_x));
}

dig_is_line_degenerate (points, thresh)
    struct line_pnts *points;
    double thresh;
{
    double last_x;
    double last_y;
    double *xptr;
    double *yptr;
    double fabs();
    int short_line;
    int i;
    int n_points;
    double *xarray;
    double *yarray;

    /* temporary way to set up use of struct line_pnts */
    n_points = points->n_points;
    xarray = points->x;
    yarray = points->y;

    last_x = *xarray;
    last_y = *yarray;
    xptr = xarray + 1;
    yptr = yarray + 1;

    short_line = 1;
    for (i=1; i < n_points; i++) /* Search for next different coord */
    {
	if ( (thresh < fabs(*xptr - last_x) ) ||
	     (thresh < fabs(*yptr - last_y) ) )
	{
	    short_line = 0;
	    break;
	}
	xptr++;  yptr++;
    }

    if (short_line)
	return(1);

    return (0);

}

static double
d_atan2 (y, x)
    double y, x;
{
    if (y == 0.0 && x == 0.0)
	return (0.0);
    else
	return (atan2 (y, x));
}
