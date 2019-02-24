/*
 *   Chris Rewerts, Agricultural Engineering, Purdue University
 *   rewerts@ecn.purdue.edu  March 1991
 *
 *   d.rast.arrow
 *
 *   Usage:  d.rast.arrow
 * 
 *   This program used Dgrid's sources as a beginning. Purpose of Darrow
 *   is to read an aspect layer produced by slope.aspect or by the 
 *   programs created for the ANSWERS or AGNPS Hydrology Toolbox
 *   endeavors.  d.rast.arrow draws an arrow on the graphic display
 *   of each cell, so that the flow pattern computed as an aspect
 *   layer can be easily seen. Other symbols ("?", "X") may be drawn
 *   as needed.
 */

#include "gis.h"
#define MAIN

    int D_x, D_y ;
    double D_ew, D_ns;
    static stash();
    char *mapset;
    char layer_name[128];
    int layer_set;
    int map_type, arrow_color, grid_color, x_color, unknown_color;

main(argc, argv)
int argc ;
char **argv ;

{
    int stat;
    extern double D_ew, D_ns;
    extern int D_x, D_y ; 
    char window_name[128] ;
    struct Cell_head window ;
    int i;
    int t, b, l, r ;
    char full_name[128] ;
    int layer_fd;
    char string[128];
    CELL *cell;
    int nrows, ncols, row, col;
    double ew_res, ns_res;
    double D_south, D_west ;
    double D_north, D_east ;
    double U_to_D_xconv, U_to_D_yconv ;
    double U_west, U_south ;
    double U_east, U_north ;
    double D_get_d_south(), D_get_d_west() ;
    double D_get_d_north(), D_get_d_east() ;
    double D_get_u_to_d_xconv(), D_get_u_to_d_yconv() ;
    double D_get_u_west(), D_get_u_south() ;
    double D_get_u_east(), D_get_u_north() ;
    double U_start;
    double U_x, U_y ;
	struct Option *opt1, *opt2, *opt3, *opt4, *opt5, *opt6 ;

	G_gisinit(argv[0]) ;

    opt1 = G_define_option() ;
    opt1->key        = "map" ;
    opt1->type       = TYPE_STRING ;
    opt1->required   = NO ;
    opt1->multiple   = NO ;
    opt1->gisprompt  = "old,cell,raster" ;
    opt1->description= "Name of existing raster map to be displayed" ;

    opt2 = G_define_option() ;
    opt2->key        = "type" ;
    opt2->type       = TYPE_STRING ;
    opt2->required   = NO ;
    opt2->answer     = "grass" ;
    opt2->options    = "grass,agnps,answers" ;
    opt2->description= "Type of existing raster map to be displayed" ;

    opt3 = G_define_option() ;
    opt3->key        = "arrow_color" ;
    opt3->type       = TYPE_STRING ;
    opt3->required   = NO ;
    opt3->answer     = "green" ;
    opt3->options = "white,red,orange,yellow,green,blue,indigo,violet,magenta,brown,gray,black";
    opt3->description= "Color for drawing arrows" ;

    opt4 = G_define_option() ;
    opt4->key        = "grid_color" ;
    opt4->type       = TYPE_STRING ;
    opt4->required   = NO ;
    opt4->answer     = "gray" ;
    opt4->options = "white,red,orange,yellow,green,blue,indigo,violet,magenta,brown,gray,black";
    opt4->description= "Color for outlining grids" ;

    opt5 = G_define_option() ;
    opt5->key        = "x_color" ;
    opt5->type       = TYPE_STRING ;
    opt5->required   = NO ;
    opt5->answer     = "white" ;
    opt5->options = "white,red,orange,yellow,green,blue,indigo,violet,magenta,brown,gray,black";
    opt5->description= "Color for drawing x's" ;

    opt6 = G_define_option() ;
    opt6->key        = "unknown_color" ;
    opt6->type       = TYPE_STRING ;
    opt6->required   = NO ;
    opt6->answer     = "red" ;
    opt6->options = "white,red,orange,yellow,green,blue,indigo,violet,magenta,brown,gray,black";
    opt6->description= "Color for showing unknown information" ;

    /* Check command line */
    if (G_parser(argc, argv))
        exit(-1);

	if (opt1->answer)
	{
		strcpy(layer_name, opt1->answer);
		if((mapset = G_find_cell2 (layer_name, "")) == NULL)
		{
			fprintf ("warning: [%s] not found\n", layer_name);
			return(1);
		}
		layer_set = 1 ;
	}
	else
		layer_set = 0 ;

    arrow_color = D_translate_color(opt3->answer) ;
    grid_color = D_translate_color(opt4->answer) ;
    x_color = D_translate_color(opt5->answer) ;
    unknown_color = D_translate_color(opt6->answer) ;

	if (strcmp("grass", opt2->answer) == 0) 
	{
		map_type = 1;
	}
	else if (strcmp("agnps", opt2->answer) == 0) 
	{
		map_type = 2;
	}
	else if (strcmp("answers", opt2->answer) == 0) 
	{
		map_type = 3;
	}

/* Setup driver and check important information */

    R_open_driver();

    if (D_get_cur_wind(window_name))
        G_fatal_error("No current window") ;

    if (D_set_cur_wind(window_name))
        G_fatal_error("Current window not available") ;

/* Read in the map window associated with window */

    G_get_window(&window) ;

    if (D_check_map_window(&window))
        G_fatal_error("Setting map window") ;

    if (G_set_window(&window) == -1) 
        G_fatal_error("Current window not settable") ;

/* Determine conversion factors */

    if (D_get_screen_window(&t, &b, &l, &r))
        G_fatal_error("Getting screen window") ;
    if (D_do_conversions(&window, t, b, l, r))
    	G_fatal_error("Error in calculating conversions") ;

/* where are we, both geographically and on the screen? */

    D_south = D_get_d_south() ;
    D_north = D_get_d_north() ;
    D_east =  D_get_d_east() ;
    D_west =  D_get_d_west() ;

    U_west = D_get_u_west() ;
    U_east = D_get_u_east() ;
    U_south = D_get_u_south() ;
    U_north = D_get_u_north() ;

    U_to_D_xconv = D_get_u_to_d_xconv() ;
    U_to_D_yconv = D_get_u_to_d_yconv() ;

/* number of rows and cols in window */

    nrows = window.rows;
    ncols = window.cols;

/*
    if ((nrows > 75) || (ncols > 75)){ 
        printf("\n"); 
        printf("Current window size:\n"); 
        printf("rows:    %d\n", nrows);
        printf("columns: %d\n", ncols);
        printf("\n"); 
        printf("Your current window setting may be too large.\n"); 
        printf("Cells displayed on your graphics window may be too\n"); 
        printf("small for arrows to be visible.\n\n"); 
        if (!G_yes("Do you wish to continue", 0))
            exit(0);
    }
*/

/* resolutions */

    ew_res  = window.ew_res;
    ns_res = window.ns_res;

/* how many screen units of distance for each cell */

    D_ew = (D_east - D_west) / ncols;
    D_ns = (D_south - D_north) / nrows; 

/*------------------------------------------
    printf("ew_res:  %.2f\n", window.ew_res);
    printf("ns_res:  %.2f\n", window.ns_res);
    printf("D_ew:  %f D_ns:  %f \n", D_ew, D_ns); 
    printf("nrows:    %d\n", nrows);
    printf("ncols:    %d\n", ncols);
    printf("t:  %d\n", t);
    printf("b:  %d\n", b);
    printf("l:  %d\n", l);
    printf("r:  %d\n", r);
    printf("U_west:	%f\n", U_west);
    printf("U_east:	%f\n", U_east);
    printf("U_south:	%f\n", U_south);
    printf("U_north:	%f\n", U_north);
    printf("D_west:	%f\n", D_west);
    printf("D_east:	%f\n", D_east);
    printf("D_south:	%f\n", D_south);
    printf("D_north:	%f\n", D_north);
    printf("U_to_D_xconv:	%f\n", U_to_D_xconv);
    printf("U_to_D_yconv:	%f\n", U_to_D_yconv);
--------------------------------------------------------*/

/* Set color */

    R_standard_color(grid_color);

/* Draw vertical grids */
    U_start = U_east;
    for (U_x=U_start; U_x>=U_west; U_x -= ew_res)
    {
        D_x = (int)( ( U_x - U_west ) * U_to_D_xconv + D_west );
        R_move_abs(D_x, (int)D_south) ;
        R_cont_abs(D_x, (int)D_north) ;
    }

/* Draw horizontal grids */
    U_start = U_north;
    for (U_y=U_start; U_y>=U_south; U_y -= ns_res)
    {
        D_y = (int)( ( U_south - U_y ) * U_to_D_yconv + D_south );
        R_move_abs((int)D_west, D_y) ;
        R_cont_abs((int)D_east, D_y) ;
    }

/* if we didn't get a layer name from the arg options, then
   get name of layer that is on the screen */

    if (!layer_set) {
        if(D_get_cell_name (full_name)) {
            printf ("warning: no data layer drawn in current window\n");
            exit(0);
        }
        mapset = G_find_cell (full_name, "");
        if(mapset == NULL) {
            printf ("warning: [%s] not found\n", full_name);
            exit(0);
        }
        sscanf (full_name, "%s", layer_name);
    }

/* open the cell file */

    layer_fd = G_open_cell_old (layer_name, mapset);
    if (layer_fd < 0) {
        fprintf ("warning: unable to open [%s] in [%s]\n", layer_name, mapset);
        exit(0);
    }


/* allocate the cell array */

   cell  = G_allocate_cell_buf ();

/* loop through cells, find value, determine direction (n,s,e,w,ne,se,sw,nw),
   and call appropriate function to draw an arrow on the cell */
   
   for(row = 0; row < nrows; row++)
    {
	G_get_map_row (layer_fd, cell, row);

/* determine screen y coordinate of top of current cell */

        D_y = (int)(row * D_ns + D_north) ;

        for(col = 0; col < ncols; col++)
        {

/* determine screen x coordinate of west side of current cell */

            D_x = (int)(col * D_ew + D_west);

/* find aspect direction based on cell value, draw arrow */

/*case switch for standard aspect map */

            if(map_type == 1){
            R_standard_color(arrow_color);
            switch(cell[col]) 
            {
                case 1:
                case 2:
                case 24:
                    arrow_e();
                    break;
                case 3:
                case 4:
                case 5:
                    arrow_ne();
                    break;
                case 6:
                case 7:
                case 8:
                    arrow_n();
                    break;
                case 9:
                case 10:
                case 11:
                    arrow_nw();
                    break;
                case 12:
                case 13:
                case 14:
                    arrow_w();
                    break;
                case 15:
                case 16:
                case 17:
                    arrow_sw();
                    break;
                case 18:
                case 19:
                case 20:
                    arrow_s();
                    break;
                case 21:
                case 22:
                case 23:
                    arrow_se();
                    break;
                 case 25:
                    R_standard_color(unknown_color);
                    unknown_();
                    R_standard_color(arrow_color);
                    break;
                default:
                    R_standard_color(x_color);
                    draw_x();
                    R_standard_color(arrow_color);
                    break;
            }
            }
            else if(map_type == 2){
            R_standard_color(arrow_color);

/* case switch for agnps type aspect map */

            switch(cell[col]) 
            {
               case 0:
                    R_standard_color(x_color);
                    draw_x();
                    R_standard_color(arrow_color);
                    break;
                case 1:
                    arrow_n();
                    break;
                case 2:
                    arrow_ne();
                    break;
                case 3:
                    arrow_e();
                    break;
                case 4:
                    arrow_se();
                    break;
                case 5:
                    arrow_s();
                    break;
                case 6:
                    arrow_sw();
                    break;
                case 7:
                    arrow_w();
                    break;
                case 8:
                    arrow_nw();
                    break;
                default:
                    unknown_();
                    break;
             }
             }
            else if(map_type == 3){
            R_standard_color(arrow_color);

/* case switch for answers type aspect map */

            switch(cell[col]) 
            {
                case 15:
                    arrow_n();
                    break;
                case 30:
                case 45:
                case 60:
                    arrow_ne();
                    break;
                case 75:
                case 90:
                case 105:
                    arrow_e();
                    break;
                case 120:
                case 135:
                case 150:
                    arrow_se();
                    break;
                case 165:
                case 180:
                case 195:
                    arrow_s();
                    break;
                case 210:
                case 225:
                case 240:
                    arrow_sw();
                    break;
                case 255:
                case 270:
                case 285:
                    arrow_w();
                    break;
                case 300:
                case 315:
                case 330:
                    arrow_nw();
                    break;
                case 345:
                case 360:
                    arrow_n();
                    break;
                case 400:
                    R_standard_color(unknown_color);
                    unknown_();
                    R_standard_color(arrow_color);
                    break;
                default:
                    R_standard_color(x_color);
                    draw_x();
                    R_standard_color(arrow_color);
                    break;
            }
            }


        }
    }
    G_close_cell (layer_fd);
    R_close_driver();
}

