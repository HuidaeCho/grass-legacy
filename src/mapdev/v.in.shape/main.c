/******************************************************************************
 * main.c [v.in.shape]
 * Import ESRI Shapefile.
 * 

 * @Copyright David D.Gray <ddgray@armadce.demon.co.uk>
 * 26th. Feb. 2002
 * Last updated 9th. Mar. 2002
 *

 * This file is part of GRASS GIS. It is free software. You can 
 * redistribute it and/or modify it under the terms of 
 * the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gis.h"
#include "import.h"
#include "basename.h"
#include "shapefil.h"
#include "vmap_import.h"

int main(int argc, char *argv[]) {

  struct Option *iput, *oput, *lfile, *verbose;
  struct Option *snapd, *mscale, *attribute, *catlabel;
  struct Flag *oflag, *fleflag, *uflag, *listflag;
  struct GModule *module;

  param_import_ctrl *ctr1;

  char src_file[512] = "", log_file[512] = "", vmap[512] = "", base0[512] = "";
  char shp_extension[64] = "shp:shx:dbf";
  char *ms0;

  int print_to_error = 0, ires = 0, verbosity = 1, do_overwrite = 0, force_le = 0;
  int u_val = 0;

  FILE *lf;
  DBFHandle   hDBF;

  int scale1;
  double snap1, sliver1;

  G_gisinit(argv[0]);

  module = G_define_module();
  module->description = "Test routine for developing MIF import.";

  iput = G_define_option() ;
  iput->key        = "input";
  iput->type       = TYPE_STRING;
  iput->required   = YES;
  iput->description= "Name of file to be imported";

  oput = G_define_option() ;
  oput->key        = "output";
  oput->type       = TYPE_STRING;
  oput->required   = NO;
  oput->description= "Name of vector map to be created.";
  oput->answer     = "" ;

  verbose = G_define_option() ;
  verbose->key        = "verbose";
  verbose->type       = TYPE_STRING;
  verbose->required   = NO;
  verbose->description= "Debugging level : 0 (silent) - 3 (verbose)" ;
  verbose->answer     = "1" ; /* Set default log reporting to basic (1) */

  lfile = G_define_option() ;
  lfile->key        = "logfile";
  lfile->type       = TYPE_STRING;
  lfile->required   = NO;
  lfile->description= "Name of file where to log operations";
  lfile->answer     = "";

  snapd = G_define_option() ;
  snapd->key        = "snapdist";
  snapd->type       = TYPE_STRING;
  snapd->required   = NO;
  snapd->description= "Snap distance in ground units (Default = 10^-10)";
  snapd->answer     = "1.0e-10";

  /*
  minangle = G_define_option() ;
  minangle->key        = "sliver";
  minangle->type       = TYPE_STRING;
  minangle->required   = NO;
  minangle->description= "Min. angle subtended by a wedge at node (degrees - default 0.0001)";
  minangle->answer     = "0.0001";
  */

  mscale = G_define_option() ;
  mscale->key        = "scale";
  mscale->type       = TYPE_INTEGER;
  mscale->required   = NO;
  mscale->description= "Set initial scale [1:2400]";
  mscale->answer     = "2400";

  attribute = G_define_option() ;
  attribute->key        = "attribute";
  attribute->type       = TYPE_STRING;
  attribute->required   = NO;
  attribute->description= "Name of attribute to use as category";
  attribute->answer     = "";
    
  catlabel = G_define_option() ;
  catlabel->key        = "label";
  catlabel->type       = TYPE_STRING;
  catlabel->required   = NO;
  catlabel->description= "Name of attribute to use as category label";
  catlabel->answer     = "";
    
  oflag = G_define_flag();
  oflag->key         = 'o';
  oflag->description = "Allow overwrite of existing vector map with the same name";
  oflag->answer      = 0;

  fleflag = G_define_flag();
  fleflag->key         = 'l';
  fleflag->description = "Force writing  polygon coverage as a line coverage";
  fleflag->answer      = 0;

  uflag = G_define_flag();
  uflag->key         = 'u';
  uflag->description = "Create unique value for parts of compound object";
  uflag->answer      = 0;

  /* Set flag for listing fields of database */
  listflag = G_define_flag();
  listflag->key     = 'd';
  listflag->description = "List fields of DBF file";

  if (G_parser(argc, argv))
    exit(-1);
    

  strcpy(src_file, iput->answer);
  strcpy(vmap, oput->answer);
  strcpy(log_file, lfile->answer);

  /* Check the input file */


  if(!strcmp(log_file, "")) {
    G_warning("Log file not specified. Sending messages to 'standard error'");
    print_to_error = 1;
  }

  verbosity = atoi(verbose->answer);

  if(verbosity < 0 || verbosity > 3) {
    G_warning("Verbosity level not recognised. Setting to 1: basic");
    verbosity = 1;
  }


  /* Check if the proposed vector map is a legal name */

  if(!strcmp(vmap, "")) {
    /* Vector map not specified - use the prefix */

    get_file_basename(base0, src_file, shp_extension);
    strncpy(vmap, base0, 511);
  }

  if(G_legal_filename(vmap) < 0 ) {
    fprintf(stderr, "\nCan't find a suitable name for resulting vector map (output parameter).\n");
    exit(1);
  }

  /* Process editing parameters */

  snap1 = atof(snapd->answer);
  sliver1 = 0.0001;
  scale1 = atoi(mscale->answer);


  /* Process flags */

  do_overwrite = oflag->answer;
  force_le = fleflag->answer;
  u_val = uflag->answer;

  /* Open the log file for write ops */

  if(print_to_error) {
    lf = stderr;
  }

  else {
    lf = fopen(log_file, "w");
    if(lf == NULL) {
      fprintf(stderr, "ERROR: Can't open log file for writing.");
      exit(-1);
    }
  }

  /* Examine the `-d' flag */
  if(listflag->answer) {
      int	i;
        
      hDBF = DBFOpen( src_file, "r" );
      if( hDBF == NULL )
        G_fatal_error("%s - DBF not found, or wrong format.\n", src_file);

      fprintf (stdout , "Attribute fields available in %s:\n", src_file );
      for( i = 0; i < DBFGetFieldCount(hDBF); i++ )
        {
	  char	field_name[15];
	  int   field_width; 
	  char  *fld=NULL; 
          DBFFieldType ftype;

          ftype=DBFGetFieldInfo( hDBF, i, field_name, &field_width, NULL );

	  switch (ftype) {
		case 0:
			fld="text";
		break;
		case 1:
			if (field_width<=7) fld="int4";
				else fld="int8";
		break;
		case 2:
			fld="float4";
		break;
		case 3:
            		G_fatal_error ("Invalid field type - bailing out");
		break;
	  }

	  DBFGetFieldInfo( hDBF, i, field_name, NULL, NULL );
	  fprintf (stdout, "%i: %s [%s:%i]\n", (i+1), field_name, fld , field_width);
        }
        
      DBFClose( hDBF );
      exit (0);
  } /* -d list */


  if((ms0 = G_find_file("dig", vmap, ""))) {

    /* Is this in the current mapset. If so we can only continue
       if over-write is allowed
    */

    if(!strcmp(ms0, G_mapset())) {
      /* Map with same name in same mapset */

      if(!do_overwrite) {
	fprintf(stderr, "Map <%s> already exists. Please select another name.\n", vmap);
	fprintf(stderr, "Alternatively you can force over-writing using the `-o' flag.\n");
	exit(1);
      }
    }
  }




  /* Build import controller struct. */

  ctr1 = import_ctrl_init();
  if(ctr1 == NULL) {
    fprintf(stderr, "Memory allocation error.\n");
    if(!print_to_error) fclose(lf);
    exit(-1);
  }

  import_ctrl_construct(ctr1, src_file, vmap, lf, do_overwrite, verbosity,
			force_le, u_val, scale1, snap1, sliver1, attribute->answer,
			catlabel->answer);
  

  /* Now pass parameters to import routine */

  ires = vmap_import(ctr1);

  switch(ires) {

  case 1: 
    {
      fprintf(stderr, "\n\nERROR: Map `%s' already exists.\n", vmap);
      fprintf(stderr, "If you wish to over-write, use the `-o' flag.\n");
      break;
    }

  case -1: 
    {
      fprintf(stderr, "\n\nFATAL ERROR: There was a problem importing the map.\n");
      if(! print_to_error) {
        fprintf(stderr, "Please consult the log file `%s' for details.\n", log_file);
      }
      break;
    }

  case -2: 
    {
      fprintf(stderr, "\n\nWARNING. Map `%s' imported, but there were problems.\n", vmap);
      if(! print_to_error) {
        fprintf(stderr, "Please consult the log file `%s' for details.\n", log_file);
      }
      break;
    }

  default: 
    {
      fprintf(stderr, "\n\nMap `%s' imported, apparently without problems.\n", vmap);
      if(! print_to_error) {
        fprintf(stderr, "Please consult the log file `%s' for details.\n", log_file);
      }    
    }
  }

  if(!print_to_error)
    fclose(lf);

  import_ctrl_destroy(ctr1);

  return(0);

}
