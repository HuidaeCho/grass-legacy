/*
*
*   GRASS4.0: converted to new parser - 1/91 -- dks
*/
#include <string.h>
#include    <stdio.h>
#include <stdlib.h>
#include    "gis.h"
#include "Vect.h"
#include    "vbuildlib.h"
/*
#include    "dig_structs.h"
*/
#include    "digit.h"
#include    "dig_head.h"
#include    "local_proto.h"

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


double dig_unit_conversion ();
static	int   snapped = 0 ;
static int ident_only = 0;

int main (int argc, char **argv)
{
    double    val;
    char buf[1024];
    int tot_atts;

    FILE    *fp_plus ;

    struct Map_info Map ;
    struct Plus_head Plus ;
    char *file_name;


#ifdef RAM
    struct Flag *ram_flag;
#endif /*RAM*/
   struct Option *map, *s_val;
   struct Flag *ident, *s_flag, *p_flag;


    setbuf(stdout, 0) ;

/* Show advertising */
    G_gisinit(argv[0]) ;

/*****************************COMMAND PARSER******************************/
    map = G_define_option();
    map->key			= "map";
    map->type			= TYPE_STRING;
    map->required		= YES;
    map->multiple		= NO;
    map->gisprompt		= "old,dig,vector";
    map->description		= "vector file name";

    
    s_val = G_define_option();
    s_val->key			= "threshold";
    s_val->type			= TYPE_DOUBLE;
    s_val->required		= NO;
    s_val->multiple		= NO;
    s_val->description		= "snap threshold value";

    ident = G_define_flag ();
    ident->key 			= 'i';
    ident->description 		= "Only run identical line tests";

    s_flag = G_define_flag();
    s_flag->key = 's';
    s_flag->description = "Snap nodes"; 

    p_flag = G_define_flag();
    p_flag->key = 'p';
    p_flag->description = "Prompt user for threshold"; 


    if (G_parser (argc, argv))
	exit (-1);

    file_name = map->answer;

    if (snap_ok = s_flag->answer)
    {
	if (s_val->answer != NULL)
	    snap_val = atof(s_val->answer);
	thresh_flag = p_flag->answer;
    }
    else
    {
	snap_val = 0.0;
	thresh_flag = 0;
    }

    ident_only = ident->answer == 0 ? 0 : 1;

    if ( !file_name || !*file_name )
    {
        fprintf (stderr, "%s: Command line error: missing map name.\n\n", argv[0]);
	G_usage();
        exit (-1);
    }
/*****************************COMMAND PARSER******************************/
/*  init Map structure and show header info  */
    init_plus_struct(&Plus) ;
    init_map_struct(&Map) ;


/*  open dig and dig plus file  */
    if ( open_dig_files( file_name, &fp_plus, &Map, &Plus ) )
   	 exit(-1) ;

    fprintf (stdout,"\nSelected information from vector header\n") ;
    fprintf (stdout," Organization:  %s\n", Map.head.organization) ;
    fprintf (stdout," Map Name:      %s\n", Map.head.map_name) ;
    fprintf (stdout," Source Date:   %s\n", Map.head.source_date) ;
    fprintf (stdout," Orig. Scale:   %ld\n", Map.head.orig_scale) ;



    {
#ifdef DEBUG
    debugf ("MAIN:scale %d  unit %f\n", Map.head.orig_scale, dig_unit_conversion ());
#endif
	Map.snap_thresh = THRESH_FUDGE * Map.head.orig_scale * dig_unit_conversion();
#ifdef DEBUG
    debugf ("MAIN:  snap_thresh %f\n", Map.snap_thresh);
#endif
	Map.head.map_thresh = Map.snap_thresh;
    }
    
    if (snap_ok)
    {
	if (snap_val == 0.0)
	    Map.snap_thresh = Map.head.map_thresh; 
	else
	    Map.snap_thresh = snap_val; 
    }
    else
	Map.snap_thresh = 0.0;

    if (thresh_flag)
    {
	fprintf (stdout,"Enter snapping threshold [default=%7.2f]: ", Map.snap_thresh);
	fflush (stdout);
	fgets (buf,1024,stdin); 

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
	fprintf (stdout," No snapping will be done \n");
    else
	fprintf (stdout," Snapping threshold %7.2f\n\n", Map.snap_thresh) ;

    fprintf (stdout, "\n");
    fprintf (stdout, "    Reading Vector file.\n");

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
#ifdef MEMORY_IO	/* not yet upgraded for 4.0 */
#ifndef RAM_OFF
    if(RAM_OK && Map.n_alines && 
      (Map.n_alines / (Map.n_llines+.5) > 1.5 || Map.n_lines < 2000))
    if (dig_Mem_load_file (Map.digit, &memptr) >= 0)
        fprintf (stdout,"    Using RAM file\n");
#endif
#endif
/****************************************/


/****************************************/
/*                      		*/
/*Here it is:           		*/
/*            VVVVVVVVVV		*/
/****************************************/
    init_extents (); /* TEST.. was commened out let v.build do this*/
    snapped = read_digit( &Map, &Plus);
    if (snapped < 0)
	fprintf (stderr, "Could not build support (dig_plus) file.\n"), exit (-1);

#ifdef FROM_BUILD
    fprintf (stdout,"    Building areas\n");
    build_all_areas (&Map);

    if (do_islands) /*ISLE*/
    {
	fprintf (stdout,"    Building islands\n");
	matchup_isles (&Map); /*ISLE*/
    }

    fprintf (stdout,"    Attaching labels\n");
    clean_atts_file (Map.att_file);
    tot_atts = read_atts (&Map, Map.att_file);
#endif
    /* TODO: try cleaning before and after for now */
    if (!ident_only)
    {
	fprintf (stdout,"    Cleaning lines less than thresh\n");
	clean_lines (&Map, Map.snap_thresh);
    }

    fprintf (stdout, "Identical lines check");
    if (!ident_only)
	fprintf (stdout,"  and   Line intersections");
    fprintf (stdout, "\n");

    intersect (&Map, ident_only);

    if (!ident_only)
    {
	clean_lines (&Map, Map.snap_thresh);
    }

    compress (&Map, 1);

    fprintf (stdout,"    Building areas\n");
    build_all_areas (&Map);

    if (do_islands) /*ISLE*/
    {
        fprintf (stdout,"    Building islands\n");
        matchup_isles (&Map); /*ISLE*/
    }

    fprintf (stdout,"    Attaching labels\n");
    clean_atts_file (Map.att_file);
    tot_atts = read_atts (&Map, Map.att_file);



/****************************************/
    /* release memory file */
    /* this only has effect if "Using RAM file" */

#ifdef MEMORY_IO	/* not yet upgraded for 4.0 */
    dig_Mem_release_file ();
#endif
/****************************************/

    fprintf (stdout," Number of lines:   %d\n", Map.n_lines) ;
    fprintf (stdout," Number of nodes:   %d\n", Map.n_nodes) ;
    fprintf (stdout," Number of areas:   %d\n", Map.n_areas) ;
    fprintf (stdout," Number of isles:   %d\n", Map.n_isles) ;
    fprintf (stdout," Number of atts :   %d\n", Map.n_atts) ;

    fprintf (stdout," Number of unattached atts :   %d\n", tot_atts-Map.n_atts) ;
    fprintf (stdout," Snapped lines  :   %d\n", snapped) ;

/*==========================================*/
    if (!ident_only)
      fprintf (stdout,"WARNING: Beta version. You must run v.support before using this data\n");
/*==========================================*/

/*  write out all the accumulated info to the plus file  */
    Plus.all_areas = 1;
    if (do_islands) /*ISLE*/
	Plus.all_isles = 1;
    else
	Plus.all_isles = 0; 


/*==========================================*/
/* TODO:  For now make this file unuseable without first running v.support */

    if (!ident_only)	/* for ident_only, is ok */
    {
	Plus.all_areas = 0;
	Plus.all_isles = 0;
    }

/*==========================================*/
    dig_map_to_head (&Map, &Plus);

/*  clean up files  */

    fflush(Map.dig_fp) ;
    fflush(Map.att_fp) ;

    if (0 > dig_write_plus_file (fp_plus, &Map, &Plus))
    fprintf (stderr, "Error writing out support (dig_plus) file.\n"), exit (-1);

    /*  Note, not using the library to update head, and close files */


    /* Re-write out the header w/ the possibly changed window */
    /* 4.0  dpg */
    /*Vect__read_head_binary  (&Map, &(Map.head));*/
    update_head_from_ext (&(Map.head));
    Vect__write_head_binary (&Map, &(Map.head));

    fclose(Map.dig_fp) ;
    fclose(Map.att_fp) ;


    fclose(fp_plus) ;

    exit (0);
}

#include <stdarg.h>
int debugf (char *format, ...)
{
    va_list a;

    va_start(a,format);
    vfprintf (stderr, format, a);
    va_end(a);

    return 0;
}
