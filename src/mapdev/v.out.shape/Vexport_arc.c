/*
 * $Id$
 * v.out.shape by Markus Neteler (no, most of the work was done by others,
 *                                see below:)
 * neteler@geog.uni-hannover.de 
 *
 * based on - v.out.arc by Dave Johnson, R.L.Glenn, David Stigberg
 *          - gen2shp  by Jan-Oliver Wagner <jan@intevation.de>
 *          - txt2dbf by Frank Koormann <Frank.Koormann@intevation.de>
 *          - shapelib by Frank Warmerdam <warmerda@home.com>
 *
 * http://www.usf.uos.de/~jwagner/gen2shp/gen2shp.html
 * http://gdal.velocet.ca/projects/shapelib/index.html
 */

/* based on Vexport_arc.c 1.0 04/90
**  Written by  Dave Johnson
**  DBA Systems, Inc.
**
**  modified by R.L.Glenn
**  USDA, Soil COnservation Service, CGIS Staff
**
**
**  modified by David Stigberg
**  USA-CERL
*/

#define MAIN
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "gis.h"
#include "Vect.h"
#include "v_out_arc.h"
#include "gtoa.h"

#define  USAGE  "v.out.shape type=name vect=name shape_prefix=name\n"


#define POLY_TYPE        1
#define LINE_TYPE        0

/*  command line args */
static	char  *cov_type = NULL ;
static	char  *dig_name = NULL ;
static	char  *shape_prefix = NULL ;
static  char  *shape_type= NULL ;
static  char  *extension= NULL ;

#ifdef OLDPARSE
static  int  load_args() ;

#endif /*OLDPARSE*/

