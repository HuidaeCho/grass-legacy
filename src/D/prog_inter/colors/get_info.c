/*  %W%  %G%  */

#include <stdio.h>
#include "gis.h"
#include "options.h"

get_map_info()
{
	char error ;
	struct Colors colors ;
	struct Categories categories ;
	char buff[128] ;

	if(!map_name) 
		exit(0);
	if (*map_name == NULL)
		exit(0) ;

/* Reading color lookup table */
	if (G_read_cats(map_name, mapset, &categories) == -1)
	{
		sprintf(buff,"category file for [%s] not available", map_name) ;
		G_fatal_error(buff) ;
	}

/* Reading color lookup table */
	if (G_read_colors(map_name, mapset, &colors) == -1)
	{
		sprintf(buff,"color file for [%s] not available", map_name) ;
		G_fatal_error(buff) ;
	}

/* Set the colors for the display */
	D_reset_colors(&colors);

	interact(&categories, &colors) ; 

/* Wrapup graphics */
	R_flush() ;
}
