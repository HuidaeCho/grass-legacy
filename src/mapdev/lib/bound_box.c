/* %W% %G% */
#include    "digit.h"
#include    "head.h"

#define	SCALE    16000.0	
/*	I arbitrarily increased this to help find straight lines ...
#define	BOX_MIN    23
*/
#define	BOX_MIN    46

static  int  box_min = 0 ;    

dig_bound_box (points, N, S, E, W)
    struct line_pnts *points;
    double *N, *S, *E, *W;
{

    double *xptr, *yptr;
    int n_coors;
    double	dist;
    double	half_dist;

    double    scale;
    double    fabs();

    /*
    *    on straight lines the bounding box was so small it made it hard to find
    *    the line with a mouse.   first time this is called set the bounding box
    *    minimum limit.  the minimum was calculated at a scale of 16000;
    *    takes any scale into account.
    *    
    *    find current distance between (N,S), (E,W).  if its less then box_min
    *    enlarge the box to the minimum;  it also keeps the line in the middle
    *    of the box.
    */

    xptr = points->x; 
    yptr = points->y; 
    n_coors = points->n_points;

    /*  first time this was called  */
    if ( box_min <= 0)
     {
	scale = (double)head.orig_scale;
	if ( scale <= 0)
	    box_min =  BOX_MIN;
	else
	    box_min = scale  /  SCALE  *  BOX_MIN;
     }

    if (n_coors == 0)
	return(1);

    *E = *xptr;
    *W = *xptr;
    *N = *yptr;
    *S = *yptr;

    while(--n_coors)
    {
	xptr++;
	yptr++;
	if (*xptr < *W) *W = *xptr;
	if (*xptr > *E) *E = *xptr;
	if (*yptr < *S) *S = *yptr;
	if (*yptr > *N) *N = *yptr;
    }

    /*  have got the bounding box,  now make sure its large enough  */

    dist = (*N - *S) - box_min;
    if ( dist < 0)
     {
	half_dist =  fabs (dist)  /  2;
	*N += half_dist;
	*S -= half_dist;
     }

    dist = (*E - *W) - box_min;
    if ( dist < 0)
     {
	half_dist =  fabs (dist)  /  2;
	*E += half_dist;
	*W -= half_dist;
     }

    return(0);
}
