/*
 *   d.what.r.inf
 *
 *
 *   Generate a list of database attributes
 *   associated with raster location selected
 *   using the GRASS mouse.
 *
 *   jaf 1/11/91, rev 2/19/92
 */

#define GLOBAL
#include "what.h"
#define MAIN

main(argc, argv)
int argc ;
char **argv ;
{
    char *dbname;
    int i;
    int selPassed;      /* User specified select inputfile */


        selPassed = 0;

	/* Initialize the GIS calls */
	G_gisinit(argv[0]) ;

	/* Check DATABASE env variable */
        if ((dbname=G__getenv("DATABASE")) == NULL) {
            fprintf(stderr,
                  "Please run g.select.inf to identify a current database.\n");
	    exit(-1);
           }

        /* Check for -s flag indicating selectfile input */
        for (i=0; i<argc; i++)
                if(strcmp(argv[i],"-s")==0)
                        selPassed = 1;


        if (selPassed)          /* user provides SQL command file       */
                getSelectOpts(argc,argv);
          else                  /*  Pgm builds SQL command file         */
                 getAllOpts(argc, argv);


	exit(0) ;
}
