/*  @(#)scal_window.c    1.0  01/28/90  */
/*
**-->  Written by R.L.Glenn, USDA, SCS
**  from mk_window.c, By Dave Gerdes
**  US Army Construction Engineering Research Lab
*/

#include "digit.h"
#include "wind.h"

#define SCALE_FACTOR 0.8

scal_window_w_mouse (button)
    int button;
{
    int screen_x, screen_y ;
    double N, S, E, W;
    double delta;
    char buffer[64] ;



	switch (button)
        {
	    case 1:
		/* ZOOM IN */
                W = U_west  + (U_east - U_west)   * (1. - SCALE_FACTOR);
                E = U_east  - (U_east - U_west)   * (1. - SCALE_FACTOR);
                S = U_south + (U_north - U_south) * (1. - SCALE_FACTOR);
                N = U_north - (U_north - U_south) * (1. - SCALE_FACTOR);

	        window_rout (N, S, E, W);
		/*
                clear_window ();
                replot(CM); 
		*/
		redraw();
		break;

	    case 2:
                return(0);
		break;

	    case 3:
		/* ZOOM OUT */
                W = U_west  - (U_east - U_west)   * (1. - SCALE_FACTOR);
                E = U_east  + (U_east - U_west)   * (1. - SCALE_FACTOR);
                S = U_south - (U_north - U_south) * (1. - SCALE_FACTOR);
                N = U_north + (U_north - U_south) * (1. - SCALE_FACTOR);

	        window_rout (N, S, E, W);
		/*
                clear_window ();
                replot(CM); 
		*/
		redraw();

	        break ;

	    default:
	        return(1) ;
	        break ;

	 } /* end switch */

}
