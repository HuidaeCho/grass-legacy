/* %W% %G% */
#include    <stdio.h>
#include    "gis.h"
/*
#include    "dig_structs.h"
*/
#include    "digit.h"
#include    "dig_head.h"

/*  will install this after fixing  digit code to be able to handle
*   negative areas (islands)
#define ISLANDS 1
*/

#define MAIN
#define METERS_PER_INCH 0.0254

#define  USAGE  "mapset  file_name snap=[yes, no] ram=[yes, no]"

/*
#define DEBUG
*/
/* still have to add the code to rebuild att file */
/* to guarantee that it is in our fixed field format */

/*  command line args and flags  */
/*  defaults are RAM off and snapping off  */
static  int   RAM_OK = 0 ;
static	int   snap_ok = 0 ;
static	double   snap_val = 0.0 ;
static	int   thresh_flag = 0 ;
static	char  *mapset = NULL ;
static	char  *name = NULL ;
static  int do_islands = 1; /*ISLE*/

static  int  load_args() ;

struct Command_keys vars[] = {
 { "mapset", 1 },
 { "name", 2 },
 { "snap", 3 },
 { "ram", 4 },
 { "ramfile", 4 },
 { "thresh", 5 },
#ifdef ISLANDS
 { "islands", 6 },
 { "isles", 6 },
#endif ISLANDS
 { NULL,     0 }
};

double dig_unit_conversion ();
static	int   snapped = 0 ;

main(argc, argv)
    int argc;
    char **argv;
{
    int   status ;
    int   ret ;
    double     atof ();
    double    val;
    char buf[1024];
    int tot_atts;

    FILE    *fp_plus ;

    struct Map_info Map ;
    struct Plus_head Plus ;
    char *memptr;        /* for load_file () */

/*
*  Usage:  dig.import  mapset  file_name snap=["yes", "no"] ram=["yes", "no"]
*    snap:    "no" to leave nodes as they are,
*             "yes" to snap nodes
*    ram:     "no" to read/write strictly from file
*             "yes" read everything into memory
*/


/*  check args and set flags  */
    ret = G_parse_command (argc, argv, vars, load_args) ;
    if (ret > 0)	/* Help was requested */
         exit (1);

    if (ret < 0  ||  *mapset == NULL  ||   *name == NULL)
    {
        fprintf (stderr, "%s: Command line error.\n\n Usage: %s\n",
		argv[0], USAGE);
        exit (-1);
    }

    setbuf(stdout, 0) ;

/* Show advertising */
    G_gisinit("CREATE DIG PLUS FILE");
    printf("\n\n   SUPPORT.VECT:\n\n") ;

/*  open dig and dig plus file  */
    if ( open_dig_files( mapset, name, &fp_plus, &Map, &Plus ) )
   	 exit(-1) ;

/* Read dig header info */
    status = dig_read_head_binary(Map.digit, &head) ;
    if (status == -1)
    	fprintf(stderr, "Problem in initial read of vector file\n"), exit(-1) ;

/*  init Map structure and show header info  */
    init_plus_struct(&Plus) ;
    init_map_struct(&Map) ;

    printf("\nSelected information from vector header\n") ;
    printf(" Organization:  %s\n", head.organization) ;
    printf(" Map Name:      %s\n", head.map_name) ;
    printf(" Source Date:   %s\n", head.source_date) ;
    printf(" Orig. Scale:   %ld\n", head.orig_scale) ;


/*  this should be done somewhere else  */

    /*
    if( head.map_thresh != 0.0)
	Map.snap_thresh = head.map_thresh ;
    else
    */
    {
/*DEBUG*/ debugf ("MAIN:scale %d  unit %lf\n", head.orig_scale, dig_unit_conversion ());
	Map.snap_thresh = THRESH_FUDGE * head.orig_scale * dig_unit_conversion();
/*DEBUG*/ debugf ("MAIN:  snap_thresh %lf\n", Map.snap_thresh);
	head.map_thresh = Map.snap_thresh;
    }
    
    if (snap_ok)
    {
	if (snap_val == 0.0)
	    Map.snap_thresh = head.map_thresh; 
	else
	    Map.snap_thresh = snap_val; 
    }
    else
	Map.snap_thresh = 0.0;

    if (thresh_flag)
    {
	printf ("Enter snapping threshold [default=%7.2lf]: ", Map.snap_thresh);
	fflush (stdout);
	gets (buf); 

	dig_rmcr (buf);  G_squeeze (buf);
	if (strlen (buf) && (val = atof (buf)) > 0.0)
	{
	    Map.snap_thresh = val;
	    /*
	    head.map_thresh = Map.snap_thresh;
	    */
	}
    }


    if (Map.snap_thresh == 0.0)
	printf (" No snapping will be done \n");
    else
	printf (" Snapping threshold %7.2lf\n\n", Map.snap_thresh) ;

    printf ( "\n");
    printf ( "    Reading Vector file.\n");
    /*
    snapped = read_digit( &Map, &Plus);
    if (snapped < 0)
	fprintf (stderr, "Could not build support (dig_plus) file.\n"), exit (-1);
    fflush (Map.digit);
    */

/****************************************/
    /* attempt to load dig file into memory for next step */
    /* if this is not desired just comment out the load and release parts*/
    /* if malloc fails, the program will continue on working with the file */


    /* check to see if it is efficient to load entire file into memory.
    ** This needs to be tuned.  Our concerns are: 
    **        Are there any area lines?
    **        Area there many more line lines than area lines?
    **        Is the file too big?
    **        If the file is small, lets just do it
    */
#ifndef RAM_OFF
    if(RAM_OK && Map.n_alines && 
      (Map.n_alines / (Map.n_llines+.5) > 1.5 || Map.n_lines < 2000))
    if (dig_Mem_load_file (Map.digit, &memptr) >= 0)
        printf ("    Using RAM file\n");
#endif
/****************************************/
    snapped = read_digit( &Map, &Plus);
    if (snapped < 0)
	fprintf (stderr, "Could not build support (dig_plus) file.\n"), exit (-1);

    /*
    printf ("    Spaghetti Cleanup\n");
    spagetti (&Map, Map.snap_thresh);
    */
    printf ("    Line intersections\n");
    intersect (&Map);
    printf ("    Building areas\n");
    build_all_areas (&Map);

    if (do_islands) /*ISLE*/
    {
	printf ("    Building islands\n");
	matchup_isles (&Map); /*ISLE*/
    }

    printf ("    Attaching labels\n");
    clean_atts_file (Map.att_file);
    tot_atts = read_atts (&Map, Map.att_file);

/****************************************/
    /* release memory file */
    /* this only has effect if "Using RAM file" */

    dig_Mem_release_file ();
/****************************************/

    printf (" Number of lines:   %d\n", Map.n_lines) ;
    printf (" Number of nodes:   %d\n", Map.n_nodes) ;
    printf (" Number of areas:   %d\n", Map.n_areas) ;
    printf (" Number of isles:   %d\n", Map.n_isles) ;
    printf (" Number of atts :   %d\n", Map.n_atts) ;

    printf (" Number of unattached atts :   %d\n", tot_atts-Map.n_atts) ;
    printf (" Snapped lines  :   %d\n", snapped) ;

/*  write out all the accumulated info to the plus file  */
    Plus.all_areas = 1;
    if (do_islands) /*ISLE*/
	Plus.all_isles = 1;
    else
	Plus.all_isles = 0; 
    dig_map_to_head (&Map, &Plus);

/*  clean up files  */
    fclose(Map.digit) ;
    fclose(Map.att) ;

    if (0 > dig_write_plus_file (fp_plus, &Map, &Plus))
    fprintf (stderr, "Error writing out support (dig_plus) file.\n"), exit (-1);

    fclose(fp_plus) ;
}