/* --- end of main --- */

/*---------------------------------------------------------------*/

arrow_se()
{
    extern double D_ew, D_ns;
    extern int D_x, D_y;
    int x,y;

    x = D_x + (int) (D_ew * .8);
    y = D_y + (int) (D_ns * .8);
    R_move_abs(x,y);
    R_cont_rel((int)(D_ew *(-.6)),((int)(D_ns *(-.6))));
    R_move_abs(x,y);
    R_cont_rel(0, (int)(D_ns*(-.4)));
    R_move_abs(x,y);
    R_cont_rel((int)(D_ew*(-.4)),0);

}

arrow_ne()
{
    extern double D_ew, D_ns;
    extern int D_x, D_y;
    int x,y;

    x = D_x + (int) (D_ew * .8);
    y = D_y + (int) (D_ns * .2);
    R_move_abs(x,y);
    R_cont_rel((int)(D_ew *(-.6)),((int)(D_ns *(.6))));
    R_move_abs(x,y);
    R_cont_rel(0, (int)(D_ns*(.4)));
    R_move_abs(x,y);
    R_cont_rel((int)(D_ew*(-.4)),0);

}

arrow_nw()
{
    extern double D_ew, D_ns;
    extern int D_x, D_y;
    int x,y;

    x = D_x + (int) (D_ew * .2);
    y = D_y + (int) (D_ns * .2);
    R_move_abs(x,y);
    R_cont_rel((int)(D_ew *(.6)),((int)(D_ns *(.6))));
    R_move_abs(x,y);
    R_cont_rel(0, (int)(D_ns*(.4)));
    R_move_abs(x,y);
    R_cont_rel((int)(D_ew*(.4)),0);

}