int 
main (int argc, char **argv)
{
	char *progname;
	/*struct dig_head head;*/
	struct Map_info Map;
	int	done=0, ret, coverage;
	int level;
	char    full_prefix[1000],
		command[200],
		errmsg[200],
		*mapset,
		name[1000],
		dig_filepath[1000],
	    dig_filename[1000],
	    lin_filename[100],
	    lab_filename[100],
	    txt_filename[100],
	    lin_filepath[1000],
	    lab_filepath[1000],
	    txt_filepath[1000];
	char buf[10000];
  
	FILE
			*pol_file,
			*lin_file,
			*lab_file,
			*txt_file;

	struct Option *opt1, *opt2, *opt3, *opt4;

	G_gisinit(argv[0]);
	progname = G_program_name();

	/* Define the different options */

	opt1 = G_define_option() ;
	opt1->key         = "type";
	opt1->type        = TYPE_STRING;
	opt1->options     = "polygon,line";
	opt1->required    = YES;
	opt1->description = "coverage type";

	opt2 = G_define_option() ;
	opt2->key        = "vect";
	opt2->type       = TYPE_STRING;
	opt2->required   = YES;
	opt2->description= "input vector file to convert to SHAPE format";
	opt2->gisprompt  ="old,dig,vector";

	opt3 = G_define_option() ;
	opt3->key        = "shape_prefix";
	opt3->type       = TYPE_STRING;
	opt3->required   = YES;
	opt3->description= "prefix for SHAPE filenames";

/* not required for SHAPE:*/
/*	opt4 = G_define_option() ;
	opt4->key        = "separator";
	opt4->type       = TYPE_STRING;
	opt4->required   = NO;
	opt4->description= "field separator";
	opt4->answer     = "space";
*/	
	/*global flags, zero until respective files are actually written to*/
	pol_flg = 0; /*apparent BUG--.pol suffix never implemented*/
	lin_flg = 0;
	lab_flg = 0;
	txt_flg = 0;


	if (G_parser (argc, argv))
	{
		exit (-1);
	}

	/*set coverage: polygon vs. line*/

	if (strcmp (opt1->answer, "polygon") == 0)
			coverage = POLY_TYPE;
	else
	{
		if (strcmp (opt1->answer, "line") == 0)
			coverage = LINE_TYPE;
		else
		{
			sprintf (errmsg,
			"%100s: Must specify POLYGON or LINE coverage type.\n",
					progname);
			G_fatal_error (errmsg);
		}
    }

	fprintf(stderr, "%s: Coverage type = %s\n", progname, coverage ? "polygon":"line");

	dig_name = opt2->answer;
	shape_prefix = opt3->answer;

	/*verify that required filenames are there*/
	if (!*dig_name  || !*shape_prefix)
	{
		fprintf (stderr, "\n\n%s: Command line error: missing vector input name or ARC/INFO prefix.\n\n", progname);
		G_usage();
		exit (-1);
	}

/* not required for SHAPE:*/
/*	if ((opt4->answer == NULL)||(strcmp(opt4->answer, "space")==0))
	{
	   separator = ' ';
	   space = 1;
	}
	else 
	{
	   if (strlen(opt4->answer) > 1) 
	       G_fatal_error("A separator field must consist of one character only!");
	   else      separator = *(opt4->answer);
        }
*/
	separator = ' ';
	space = 1;
	
	if ((mapset = G_find_vector2 (dig_name, "")) == NULL)
	{
		sprintf (errmsg, "Could not find vector file <%s>\n", dig_name);
		G_fatal_error (errmsg);
	}


/* Do initial read of DIGIT file */
    fprintf (stdout,"\nLoading vector information.\n");


    level = Vect_open_old (&Map, dig_name, mapset);

    if (level < 2)
        G_fatal_error ("You must run v.support before running this program");

	G__make_mapset_element("arc_tmp") ;
	G__file_name(full_prefix, "arc_tmp", shape_prefix, G_mapset()) ;


	strcpy(lin_filename,shape_prefix);
	strcpy(lin_filepath,full_prefix);
	if (coverage == POLY_TYPE) /*if POLY, line file is prefix.pol--dks*/
	{
	    strcat(lin_filename,".pol");
	    strcat(lin_filepath,".pol");
	}
    else                        /*if LINE, file is prefix.lin*/
	{
	    strcat(lin_filename,".lin");
	    strcat(lin_filepath,".lin");
	}

	strcpy(lab_filename,shape_prefix);
	strcat(lab_filename,".lab");
	strcpy(lab_filepath,full_prefix);
	strcat(lab_filepath,".lab");

	strcpy(txt_filename,shape_prefix);
	strcat(txt_filename,".txt");
	strcpy(txt_filepath,full_prefix);
	strcat(txt_filepath,".txt");

	if ( (lin_file = G_fopen_new("arc_tmp", lin_filename )) == NULL )
	{
		sprintf(errmsg, "Cannot open ARC/INFO lines file <%s>\n", lin_filename) ;
		G_fatal_error (errmsg);
	}
	if ( (lab_file = G_fopen_new("arc_tmp", lab_filename )) == NULL )
	{
		sprintf(errmsg,"Can't open ARC/INFO label-points file <%s>\n",lab_filename);
		G_fatal_error (errmsg);
	}
	if ( (txt_file = G_fopen_new("arc_tmp", txt_filename )) == NULL )
	{
		sprintf(errmsg, "Cannot open ARC/INFO label-text file <%s>\n", txt_filename) ;
		G_fatal_error (errmsg);
	}
	
	if (coverage == POLY_TYPE)
	{
		fprintf (stderr,"ARC Area/Polygon data being created\n");
		write_areas(dig_name,mapset,&Map,lin_file,lab_file,txt_file);
		G__file_name(dig_filepath,"dig",dig_name,mapset);
		write_area_lines(&Map, lin_file); 
		lin_flg=1;
	}
	else
	{
		fprintf (stderr,"ARC Line data being created\n");
		write_lines(dig_name,mapset,&Map,lin_file,txt_file);
	}
	Vect_close (&Map);

	/* delete new files not written to */

	if (!lin_flg)
	{
			unlink (lin_filepath);
	}
	if (!lab_flg)
	{
		unlink (lab_filepath);
	}
	if (!txt_flg)
	{
		unlink (txt_filepath);
	}

	fclose(lin_file);
	fclose(lab_file);
	fclose(txt_file);

/* new v.out.shape stuff MN 3/2000 */
/* usage: 
 * gen2shp outfile type < infile
 *  reads stdin and creates outfile.shp, outfile.shx
 *  type must be one of these: points lines polygons
 *  infile must be in 'generate' format
 *
 *  txt2dbf creates output.dbf with attributes
 *
 */
	if (strcmp (opt1->answer, "polygon") == 0)
	{
			shape_type = "polygons";
			extension = "pol";
	}
	else
	{
		if (strcmp (opt1->answer, "line") == 0)
		{
			shape_type = "lines";
			extension = "lin";
		}
	}
	/* do we need points ??: see v.out.idrisi for implementation */

	fprintf(stdout, "Converting $LOCATION/arc_tmp/%s.%s to SHAPE file...\n", shape_prefix, extension);
	sprintf(buf, "$GISBASE/etc/v.out.shape/gen2shp %s %s < $LOCATION/arc_tmp/%s.%s", shape_prefix, shape_type, shape_prefix, extension);
	G_system(buf);

	fprintf(stdout, "Converting attributes to DBF file...\n");
	sprintf(buf, "wc $LOCATION/arc_tmp/%s.txt ; $GISBASE/etc/v.out.shape/txt2dbf -v -I6 -I6 -I6 -C80 -d' ' -U -F'ID CAT ID2 ATT' $LOCATION/arc_tmp/%s.txt %s.dbf ; m -rf  $LOCATION/arc_tmp/ ", shape_prefix, shape_prefix, shape_prefix);
	fprintf(stderr,"%s\n",buf);
	G_system(buf);

	exit(0);
}

#ifdef OLD_PARSER
static 
load_args (int position, char *str)
{
	double atof ();

	    switch(position)
	    {
	case 1:
		if (strcmp(str,"area")==0) cov_type = G_store("polygon") ;
		    if (strcmp(str,"line")==0) cov_type = G_store("line") ;
		    break ;
	    case 2:
		dig_name = G_store(str) ;
		    break ;
	    case 3:
		arc_name = G_store(str) ;
		    break ;
	    default:
		break;
	}	/*  switch  */

	return (0);
}
#endif /*OLD_PARSER*/

