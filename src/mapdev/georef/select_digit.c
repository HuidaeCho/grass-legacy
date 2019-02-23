
#include	<stdio.h>
#include	"bin_digit.h"

/*
*	show available digitizers and ask if they want to keep their current
*	digitizer or select a different one.  checks the answer against the
*	digitcap file.
*/

select_digitizer( fp, selected_digitizer, Driver)
	FILE	*fp ;
	char	*selected_digitizer ;
	struct  driver_desc  *Driver ;
{

	int   status ;
	int   driver_cnt ;
	int   default_driver ;
	int   which_driver ;

	char  buf[128];
	

/*  check the digitcap file and get a count of drivers  */

	driver_cnt = get_driver_name( fp, "", Driver) ;

	if ( ! driver_cnt )
	{
		fprintf(stderr, "There are no digitizers defined in the digitcap file\n") ;
		fprintf(stderr, "Contact your GRASS system administrator\n") ;
		exit(-1) ;
	}


/*  if there is no digitizer set but there is only one digitizer on the system.
*	give them that one.  all the fields were loaded by get_driver_name().
*/
	if ( driver_cnt == 1)
			return(1) ;


/*  show available digitizers on the system   */
	show_driver_names( fp, Driver) ;
	
/*  setup to use the digitizer from the last session */
	default_driver = 0 ;
	if ( *selected_digitizer != NULL)
		default_driver = get_driver_name( fp, selected_digitizer, Driver) ;

	if (default_driver < 0)
		default_driver = 0 ;

/*  show them their current digitizer from .gisrc  */
	printf("\n\n") ;

	if (default_driver)
	{
		printf(" Hit return to use digitizer in brackets\n or type in number or name of other digitizer.\n\n") ;
		printf(" Select digitizer [%s] :  ",  selected_digitizer) ;
	}
	else
		printf(" Select digitizer by number or name :  ") ;
	
	
	/*  let them select a different digitizer  */
	
	which_driver = -1 ;
	while(1)
	{
		if ( gets(buf) == NULL)
			exit(-1) ;

		G_squeeze(buf) ;

		if( *buf == NULL )
		{
			if (default_driver)
				return(2) ;
			else
			{
				printf(" There is no digitizer selected.\n") ;
				printf(" Select digitizer by number or name :  ") ;
				continue ;
			}
		}

		which_driver = atoi(buf) ;
	
	    /*  entered number of digitizer  */
		if ( which_driver > 0  &&  which_driver < driver_cnt+1)
		{
			status = get_driver( fp, which_driver, Driver) ;
	
			if ( status > 0 )
				return(2) ;
		}
	
	    /*  entered text name of digitizer  */
		if ( which_driver  == 0 )
		{
			status = get_driver_name( fp, buf, Driver) ;
	
			if ( status > 0 )
				return(2) ;
		}
	
		printf(" I don't know this digitizer:  [%s] . \n\n", buf) ;
		printf(" Select digitizer by number or name :  ") ;

		default_driver = 0 ;
	
	}
	
}
