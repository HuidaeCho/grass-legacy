/**** main.c ****/
/*
**  $Id$
**  Written by David Gerdes  Fall 1992
**  US Army Construction Engineering Research Lab
**  Copyright  David Gerdes  USA-CERL  1992
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gis.h"
#include "Vect.h"
#include "bitmap.h"

#define MAIN_T
#include "parse.h"
#include "cutter.h"

#define DEBUG


struct Map_info Maps[2];
struct Map_info Out;


int output_open = 1;


int main (int argc, char *argv[])
{
	struct GModule *module;
    struct ARGS Args;
    struct Categories *Cat1;
    struct table_base *PTable;
    struct table_base *LTable;
    int newpolys = 0;
    int newlines = 0;
    int tmp;
    char *dmapset;
    char buf[64];


    G_gisinit (argv[0]);

	module = G_define_module();
	module->description =
		"Polygon Cookie Cutter  (Boolean AND Overlay).";

    parse_args (argc, argv, &Args);
    open_files (&Args, Maps, &Out);
    cutter_init (&(Maps[A_CODE]));

    /* Areas */
    if (Do_areas && Maps[B_CODE].n_areas > 0)
    {
	if (!Quiet)
	    fprintf (stderr,  "Building intersection table:   ");
	PTable = build_table (&(Maps[A_CODE]), &(Maps[B_CODE]));

	if (NULL != getenv ("DUMP"))
	{
	    dump_table (PTable->Table);
	}

	if (!Quiet)
	    fprintf (stderr,  "Building new polygons:         ");
	tmp = build_polys (Maps, &Out, PTable->Table);
	if (tmp < 0)
	    exit (-1);
	newpolys = tmp;

	tmp = update_intersect_table (Maps);
	newpolys += tmp;

	/*
	dump_nointersect (Maps);
	*/

	if (!Quiet)
	    fprintf (stderr,  "Tying up loose ends:           ");
	tmp = interior_polys (Maps, &Out, PTable->Table);

	if (tmp > 0)
	    newpolys += tmp;
	destroy_table (PTable);
    }
    


    /* Lines */
    if(Do_lines)
    {
	if (!Quiet)
	    fprintf (stderr,  "Building line intersect table: ");
	LTable = build_line_table (&(Maps[A_CODE]), &(Maps[B_CODE]));

	if (NULL != getenv ("DUMP"))
	{
	    dump_table (LTable->Table);
	}

	if (!Quiet)
	    fprintf (stderr,  "Building new lines:            ");
	tmp = build_lines (Maps, &Out, LTable->Table);
	if (tmp < 0)
	    exit (-1);
	newlines = tmp;

	if (!Quiet)
	    fprintf (stderr,  "Tying up line loose ends:      ");
	tmp = interior_lines (Maps, &Out, LTable->Table);
	if (tmp > 0)
	    newlines += tmp;

	/* tidy up afterwards */
	BM_destroy (intersect_bitmap);
	destroy_table (LTable);
    }


#ifdef FOO    /*	I can't do this accurately it seems  */
    if (!Quiet)
    {
    fprintf (stderr, "\n    %5d polygons %5d lines %5d sites in %s\n", Maps[A_CODE].n_areas, Maps[A_CODE].n_llines, Maps[A_CODE].n_plines, Args.MapA);
    fprintf (stderr,   "    %5d polygons %5d lines %5d sites in %s\n", Maps[B_CODE].n_areas, Maps[B_CODE].n_llines, Maps[B_CODE].n_plines, Args.MapB);
    fprintf (stderr,   "    %5d polygons %5d lines and sites   in %s\n", newpolys, newlines, Args.Out);
    }
#endif


    /* close files */
    Vect_close (&(Maps[A_CODE]));
    Vect_close (&(Maps[B_CODE]));
    if (output_open)
    {
	fclose (Out.att_fp);
	Vect_close (&Out);	
    }

    /* Category labels copied across */

    Cat1 = (struct Categories *)malloc( sizeof(struct Categories) );

    if( (dmapset = G_find_file( "dig_cats", Args.MapB, "" ) ) == NULL )
      G_warning( "Unable to find dig_cats file. No category support available.\n" );
    else {
      G_read_vector_cats( Args.MapB, dmapset, Cat1 );
      G_write_vector_cats( Args.Out, Cat1 );
    }

    if(Do_lines && !Do_areas)
    {
        sprintf(buf,"v.support map=%s",Args.Out);
        system(buf);
    }
    

    exit (0);
}

#ifdef DEBUG
#include <stdarg.h>
int debugf (char *format, ...)
{
    static int first_time = 1;
    static FILE *fp;
    va_list ap;
    char *p;

    if (first_time)
    {
	first_time = 0;
	if (NULL == (p = getenv ("DEBUG")))
	    Debug_on = 0;
	else
	{
	    Debug_on = 1;
	    if (strlen (p))
	    {
		if (NULL == (fp = fopen (p, "w")))
		    fp = stderr;
		else
		    setbuf (fp, NULL);
	    }
	    else
		    fp = stderr;
	}
    }
    if (Debug_on) {
        va_start(ap,format);
	vfprintf (fp, format, ap);
        va_end(ap);
    }

    return 0;
}
#endif
