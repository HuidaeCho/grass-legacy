/******************************************************************
 * Plot lines and filled polygons. Input space is database window.
 * Output space and output functions are user defined.
 * Converts input east,north lines and polygons to output x,y
 * and calls user supplied line drawing routines to do the plotting.
 *
 * Handles global wrap-around for lat-lon databases.
 *
 * Does not perform window clipping.
 * Clipping must be done by the line draw routines supplied by the user.
 *
 * Note:
 *  Hopefully, cartographic style projection plotting will be added later.
 *******************************************************************/
#include "gis.h"

static double xconv, yconv;
static double left, top;
static struct Cell_head window;
static int (*move)() = NULL;
static int (*cont)() = NULL;

static double fabs(x)
    double x;
{
    return x>0?x:-x;
}

extern double G_adjust_easting();

/*
 * G_setup_plot (t, b, l, r, Move, Cont)
 *     double t, b, l, r;
 *     int (*Move)(), (*Cont)();
 *
 * initialize the plotting capability.
 *    t,b,l,r:   top, bottom, left, right of the output x,y coordinate space.
 *    Move,Cont: subroutines that will draw lines in x,y space.
 *       Move(x,y)   move to x,y (no draw)
 *       Cont(x,y)   draw from previous position to x,y
 * Notes:
 *   Cont() is responsible for clipping.
 *   The t,b,l,r are only used to compute coordinate transformations.
 *   The input space is assumed to be the current GRASS window.
 */
G_setup_plot (t, b, l, r, Move, Cont)
    double t, b, l, r;
    int (*Move)();
    int (*Cont)();
{
    G_get_set_window (&window);

    xconv = (r-l)/(window.east-window.west);
    left = l;

    yconv = (b-t)/(window.north-window.south);
    top = t;

    move = Move;
    cont = Cont;
}

#define X(e) (left + xconv * ((e) - window.west))
#define Y(n) (top + yconv * (window.north - (n)))

#define EAST(x) (window.west + ((x)-left)/xconv)
#define NORTH(y) (window.north - ((y)-top)/xconv)

G_plot_where_xy (east, north, x, y)
    double east, north;
    int *x, *y;
{
    *x = (int) X(G_adjust_easting(east,&window));
    *y = (int) Y(north);
}

G_plot_where_en (x, y, east, north)
    double *east, *north;
{
    *east = G_adjust_easting(EAST(x+.5),&window);
    *north = NORTH(y+.5);
}

/*
 * Line in map coordinates is plotted in output x,y coordinates
 * This routine handles global wrap-around for lat-long databses.
 *
 */
G_plot_line (east1, north1, east2, north2)
    double east1, north1, east2, north2;
{
    int x1,x2,y1,y2;

    y1 = Y(north1);
    y2 = Y(north2);

    if (window.proj == PROJECTION_LL)
    {
	if (east1 > east2)
	    while ((east1-east2) > 180)
		east2 += 360;
	else if (east2 > east1)
	    while ((east2-east1) > 180)
		east1 += 360;
	while (east1 > window.east)
	{
	    east1 -= 360.0;
	    east2 -= 360.0;
	}
	while (east1 < window.west)
	{
	    east1 += 360.0;
	    east2 += 360.0;
	}
	x1 = X(east1);
	x2 = X(east2);

	move(x1, y1);
	cont(x2, y2);

	if (east2 > window.east || east2 < window.west)
	{
	    while (east2 > window.east)
	    {
		east1 -= 360.0;
		east2 -= 360.0;
	    }
	    while (east2 < window.west)
	    {
		east1 += 360.0;
		east2 += 360.0;
	    }
	    x1 = X(east1);
	    x2 = X(east2);
	    move(x1, y1);
	    cont(x2, y2);
	}
    }
    else
    {
	x1 = X(east1);
	x2 = X(east2);
	move (x1, y1);
	cont (x2, y2);
    }
}
/*
 * G_plot_polygon (x, y, n)
 * 
 *    double *x       x coordinates of vertices
 *    double *y       y coordinates of vertices
 *    int n           number of verticies
 *
 * polygon fill from map coordinate space to plot x,y space.
 *     for lat-lon, handles global wrap-around as well as polar polygons.
 *
 * returns 1 ok, 0 n<3, -1 weird internal error
 *
 * Note: at present, this code fills north-south, rather than east-west
 *       This was to allow polygons which outlined the north or south pole
 *       to look like closed polygons and be filled accordingly (a hack!).
 *       This works fine for graphics output which is random access, but
 *       will have to be modified to fill east-west if it is to be used
 *       for vect.to.cell which does file i/o. Also, vect.to.cell has a need
 *       to control edge pixels. The x coordinate in the POINTS structure
 *       must be changed to a double.  See polyfill.c under mapdev/vect_to_cell
 */

#define POINT struct point
POINT
{
    int x;
    int y;
};

static POINT *P;
static int np;
static int npmax;

static double nearest(e0,e1)
    double e0, e1;
{
    while (e0 - e1 > 180)
	e1 += 360.0;
    while (e1 - e0 > 180)
	e1 -= 360.0;
    
    return e1;
}

