
#include <stdio.h>
#include "digit.h"
#include "dig_head.h"

static  double  scale ;
double dig_unit_conversion ();

/*  functions calculate mapscale, get map scale and check it with the user  */


calculate_map_scale ()
{
	int	i ;
	int	screen_no ;
	char	*ptr ;

	double	x1, y1 ;
	double	x2, y2 ;
	double	X, Y ;
	double	hypot_x,  hypot_y,  hypot_avg ;
	float	d_scale ;		/*  digitizer scale  */

	double	hypot() ;
	double	fabs() ;


	transform_a_into_b ( (double)0, (double)0, &X, &Y) ;
	transform_a_into_b ( (double)10, (double)0, &x1, &y1) ;
	transform_a_into_b ( (double)0, (double)10, &x2, &y2) ;

	hypot_x = hypot ( fabs(x1-X), fabs(y1-Y)) ;
	hypot_y = hypot ( fabs(x2-X), fabs(y2-Y)) ;

	hypot_avg = (hypot_x + hypot_y) / 2.0 ;

	/*  
	*	convert digitizer units (inches) to meters by multipling by .0254 and 
	*	multiply by 10 for the number of digiter units used to compute the
	*	hypot average.	0.0254 * 10 = 0.254
	*/

	D_get_scale (&d_scale) ;		/*  get digitizer scale  */

	scale = hypot_avg / ( d_scale * (10. * dig_unit_conversion ())) ;
	/*
	scale = hypot_avg / ( d_scale * .254) ;
	*/
		
}


double
get_map_scale()
{
	return (scale) ;
}


