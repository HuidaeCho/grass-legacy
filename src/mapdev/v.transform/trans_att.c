
/*
*  Transform the current attribute file.
*
*
*  Written by the GRASS Team, 04/13/90, -mh.
*/

#include <stdio.h>
#include "libtrans.h"
#include "dig_atts.h"

int transform_att_file (FILE *fp_current, FILE *fp_new)
{
	char type ;
	int  cat ;
	long offset ;
	double x, y;
	int  return_value ;

	rewind(fp_current) ;


	while( 1 )
	{

		return_value = read_att (fp_current, &type, &x, &y, &cat, &offset ) ;

		/*  EOF condition  */
		if (return_value == 1)
			break ;

		if (return_value < 0)
		{
			/*
			*  Deleted atts 'a' will cause this to occur.
			*
			* fprintf( stderr, "Attribute file has unreadable lines in it\n") ;
			*/
			continue ;
		}

		transform_a_into_b( x, y, &(x), &(y) ) ;

		write_att (fp_new, type, x, y, cat) ;
	}

	return(0) ;
}


