/*
*  Written by GRASS, Fall of 88,  Michael H.
*/

#include	<stdio.h>
#include	"gis.h"
#include        "Vect.h"
#include	"grid_structs.h"
#include	"local_proto.h"

/**  data directories   **/
#define		B_DIG		"dig"

static char  *PROG ;

int main (int argc, char *argv[])
{

	/*  store filename and path  */
	char  dig_file[128] ;
	char  buffer[128] ;
	char errmsg[1024];
	char buf[256];
	int quiet = 0;

        struct Map_info Map;
	struct  grid_description  grid_info ;
	struct Cell_head window ;
	struct dig_head d_head;

	PROG = argv[0] ;
	G_gisinit(argv[0]) ;
	/*system("clear") ;*/
	setbuf(stdout, NULL) ;

    /*  get the current window  */
	G_get_window(&window) ;


    /*  make sure dig directory is there  */
	G__make_mapset_element( B_DIG) ;

/*  information we need to collect from user: origin point x and y (lower left),
*   shift in x, shift in y,  number of rows, number of cols
*/

	welcome_mat() ;

	ask_for_name( dig_file, " VECTOR FILENAME ", buffer,
		B_DIG, "binary vector") ;

        if (!*dig_file || (G_legal_filename (dig_file) != 1))
        {
          fprintf (stderr, "%s: Command line error. missing or illegal map name.\n\n", argv[0]);
          G_usage ();
          exit (-1);
        }

	ask_for_int ( &grid_info.num_rows, "\n Enter the number of rows: ") ;
	ask_for_int ( &grid_info.num_cols, " Enter the number of columns: ") ;

	fprintf (stdout,"\n\n Enter the lower left easting (x) of the grid: ");
        while(1)
	{
	    if (fgets (buf,256,stdin) == NULL)
 	    {
		clearerr (stdin) ;
		exit (1) ;
	    }

            if(!G_scan_easting(buf, &grid_info.origin_x, window.proj))
	       fprintf (stdout,"\n Illegal easting! Try again: ") ;
            else break;
        }
	fprintf (stdout,"\n\n Enter the lower left northing (y) of the grid: ");
        while(1)
	{
	    if (fgets (buf,256,stdin) == NULL)
 	    {
		clearerr (stdin) ;
		exit (1) ;
	    }

            if(!G_scan_northing(buf, &grid_info.origin_y, window.proj))
	       fprintf (stdout,"\n Illegal northing! Try again: ") ;
            else break;
        }

	fprintf (stdout,"\n\n Enter the width of a box in the grid (distance to the east): " ) ;
        while(1)
	{
	    if (fgets (buf,256,stdin) == NULL)
 	    {
		clearerr (stdin) ;
		exit (1) ;
	    }

            if(!G_scan_resolution(buf, &grid_info.length, window.proj))
	       fprintf (stdout,"\n Illegal distance! Try again: ") ;
            else break;
        }
	fprintf (stdout,"\n\n Enter the height of a box in the grid (distance to the north): " ) ;
        while(1)
	{
	    if (fgets (buf,256,stdin) == NULL)
 	    {
		clearerr (stdin) ;
		exit (1) ;
	    }

            if(!G_scan_resolution(buf, &grid_info.width, window.proj))
	       fprintf (stdout,"\n Illegal distance! Try again: ") ;
            else break;
        }
	ask_for_double ( &grid_info.angle, "\n Enter the angle of rotation about the origin: " ) ;
	grid_info.angle = 3.1415927 / 180 * grid_info.angle;

        if (0 > Vect_open_new (&Map, dig_file))
        {
           sprintf (errmsg, " %s: Cannot open vector output file <%s>\n", PROG, dig_file);
	   G_fatal_error (errmsg);
	 }

	if (G_yes("Run quet?",0) ) quiet =1;

    /*  vector rows are the actual number of rows of vectors to make up the
    *   entire grid.   ditto for cols.
    */
	grid_info.num_vect_rows =  grid_info.num_rows + 1 ;
	grid_info.num_vect_cols =  grid_info.num_cols + 1 ;

    /*  initialize and write the digit vector header  */
	fprintf (stdout,"\n Creating vector header...") ;
	init_header ( &window, &d_head) ;

	fprintf (stdout,"\n Creating vector grid...") ;
	Vect_copy_head_data (&d_head, &Map.head);
	write_grid (&grid_info , &Map, quiet) ;
	if(!quiet) fprintf (stdout,"\n Finished\n\n") ;

        Vect_close(&Map);

	exit(0);
}		/*  main()  */