static
load_args (position, str)
    int position;
    char *str;
{
    double atof ();

    switch(position)
    {
	case 1:
		mapset = G_store(str) ;
		break ;
	case 2:
		name = G_store(str) ;
		break ;
	case 3:
		if ( strcmp("no", str) == 0 || strcmp("n", str) == 0)
		{
		    snap_ok = 0;
		    snap_val = 0.0;
		}
		else
		    if ( strcmp("yes", str) == 0 || strcmp("y", str) == 0)
		    {
			snap_ok = 1 ;
		    }
		else
		    {
			snap_ok = 1 ;
			snap_val = atof (str);
		    }
		break ;
	case 4:
		if ( strcmp("yes", str) == 0 || strcmp("y", str) == 0)
		    RAM_OK = 1 ;
		else
		    RAM_OK = 0;
		break ;
	case 5:
		if ( strcmp("yes", str) == 0 || strcmp("y", str) == 0)
			thresh_flag  = 1 ;
		break ;
#ifdef ISLANDS
	case 6:
		if ( strcmp("yes", str) == 0 || strcmp("y", str) == 0)
			do_islands  = 1 ;
		break ;
#endif
    }	/*  switch  */

    return (0);
}

#ifdef DEBUG
debugf (format, a, b, c, d, e, f, g, h, i, j, k, l)
    char *format;
    int a, b, c, d, e, f, g, h, i, j, k, l;
{
    fprintf (stderr, format, a, b, c, d, e, f, g, h, i, j, k, l);
}
#endif
