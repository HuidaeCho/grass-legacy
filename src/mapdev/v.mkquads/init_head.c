#include <string.h>
#include <stdio.h>
#include "gis.h"
#include "Vect.h"

int init_header (FILE *fp_digit, struct Cell_head *window,
	struct dig_head *d_head)
{

	strcpy(d_head->organization, "US Army Const. Eng. Rsch. Lab") ;
	strcpy (d_head->date, "");
	strcpy (d_head->your_name, "");
	strcpy (d_head->map_name, "");
	strcpy (d_head->source_date, "");
	strcpy (d_head->line_3, "");
	d_head->orig_scale = 0 ;
	d_head->digit_thresh = 0.0 ;
	d_head->map_thresh = 0.0 ;
	d_head->plani_zone = window->zone ;

/*  load default window settings into digit header  */
	d_head->W = window->west ;
	d_head->E = window->east ;
	d_head->S = window->south ;
	d_head->N = window->north ;

   /*superceded by Vect_copy.. in calling prog*/
/*	dig_write_head_binary( fp_digit, d_head) ; */

	return 0;
}