arrow_sw()
{
    extern double D_ew, D_ns;
    extern int D_x, D_y;
    int x,y;

    x = D_x + (int) (D_ew * .2);
    y = D_y + (int) (D_ns * .8);
    R_move_abs(x,y);
    R_cont_rel((int)(D_ew *(.6)),((int)(D_ns *(-.6))));
    R_move_abs(x,y);
    R_cont_rel(0, (int)(D_ns*(-.4)));
    R_move_abs(x,y);
    R_cont_rel((int)(D_ew*(.4)),0);

}
arrow_e()
{
    extern double D_ew, D_ns;
    extern int D_x, D_y;
    int x,y;

    x = D_x + (int) (D_ew * .9);
    y = D_y + (int) (D_ns * .5);
    R_move_abs(x,y);
    R_cont_rel((int)(D_ew *(-.8)), 0);
    R_move_abs(x,y);
    R_cont_rel((int)(D_ew*(-.3)), (int)(D_ns*(-.3)));
    R_move_abs(x,y);
    R_cont_rel((int)(D_ew*(-.3)),(int)(D_ns*(.3)));

}
arrow_w()
{
    extern double D_ew, D_ns;
    extern int D_x, D_y;
    int x,y;

    x = D_x + (int) (D_ew * .1);
    y = D_y + (int) (D_ns * .5);
    R_move_abs(x,y);
    R_cont_rel((int)(D_ew *(.8)), 0);
    R_move_abs(x,y);
    R_cont_rel((int)(D_ew*(.3)), (int)(D_ns*(-.3)));
    R_move_abs(x,y);
    R_cont_rel((int)(D_ew*(.3)),(int)(D_ns*(.3)));

}
arrow_s()
{
    extern double D_ew, D_ns;
    extern int D_x, D_y;
    int x,y;

    x = D_x + (int) (D_ew * .5);
    y = D_y + (int) (D_ns * .9);
    R_move_abs(x,y);
    R_cont_rel(0, (int)(D_ns *(-.8)));
    R_move_abs(x,y);
    R_cont_rel((int)(D_ew*(.3)), (int)(D_ns*(-.3)));
    R_move_abs(x,y);
    R_cont_rel((int)(D_ew*(-.3)),(int)(D_ns*(-.3)));

}
arrow_n()
{
    extern double D_ew, D_ns;
    extern int D_x, D_y;
    int x,y;

    x = D_x + (int) (D_ew * .5);
    y = D_y + (int) (D_ns * .1);
    R_move_abs(x,y);
    R_cont_rel(0, (int)(D_ns *(.8)));
    R_move_abs(x,y);
    R_cont_rel((int)(D_ew*(.3)), (int)(D_ns*(.3)));
    R_move_abs(x,y);
    R_cont_rel((int)(D_ew*(-.3)),(int)(D_ns*(.3)));

}
draw_x()
{
    extern double D_ew, D_ns;
    extern int D_x, D_y;
    int x,y;

    x = D_x;
    y = D_y;
    R_move_abs(x,y);
    R_cont_rel((int)D_ew, (int)D_ns );
    y = D_y + (int)D_ns;
    R_move_abs(x,y);
    R_cont_rel((int)D_ew, (int)(D_ns * -1));
}
unknown_()
{
    extern double D_ew, D_ns;
    extern int D_x, D_y;
    int x,y;

    x = D_x + (int) (D_ew * .3);
    y = D_y + (int) (D_ns * .4);
    R_move_abs(x,y);
    R_cont_rel(0, (int)(D_ns *(-.15)));
    R_cont_rel((int)(D_ew *(.1)), (int)(D_ns *(-.1)));
    R_cont_rel((int)(D_ew *(.2)), 0);
    R_cont_rel((int)(D_ew *(.1)), (int)(D_ns *(.1)));
    R_cont_rel(0, (int)(D_ns *(.2)));
    R_cont_rel((int)(D_ew *(-.1)), (int)(D_ns *(.1)));
    R_cont_rel((int)(D_ew *(-.1)), 0);
    R_cont_rel(0, (int)(D_ns *(.25)));
    R_move_rel(0, (int)(D_ns *(.1)));
    R_cont_rel(0, (int)(D_ns *(.1)));
}
