#include <stdio.h>
#include "digit.h"
#include "dig_curses.h"
#include	"map.h"
#include "local_proto.h"
#include "glocale.h"

#define	SPACES	"  "
#define	POINTER	"->"
#define	STAR	"* "

int 
show_residual_results (int n_points, int active_point)
{
	int	i ;
	int	screen_no ;
	char	*ptr ;
	char	*active_ptr ;
	char	*reg_ptr ;

	double	m_scale ;		/*  map scale  */

	char	buff[85] ;

	ptr = buff ;
	screen_no = 3 ;


	Write_base (1, _("                        POINTS TO REGISTER"));
#ifdef LATLON
        if (ll_flag)
	  Write_base (2, _("     POINT                LATITUDE             LONGTITUDE     RESIDUALS   "));
        else
#endif
	  Write_base (2, _("     POINT               EASTING (X)           NORTHING (Y)    RESIDUALS   "));

	for (  i = 0 ;  i < n_points;  i++ )
 	{
		if ( active_point == i)
			active_ptr = POINTER ;
		else
			active_ptr = SPACES ;

		if ( use[i])
			reg_ptr = STAR ;
		else
			reg_ptr = SPACES ;
#ifdef LATLON
                if (ll_flag) 
	  	  sprintf ( ptr, " %2s %2s %2d.    %20s   %20s  %12.2f      ",
			active_ptr, reg_ptr, i+1, bcx[i], bcy[i], residuals[i]) ;
                else 
#endif
                  sprintf (ptr," %2s %2s %2d.      %12.2f    %12.2f  %12.2f",
			active_ptr, reg_ptr, i+1, bx[i], by[i], residuals[i]) ;
	
		Write_base (screen_no++, ptr) ;
 	}
		
	screen_no += 2 ;
	sprintf ( ptr, _(" Number of points: %d,   Points registered: %d"),
		n_points, reg_cnt) ;
	Write_base (screen_no++, ptr) ;

	sprintf ( ptr, _(" Residual mean average   : %f"), rms) ;
	Write_base (screen_no, ptr) ;

	calculate_map_scale() ;

	/*  multiply residual (meters) by 1000 to convert into mm
	*	divide by scale to match the paper map
	*
	*  rms stands for root mean sqaure .
	*/

/****  not sure what units this map may be in  COMMENTED OUT
	m_scale = get_map_scale() ;
	sprintf ( ptr, " Residual mean average (millimeters) to the map  : %f", (rms*1000)/m_scale) ;
	Write_base (screen_no++, ptr) ;
*********/


	return (0) ;

}		/*  show_residual_results ()  */


int 
show_coor_only (int n_points, int active_point, int status)
{
	int	i ;
	int	screen_no ;
	char	*ptr ;
	char	*active_ptr ;
	char	*reg_ptr ;
	char	buff[85] ;

	ptr = buff ;
	screen_no = 3 ;

	if( n_points <= 0)
	{
		Write_base ( 5, _("  There are no points to register.")) ;
		Write_base ( 6, _("  You need to type in the coordinates of your points  before ")) ;
		Write_base ( 7, _("  you can register the map.")) ;
		Write_base ( 9, _("  Use the 'add more points' option and enter the ")) ;
		Write_base ( 10, _("  coordinates of the points.")) ;
		return(0) ;
	}

	Write_base (1, _("                        POINTS TO REGISTER"));
#ifdef LATLON
        if (ll_flag)
	  Write_base (2, _("     POINT              LATITUDE             LONGTITUDE                 "));
        else
#endif
	  Write_base (2, _("     POINT             EASTING (X)           NORTHING (Y)                 "));

	for (  i = 0 ;  i < n_points;  i++ )
 	{
		if ( active_point == i)
			active_ptr = POINTER ;
		else
			active_ptr = SPACES ;

		if ( use[i])
			reg_ptr = STAR ;
		else
			reg_ptr = SPACES ;

#ifdef LATLON
                if (ll_flag)
	  	  sprintf ( ptr, " %2s %2s %2d.   %20s   %20s         ",
			active_ptr, reg_ptr, i+1, bcx[i], bcy[i]) ; 
                else 
#endif
		  sprintf ( ptr, " %2s %2s %2d.     %12.2f    %12.2f         ",
			active_ptr, reg_ptr, i+1, bx[i], by[i]) ;
	
		Write_base (screen_no++, ptr) ;
 	}
	screen_no += 2 ;
	sprintf ( ptr, _(" Number of points: %d,   Points registered: %d"),
		n_points, reg_cnt) ;
	Write_base (screen_no++, ptr) ;

    /* 
    *  We can't show residuals since they are no good, but tell them why. 
    *  Else overwrite the line without redrawing the entire screen. 
    */
	if( status == -1)
		sprintf ( ptr, _(" Points aren't spread out enough.                 ")) ;
	else
		sprintf ( ptr, "                                                  ") ;
	Write_base (screen_no, ptr) ;

	return(0) ;
}

