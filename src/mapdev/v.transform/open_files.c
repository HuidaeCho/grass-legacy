
/*
*  open_vect_files() - asks the user for vector filenames and opens the files with
*    the appropriate mode.  It also stores the mapnames and mapsets.
*    Exits if there is no vector file to convert.
*
*  open_att_files() - opens the attribute files with the appropriate mode.
*    It doesn't prompt the user for mapnames it uses mapnames and mapsets
*    information stored by open_vect_files().
*    Returns a '-1' if there is no att file to convert.
*    Exits if it can't open a file to write to.
*
*  Not having a vector file is an error, but it is not an error for a vector file
*  to not have an att file associated with it.
*
*  Written by the GRASS Team, 02/16/90, -mh.
*/

#include	<stdio.h>
#include	"gis.h"
#include	"trans.h"

#define		ASCII_DIR    "dig_ascii"
#define		ATTS_DIR      "dig_att"

int 
open_vect_files (struct file_info *Readfile, struct file_info *Writefile, struct file_info *Coordfile)
{
/*  check for existance of support directories  */
	G__make_mapset_element(ASCII_DIR) ;


/*  If the name wasn't given on the command line, ask for it  */
	if ( Readfile->name[0] == '\0')
		Readfile->mapset = G_ask_old( " ASCII VECTOR (DIGIT) FILE TO TRANSFORM",
			Readfile->name, ASCII_DIR, "ascii vector") ;
	else
		Readfile->mapset =  G_mapset() ;

	if ( ! Readfile->mapset)
		exit(0) ;

	G__file_name( Readfile->full_name, ASCII_DIR, Readfile->name, Readfile->mapset) ;

/*  Now do the same for the output file  */

	if ( Writefile->name[0] == '\0')
		Writefile->mapset = G_ask_new( " FILENAME TO STORE TRANSFORMED ASCII VECTOR FILE",
			Writefile->name, ASCII_DIR, "ascii vector") ;
	else
		Writefile->mapset =  G_mapset() ;

	if ( ! Writefile->mapset)
		exit(0) ;

	G__file_name( Writefile->full_name, ASCII_DIR, Writefile->name, Writefile->mapset) ;

/*  Open the files with appropriate modes  */
	if ( (Readfile->fp = fopen(Readfile->full_name, "r"))  ==  NULL)
	{
		fprintf(stderr, "Can't open vector file : %s\n", Readfile->full_name) ;
		exit(-1) ;
	}

	if ( (Writefile->fp = fopen(Writefile->full_name, "w"))  ==  NULL)
	{
		fprintf(stderr, "Can't open vector file : %s\n", Writefile->full_name) ;
		exit(-1) ;
	}

	if ( Coordfile->name[0] != '\0')
	if ( (Coordfile->fp = fopen(Coordfile->name, "r"))  ==  NULL)
	{
		fprintf(stderr, "Can't open file with coordinates : %s\n", Coordfile->name) ;
		exit(-1) ;
	}

	return(0) ;
}


int 
open_att_files (struct file_info *Readfile, struct file_info *Writefile)
{

/*  check for existance of support directories  */
	G__make_mapset_element(ATTS_DIR) ;

	G__file_name( Readfile->full_name, ATTS_DIR, Readfile->name, Readfile->mapset) ;

	G__file_name( Writefile->full_name, ATTS_DIR, Writefile->name, Writefile->mapset) ;

	if ( (Readfile->fp = fopen(Readfile->full_name, "r"))  ==  NULL)
		return(-1) ;

	if ( (Writefile->fp = fopen(Writefile->full_name, "w"))  ==  NULL)
	{
		fprintf(stderr, "Can't open dig_atts file : %s for write.\n",
			Writefile->full_name) ;
		return(-1) ;
	}

	return(0) ;
}


