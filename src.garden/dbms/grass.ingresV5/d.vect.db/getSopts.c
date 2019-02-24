/* getSelectOpts.c - passes select range of program options to G_parser.
                     The [-s] option indicates that an input
                     file with SQL commands is being provided. This
		     ability to include a well formed SQL command
		     file gives the user more control over output
		     columns and complex database joins etc.

                     If the sql file requires input from GRASS
                     (eg category val or coord X,Y use a [?]
                     as a placeholder as per PREPARE). The SQL
                     input file will be parsed and the [?] will 
                     replaced prior to executing the query.

                  jaf 2/19/92
       
   INGRES modifications - New subroutine name:
			  runSQL <=> runInfxFile

   Katarina Johnsson 930416
*/

#include "gis.h"


getSelectOpts (argc, argv)
    int argc;
    char **argv;


{

	char *mapset;
    	int colr, i, j, retval;

	struct Option *key, *where, *tab, *map,*color, *sql, *join  ;
	struct Flag *select;

	retval = 0;

	select = G_define_flag();
	select->key	= 's';
	select->description	= "Use [s] flag to select db records using an input file." ;

        sql = G_define_option() ;
        sql->key        = "sql" ;
        sql->key_desc  = "file" ;
        sql->type       = TYPE_STRING ;
        sql->required   = YES  ;
        sql->multiple   = NO ;
        sql->description= "SQL statements specifying selection criteria. ";

        map = G_define_option() ;
        map->key        = "map" ;
        map->type       = TYPE_STRING ;
	map->gisprompt	= "old,cell,raster" ;
        map->required   = YES  ;
        map->multiple   = NO ;
        map->description= "Name of existing vector file.";


        color = G_define_option() ;
        color->key        = "color" ;
        color->type       = TYPE_STRING ;
        color->required   = NO  ;
        color->multiple   = NO ;
        color->description= "Color for vector draw.";


        /* Check for help flag */
        for (i=0; i<argc; i++)
                if(strcmp(argv[i],"help")==0)  
			argv[1] = "help";

        if((argc == 2)&&(strcmp(argv[1],"-s")==0 )) {        /* Run interactive parser */
                argv[1] == NULL ;
                argc = 1;
           }


	/* Invoke parser */
	if (G_parser(argc, argv))
	    exit(-1);
        if (color->answer == NULL)
                colr = D_translate_color("white");
          else
                colr = D_translate_color(color->answer);


	if ((mapset=G_find_file2("dig",map->answer,""))==NULL)  {
	     fprintf(stderr,"Vector file %s not found.\n",map->answer);
             exit(-1);
	}

	runSQL( sql->answer, map->answer, mapset, colr );

	exit(0) ;

}
