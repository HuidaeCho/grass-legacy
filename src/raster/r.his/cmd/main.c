/*
 * $Id$
 *
 ****************************************************************************
 *
 * MODULE:       r.his
 * AUTHOR(S):    Glynn Clements - glynn.clements@virgin.net
 * PURPOSE:      Create a color image by composing color, brightness
 *               and haze maps
 * COPYRIGHT:    (C) 2001 by the GRASS Development Team
 *
 *               This program is free software under the GNU General Public
 *   	    	 License (>=v2). Read the file COPYING that comes with GRASS
 *   	    	 for details.
 *
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include "gis.h"
#include "display.h"
#include "raster.h"
#include "his.h"

int 
main (int argc, char **argv)
{
	CELL *hue_array ;
	CELL *int_array = NULL;
	CELL *sat_array = NULL;
	CELL *r_array, *g_array, *b_array;
	char *mapset ;
	char *name_h, *name_i, *name_s ;
	int intensity ;
	int saturation ;
	int atrow, atcol ;
	int next_row;
	int hue_file ;
	int int_file = 0;
	int int_used ;
	int sat_file = 0;
	int sat_used ;
	char *name_r, *name_g, *name_b ;
	int r_file = 0;
	int r_used ;
	int g_file = 0;
	int g_used ;
	int b_file = 0;
	int b_used ;
	struct Cell_head window ;
	struct Colors hue_colors ;
	struct Colors int_colors ;
	struct Colors sat_colors ;
	struct Colors gray_colors ;
	struct GModule *module;
	struct Option *opt_h, *opt_i, *opt_s;
	struct Option *opt_r, *opt_g, *opt_b;
	struct Flag *flg1 ;

	G_gisinit(argv[0]) ;

	module = G_define_module();
	module->description =
		"Generates red, green and blue raster map layers "
		"combining hue, intensity, and saturation (his) "
		"values from user-specified input raster map layers.";

	opt_h = G_define_option() ;
	opt_h->key        = "h_map" ;
	opt_h->type       = TYPE_STRING ;
	opt_h->required   = YES ;
	opt_h->gisprompt  = "old,cell,raster" ;
	opt_h->description= "Name of layer to be used for HUE" ;

	opt_i = G_define_option() ;
	opt_i->key        = "i_map" ;
	opt_i->type       = TYPE_STRING ;
	opt_i->required   = NO ;
	opt_i->gisprompt  = "old,cell,raster" ;
	opt_i->description= "Name of layer to be used for INTENSITY" ;

	opt_s = G_define_option() ;
	opt_s->key        = "s_map" ;
	opt_s->type       = TYPE_STRING ;
	opt_s->required   = NO ;
	opt_s->gisprompt  = "old,cell,raster" ;
	opt_s->description= "Name of layer to be used for SATURATION" ;

	opt_r = G_define_option() ;
	opt_r->key        = "r_map" ;
	opt_r->type       = TYPE_STRING ;
	opt_r->required   = YES ;
	opt_r->gisprompt  = "old,cell,raster" ;
	opt_r->description= "Name of output layer to be used for RED" ;

	opt_g = G_define_option() ;
	opt_g->key        = "g_map" ;
	opt_g->type       = TYPE_STRING ;
	opt_g->required   = YES ;
	opt_g->gisprompt  = "old,cell,raster" ;
	opt_g->description= "Name of output layer to be used for GREEN" ;

	opt_b = G_define_option() ;
	opt_b->key        = "b_map" ;
	opt_b->type       = TYPE_STRING ;
	opt_b->required   = YES ;
	opt_b->gisprompt  = "old,cell,raster" ;
	opt_b->description= "Name of output layer to be used for BLUE" ;

	flg1 = G_define_flag() ;
	flg1->key	  = 'o' ;
	flg1->description = "Overwrite output maps" ;

	if (G_parser(argc, argv))
		exit(-1);

	/* read in current window */
	G_get_window(&window) ;

	/* Get name of layer to be used for hue */
	name_h = opt_h->answer;

	mapset = G_find_cell2(name_h, "");
	if (mapset == NULL)
		G_fatal_error("%s: <%s> cell file not found\n",
			      G_program_name(),
			      opt_h->answer);

	/* Make sure map is available */
	if ((hue_file = G_open_cell_old(name_h, mapset)) == -1)
		G_fatal_error("Not able to open cellfile for [%s]",
			      name_h) ;

	hue_array = G_allocate_cell_buf () ;

	/* Reading color lookup table */
	if (G_read_colors(name_h, mapset, &hue_colors) == -1)
		G_fatal_error("Color file for [%s] not available",
			      name_h) ;

	int_used = 0 ;

	if (opt_i->answer != NULL)
	{
		/* Get name of layer to be used for intensity */
		name_i = opt_i->answer;
		mapset = G_find_cell2(name_i, "");
		if (mapset != NULL)
		{
			int_used = 1 ;
			/* Make sure map is available */
			if ((int_file = G_open_cell_old(name_i, mapset)) == -1)
				G_fatal_error("Not able to open cellfile for [%s]",
					      name_i) ;

			int_array = G_allocate_cell_buf () ;

			/* Reading color lookup table */
			if (G_read_colors(name_i, mapset, &int_colors) == -1)
				G_fatal_error("Color file for [%s] not available",
					      name_i) ;
		}
	}

	sat_used = 0 ;

	if (opt_s->answer != NULL)
	{
		/* Get name of layer to be used for saturation */
		name_s = opt_s->answer;
		mapset = G_find_cell2 (name_s, "");
		if (mapset != NULL)
		{
			sat_used = 1 ;

			/* Make sure map is available */
			if ((sat_file = G_open_cell_old(name_s, mapset)) == -1)
				G_fatal_error("Not able to open cellfile for [%s]",
					      name_s) ;

			sat_array = G_allocate_cell_buf () ;

			/* Reading color lookup table */
			if (G_read_colors(name_s, mapset, &sat_colors) == -1)
				G_fatal_error("Color file for [%s] not available",
					      name_s) ;
		}
	}

	r_used = 0;

	if (opt_r->answer != NULL)
	{
		name_r = opt_r->answer;

		mapset = G_find_cell2( name_r, "");
		if (mapset != NULL)
		{
			if (flg1->answer)
				G_remove("cell", name_r);
			else
				G_fatal_error("%s: <%s> cell file exists already\n",
					      G_program_name(), name_r);
		}
		if ((r_file = G_open_cell_new (name_r)) < 0)
			r_used = 0;
		else
			r_used = 1;
	}

	g_used = 0;

	if (opt_g->answer != NULL)
	{
		name_g = opt_g->answer;

		mapset = G_find_cell2( name_g, "");
		if (mapset != NULL)
		{
			if (flg1->answer)
				G_remove("cell", name_g);
			else
				G_fatal_error("%s: <%s> cell file exists already\n",
					      G_program_name(), name_g);
		}
		if ((g_file = G_open_cell_new (name_g)) < 0)
			g_used = 0;
		else
			g_used = 1;
	}

	b_used = 0;

	if (opt_b->answer != NULL)
	{
		name_b = opt_b->answer;

		mapset = G_find_cell2(name_b, "");
		if (mapset != NULL)
		{
			if (flg1->answer)
				G_remove("cell", name_b);
			else
				G_fatal_error("%s: <%s> cell file exists already\n",
					      G_program_name(), name_b);
		}
		if ((b_file = G_open_cell_new (name_b)) < 0)
			b_used = 0;
		else
			b_used = 1;
	}

	r_array = G_allocate_cell_buf () ;
	g_array = G_allocate_cell_buf () ;
	b_array = G_allocate_cell_buf () ;

	/* Make color table */
	make_gray_scale(&gray_colors) ;

	/* Now do the work */
	intensity  = 255 ;  /* default is to not change intensity */
	saturation = 255 ;  /* default is to not change saturation */

	for (atrow=0; atrow<window.rows; atrow++)
	{
		G_percent (atrow, window.rows, 2);

		if(G_get_c_raster_row(hue_file, hue_array, atrow) < 0)
			exit(1);
		if (int_used && (G_get_c_raster_row(int_file, int_array, atrow) < 0))
			exit(1);
		if (sat_used && (G_get_c_raster_row(sat_file, sat_array, atrow) < 0))
			exit(1);

		for (atcol=0; atcol<window.cols; atcol++)
		{
			int r, g, b ;

			if (int_used)
			{
				G_get_color(int_array[atcol], &r, &g, &b, &int_colors) ;
				intensity = r;
				/* intensity = (r + g + b) / 3; */
			}

			if (sat_used)
			{
				G_get_color(sat_array[atcol], &r, &g, &b, &sat_colors) ;
				saturation = r;
				/* saturation = (r + g + b) / 3; */
			}

			G_get_color(hue_array[atcol], &r, &g, &b, &hue_colors) ;

			HIS_to_RGB(r, g, b, intensity, saturation,
				   &r_array[atcol], &g_array[atcol], &b_array[atcol]) ;
		}

		if (r_used)
			if(G_put_raster_row (r_file, r_array, CELL_TYPE) < 0)
				r_used = 0;

		if (g_used)
			if(G_put_raster_row (g_file, g_array, CELL_TYPE) < 0)
				g_used = 0;

		if (b_used)
			if(G_put_raster_row (b_file, b_array, CELL_TYPE) < 0)
				b_used = 0;
	}
	G_percent (window.rows, window.rows, 5);

	/* Close the cell files */
	G_close_cell(hue_file) ;
	if (int_used)
		G_close_cell(int_file) ;
	if (sat_used)
		G_close_cell(sat_file) ;
	if (r_used)
	{
		G_close_cell(r_file);
		G_write_colors (name_r, G_mapset(), &gray_colors);
	}
	if (g_used)
	{
		G_close_cell(g_file);
		G_write_colors (name_g, G_mapset(), &gray_colors);
	}
	if (b_used)
	{
		G_close_cell(b_file);
		G_write_colors (name_b, G_mapset(), &gray_colors);
	}

	return 0;
}
