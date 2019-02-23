#include "gis.h"
#include "options.h"

where_am_i()
{
    char buffer[200] ;
    char buf1[50], buf2[50];
    char temp[100];
    double lat, lon ;
    int screen_x, screen_y ;
    int cur_screen_x, cur_screen_y ;
    double east, north ;
    int button ;
    double D_get_d_north(), D_get_d_south() ;
    double D_get_d_east(), D_get_d_west() ;
    double D_d_to_u_row(), D_d_to_u_col() ;
    int white, black ;
    int projection;

    projection = G_projection();
    white = D_translate_color("white") ;
    black = D_translate_color("black") ;

    screen_x = ((int)D_get_d_west() + (int)D_get_d_east()) / 2 ;
    screen_y = ((int)D_get_d_north() + (int)D_get_d_south()) / 2 ;

    if(mode == LOUD)
    {
	fprintf(stderr, "\n\nButtons:\n") ;
	fprintf(stderr, "Left:   where am i\n") ;
	fprintf(stderr, "Middle: draw to/from here\n") ;
	fprintf(stderr, "Right:  quit this\n\n\n") ;

	if (projection == PROJECTION_LL)
	    fprintf(stderr,"%18s  %18s","LON:","LAT:") ;
	else
	    fprintf(stderr,"%18s  %18s","EAST:","NORTH:") ;
	if (have_spheroid)
	    fprintf(stderr,"  %20s  %20s","LON:","LAT:") ;
	fprintf (stderr, "\n");
    }

    do
    {
	R_get_location_with_pointer(&screen_x, &screen_y, &button) ;
	east = D_d_to_u_col((double)screen_x) ;
	north = D_d_to_u_row((double)screen_y) ;
	G_format_easting  (east,  buf1, projection);
	G_format_northing (north, buf2, projection);
	if (mode == SILENT)
	{
	    printf("%18s  %18s %d", buf1, buf2, button) ;
	    return(0) ;
	}
	sprintf(buffer,"%18s  %18s", buf1, buf2) ;
	if (have_spheroid)
	{
	    CC_u2ll_north (north);
	    CC_u2ll (east, &lat, &lon);
	    CC_lon_format (lon, buf1);
	    CC_lat_format (lat, buf2);
	    sprintf (temp, "  %20s  %20s", buf1, buf2);
	    strcat (buffer, temp);
	}
	show (buffer);
	if(button == 3)
	    return(0) ;
    } while (button != 2) ;

    R_move_abs(screen_x, screen_y) ;
    cur_screen_x = screen_x ;
    cur_screen_y = screen_y ;

    do
    {
	R_get_location_with_line(cur_screen_x, cur_screen_y, &screen_x, &screen_y, &button) ;
	east = D_d_to_u_col((double)screen_x) ;
	north = D_d_to_u_row((double)screen_y) ;
	G_format_easting  (east,  buf1, projection);
	G_format_northing (north, buf2, projection);
	sprintf(buffer,"%18s  %18s", buf1, buf2) ;
	if (have_spheroid)
	{
	    CC_u2ll_north (north);
	    CC_u2ll (east, &lat, &lon);
	    CC_lon_format (lon, buf1);
	    CC_lat_format (lat, buf2);
	    sprintf (temp, "  %20s  %20s", buf1, buf2);
	    strcat (buffer, temp);
	}
	show (buffer);
	if(button == 2)
	{
	    black_and_white_line(black, white, screen_x,screen_y,cur_screen_x,cur_screen_y) ;
	    cur_screen_x = screen_x ;
	    cur_screen_y = screen_y ;
	    R_move_abs(cur_screen_x, cur_screen_y) ;
	}
    } while (button != 3) ;
}

static
show (buf) char *buf;
{
    char *b;

    if (!isatty(1))
	printf ("%s\n", buf);
    for (b = buf; *b; b++)
	fprintf (stderr, "%c", *b);
    for (b = buf; *b; b++)
	fprintf (stderr, "\b");
}
