#include	<stdio.h>
#include	"dig_structs.h"


main(argc, argv)
	int argc ;
	char *argv[] ;
{
	FILE *binary, *ascii, *fopen() ;

	if (argc != 3)
	{
		fprintf (stdout,"USAGE: %s old-plus-binary new-plus-ascii\n", argv[0]) ;
		exit(-1) ;
	}

	if ( (binary = fopen(argv[1], "r") ) == NULL )
	{
		fprintf (stdout,"Not able to open <%s>\n", argv[1]) ;
		exit(-1) ;
	}

	if ( (ascii = fopen(argv[2], "w") ) == NULL )
	{
		fprintf (stdout,"Not able to open <%s>\n", argv[2]) ;
		exit(-1) ;
	}

	write_plus_asc(map,  ascii, binary) ;

	fclose(binary) ;
	fclose(ascii) ;
}

