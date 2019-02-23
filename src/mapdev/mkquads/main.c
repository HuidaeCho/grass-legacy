
/*
*  Creates as many quads as it can in the current window.
*  Find the lower left quad point in the window. 
*  From that calculate number of quads that will fit in window.
*  Create the vector file.
*
*  Written by GRASS Team, Fall of 88,  Michael H.
*/

/*
*  There is a hidden option in this program. `-r`  it creates a reg file that
*  can be used by `digit`.  This is to be used by users who understand
*  the reg file, because the reg file WILL have to be edited for their use.
*  The reg file is a by-product of building the vector file.
*/

#include	<stdio.h>
#include	"gis.h"
#include	"quad_structs.h"


/**  data directories   **/
#define		B_DIG		"dig"
#define		SITE_DIR	"site_lists"
#define		WIND_DIR	"windows"
#define		CHARNULL	'\0'

static char  *PROG ;


main (argc, argv)
	int  argc ;
	char  *argv[] ;
{

	/*  store filename and path  */
	char  file_name[128] ;
	char  buffer[128] ;

	FILE  *fp_reg, *fp_sites ;
	FILE  *fp_digit,  *fopen() ;

	struct  quads_description  quads_info ;
	struct Cell_head window ;	/*  window in utm's  */
	struct Cell_head window_ll ;	/*  window in lat, long  */
	struct Cell_head work_window ;	/*  used for writing multiple windows */

	struct command_flags flags ;

	PROG = argv[0] ;
	G_gisinit(PROG) ;
	system("clear") ;
	setbuf(stdout, NULL) ;


	if (G_projection() != METERS)
	{
		fprintf( stderr, " This program can only create a UTM vector file and this is not a UTM location.\n") ;
		exit(-1) ;
	}
		
    /*  get the current window  */
	G_get_window(&window) ;
	G_get_window(&window_ll) ;

    /*  make sure dig directory is there  */
	G__make_mapset_element( B_DIG) ;

	*buffer = CHARNULL ;
	check_args( argc, argv, buffer, &flags) ;

	if (flags.usage)
	{
		fprintf( stderr, " Usage:  %s  file.name  -[e,v,s]\n", PROG) ;
		fprintf( stderr, "    -e   encompass the GIS window\n") ;
		fprintf( stderr, "    -s   creates a site file\n") ;
		fprintf( stderr, "    -v   creates a vector file (default)\n") ;
		exit(-1) ;
	}

/*  setup vector file */
	if (flags.vectors)
	{

		create_name( file_name, " VECTOR FILENAME ", buffer,
			B_DIG, "binary vector") ;

		if ( (fp_digit = fopen(file_name, "w"))  ==  NULL)
		{
			fprintf(stderr, " %s: Can't open file for write: %s\n", PROG, file_name) ;
			exit(-1) ;
		}
	}

/*  setup sites file */
	if (flags.sites)
	{
    	/*  make sure directory is there  */
		G__make_mapset_element( SITE_DIR) ;
		create_name( file_name, " SITES FILENAME ", buffer,
			SITE_DIR, "ascii sites file") ;


		if ( (fp_sites = fopen(file_name, "w"))  ==  NULL)
		{
			fprintf(stderr, " %s: Can't open sites file for write.\n", PROG) ;
			exit(-1) ;
		}
	}

/*  setup reg file */

	if (flags.reg)
	{

		if ( (fp_reg = fopen("reg.quads", "w"))  ==  NULL)
		{
			fprintf(stderr, " %s: Can't open reg file for write.\n", PROG) ;
			exit(-1) ;
		}
	}

/*  setup the conversions */
	setup_ll_to_utm (&quads_info) ;
	convert_window_to_ll (&window_ll) ;
	init_quad_struct( &quads_info, &window_ll) ;
	find_quad_point (&quads_info, &window_ll, &flags) ;
	calculate_quads ( &quads_info, &window_ll, &flags) ;

    /*  initialize and write the digit vector header  */
	if (flags.vectors)
	{
		printf("\n Creating vector file of quads...") ;
		init_header ( fp_digit, &window) ;
		write_quads ( fp_digit, &quads_info ) ;
		fclose (fp_digit) ;
	}

	if (flags.sites)
	{
		printf("\n\n Creating sites file ...") ;
		sites_quads ( fp_sites, &quads_info ) ;
		fclose (fp_sites) ;
	}

	if (flags.reg)
	{
		printf("\n\n Creating reg file of `reg.quads`...") ;
		reg_quads ( fp_reg, &quads_info ) ;
		fclose (fp_reg) ;
	}

    /*  this is in the future  */
	if (flags.windows)
	{
		printf("\n\n Creating window files with names `quad.#`...") ;
    	/*  make sure directory is there  */
		G__make_mapset_element( WIND_DIR) ;
		G_get_window(&work_window) ;

		window_quads ( &quads_info, &work_window) ;
	}


	printf("\n\n") ;
	report_quads ( stdout, &quads_info, &flags ) ;
	printf("\n") ;

}		/*  main()  */


check_args( argc, argv, filename, flags) 
	int  argc ;
	char  *argv[] ;
	char  *filename ;
	struct  command_flags  *flags ;
{

	int  stat ;

	flags->vectors = 0 ;
	flags->sites = 0 ;
	flags->windows = 0 ;
	flags->encompass = 0 ;
	flags->reg = 0 ;
	flags->usage = 0 ;
	flags->files = 0 ;

	if (argc == 1)
	{
		flags->vectors = 1 ;
		return(0) ;
	}

	while ( --argc)
		if ( *argv[argc] == '-' )
		{
			stat = dashed_args(  argv[argc], flags) ;
			if (stat)
			{
				flags->usage = 1 ;
				break ;
			}
			
		}
		else
		if (flags->files)
		{
			fprintf( stderr, " Too many filenames on the command line ...\n") ;
			flags->usage = 1 ;
			break ;
		}
		else
		if ( ! (strcmp( argv[argc], "usage") ) )
		{
			flags->usage = 1 ;
			break ;
		}
		else
		{
			flags->files = 1 ;
			strcpy ( filename, argv[argc]) ;
		}

	    /*  set default  */
		if ( flags->sites == 0  &&  flags->windows == 0)
			flags->vectors = 1 ;
	return (0) ;
}

dashed_args(  args, flags) 
	char  *args ;
	struct  command_flags  *flags ;

{

	char  *ptr ;

	ptr = args ;
	/*  skip over the '-'  */
	++ptr ;
	while ( *ptr )
		switch ( *ptr++)
		{
		case 'v':
			flags->vectors = 1 ;
			break ;
		case 's':
			flags->sites = 1 ;
			break ;
		case 'w':
			flags->windows = 1 ;
			break ;
		case 'e':
			flags->encompass = 1 ;
			break ;
		case 'r':
			flags->reg = 1 ;
			break ;
		default:
			return(-1) ;
			break ;
		}

	return(0) ;
}