G_plot_polygon (x,y,n)
    double *x, *y;
{
    static char *me="G_plot_polygon";

    int i;
    int edge_order();
    int x0,x1;
    int y0,y1;
    double shift,E,W;
    double e0,e1;
    int shift1, shift2;

    if (n < 3)
        return 0;

/* traverse the perimeter */

    np = 0;
    P = (POINT *) G_calloc (npmax = 32, sizeof (POINT));

    shift1 = 0;

/* global wrap-around for lat-lon, part1 */
    if (window.proj == PROJECTION_LL)
    {
	e0 = x[n-1];
	E = W = e0;

	x0 = X(e0);
	y0 = Y(y[n-1]);

	for (i = 0; i < n; i++)
	{
	    e1 = nearest (e0, x[i]);
	    if (e1 > E) E = e1;
	    if (e1 < W) W = e1;

	    x1 = X(e1);
	    y1 = Y(y[i]);

	    edge (x0, y0, x1, y1);

	    x0 = x1;
	    y0 = y1;
	    e0 = e1;
	}

	if (i = G_pole_in_polygon(x,y,n)) /* add line at pole */
	{
	    x0 = X(E);
	    x1 = X(W);
	    y1 = y0 = Y(i*90.0);
	    edge(x0,y0,x1,y1);
	}

	shift = 0;        /* shift into window */
	while (E+shift > window.east)
	    shift -= 360.0;
	while (E+shift < window.west)
	    shift += 360.0;
	shift1 = X(x[n-1]+shift) - X(x[n-1]);
    }
    else
    {
	x0 = X(x[n-1]);
	y0 = Y(y[n-1]);

	for (i = 0; i < n; i++)
	{
	    x1 = X(x[i]);
	    y1 = Y(y[i]);
	    edge (x0,y0,x1,y1);
	    x0 = x1;
	    y0 = y1;
	}
    }

/* check if perimeter has odd number of points */
    if (np%2)
    {
        if (P)
            free (P);
        return -1;
    }

/* sort the edge points by col(x) and then by row(y) */
    qsort (P, np, sizeof(POINT), edge_order);

/* plot */
    for (i = 1; i < np; i += 2)
    {
        if (P[i].x != P[i-1].x)
        {
            fprintf(stderr,"%s: OOPS, out of sync\n", me);
            continue;
        }
	move (P[i-1].x+shift1, P[i-1].y);
	cont (P[i].x+shift1, P[i].y);
    }
    if (window.proj == PROJECTION_LL)	/* now do wrap-around, part 2 */
    {
	shift = 0;
	while (W+shift < window.west)
	    shift += 360.0;
	while (W+shift > window.east)
	    shift -= 360.0;
	shift2 = X(x[n-1]+shift) - X(x[n-1]);
	if (shift2 != shift1)
	{
	    for (i = 1; i < np; i += 2)
	    {
		move (P[i-1].x+shift2, P[i-1].y);
		cont (P[i].x+shift2, P[i].y);
	    }
	}
    }

    free (P);
    return 1;
}

static
edge (x0,y0,x1,y1)
    register int x0, y0 ;
    int x1, y1 ;
{
    register float m;
    register float y;


    if (x0 == x1) return;

    m = (float) (y0 - y1) / (float) (x0 - x1) ;

    if (x0 < x1)
    {
        y = y0;
        while (++x0 <= x1)
        {
            y0 = (y += m) + .5;
            edge_point (x0, y0);
        }
    }
    else
    {
        y = y1;
        while (++x1 <= x0)
        {
            y1 = (y += m) + .5;
            edge_point (x1, y1);
        }
    }
}

static
edge_point (x, y)
    register int x, y;
{

    if (np >= npmax)
        P = (POINT *) G_realloc (P, (npmax += 32) * sizeof (POINT));
    P[np].x   = x;
    P[np++].y = y;
}

static
edge_order (a, b)
    struct point *a, *b;
{
    if (a->x < b->x) return (-1);
    if (a->x > b->x) return (1);

    if (a->y < b->y) return (-1);
    if (a->y > b->y) return (1);

    return (0);
}
/*
 * G_plot_fx(e1,e2)
 *
 * plot f(x) from x=e1 to x=e2
 */

G_plot_fx (f, east1, east2)
    double (*f)();
    double east1, east2;
{
    double east,north,north1;
    double incr;


    incr = fabs(1.0 / xconv) ;

    east  = east1;
    north = f(east1);

    if (east1 > east2)
    {
	while ((east1 -=incr) > east2)
	{
	    north1 = f(east1);
	    G_plot_line (east, north, east1, north1);
	    north = north1;
	    east  = east1;
	}
    }
    else
    {
	while ((east1 += incr) < east2)
	{
	    north1 = f(east1);
	    G_plot_line (east, north, east1, north1);
	    north = north1;
	    east  = east1;
	}
    }
    G_plot_line (east, north, east2, f(east2));
}
