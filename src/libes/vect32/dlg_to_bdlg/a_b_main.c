/*  @(#)a_b_main.c	2.1  6/26/87  */
#include <stdio.h>
#define MAIN
#include "gis.h"
#include "dlghead.h"
#include "format.h"

/*
*	old version is for 1.0 release dlg files
*	new version is for 2.0 release and beyond
*/

main(argc, argv)
	int argc ;
	char *argv[] ;
{
	int i ;
	char  *ascii ;
	char  *binary ;
	FILE *dlg ;
	FILE *bin ;
	FILE *fopen() ;
	char filename[128] ;

	extern	int	new_format ;

	setbuf(stdout, 0) ;

/* Print warning */
	if (argc != 3 && argc != 4)
	{
		fprintf (stdout,"\nUsage: %s old-dlg-ascii-file new-dlg-binary-file\n",
			argv[0]) ;
		fprintf (stdout,"  or : %s old-dlg-ascii-file new-dlg-binary-file -o\n",
			argv[0]) ;
		exit(-1) ;
	}

	if ( argc == 4)
	 {
		if ( strcmp ( argv[3], "-o")) 
		 {
			fprintf (stdout,"\nUsage: %s old-dlg-ascii-file new-dlg-binary-file\n",
				argv[0]) ;
			fprintf (stdout,"  or : %s old-dlg-ascii-file new-dlg-binary-file -o\n\n",
				argv[0]) ;
			exit(-1) ;
		 }

		new_format = 0 ;
	 }
	else
		new_format = 1 ;


/* Print warning */
	if ( (dlg = fopen(argv[1], "r")) == NULL)
	{
		fprintf (stdout,"Can't find %s\n", "dlg") ;
		exit(-1) ;
	}

	if ( ! (ascii= G_rindex(argv[1], '/')))
		ascii = argv[1] ;
	else
		++ascii ;

	if ( ! (binary= G_rindex(argv[2], '/')))
		binary = argv[2] ;
	else
		++binary ;

    /*
	fprintf (stdout,"\n\n Ascii DLG to binary DLG conversion routine\n") ;
	fprintf (stdout,"\nConverting the ascii format file: %s\n", ascii);
	fprintf (stdout,"     to the %sbinary file format: %s\n\n",
		new_format ? "": "OLD " , binary);
    */

/* Read the header section */
	if (read_dlg_head(dlg) == -1)
	{
		fprintf (stdout,"Error in reading header\n") ;
		exit (-1) ;
	}

/* Check to see if we are going to make multiple files.  Binary
 * files contain only one category per file while "optional" DLG
 * files can contain up to 32 category overlays.
 */

	if (num_cats == 1)
	{
	/* Open file for writing */
		if ( (bin = fopen(argv[2], "w")) == NULL)
		{
			fprintf (stdout,"Can't open %s\n", "bin") ;
			exit(-1) ;
		}

	/* Write binary dlg head */
		if (write_bdlg_head(bin, 0) == -1)
		{
			fprintf (stdout,"Error in writing binary dlg header\n") ;
			exit (-1) ;
		}
		
	/* Read and write the main body */
		if (a_b_dlg(dlg, bin) == -1)
		{
			fprintf (stdout,"Error in translating header\n") ;
			exit (-1) ;
		}

		fclose (dlg) ;
		fclose (bin) ;
	}
	else
	{
		fprintf (stdout,"This dlg file contains %d overlays.  Each overlay will be\n",
			num_cats) ;
		fprintf (stdout,"written to a different file\n") ;

		for (i=1; i<=num_cats; i++)
		{
		/* Open file for writing */
			sprintf(filename, "%s_%d", argv[2], i) ;
			if ( (bin = fopen(filename, "w")) == NULL)
			{
				fprintf (stdout,"Can't open %s\n", "bin") ;
				exit(-1) ;
			}

			fprintf (stdout,"\nWriting file [%s]", filename) ;

		/* Write binary dlg head */
			if (write_bdlg_head(bin, i-1) == -1)
			{
				fprintf (stdout,"Error in writing binary dlg header\n") ;
				exit (-1) ;
			}
			
		/* Read and write the main body */
			if (a_b_dlg(dlg, bin) == -1)
			{
				fprintf (stdout,"Error in translating header\n") ;
				exit (-1) ;
			}

			fclose (bin) ;
		}
		fclose (dlg) ;
	}
	exit(0);
}
