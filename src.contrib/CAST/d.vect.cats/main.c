/*
 *   d.vect.cat
 *
 *
 *   Draw the category in the binary vector (dig) file that
 *   the user wants displayed on top of the current image.
 *   jaf 12/1/91
 */

#include "gis.h"
#include "Vect.h"
#define MAIN

main(argc, argv)
int argc ;
char **argv ;
{
	char *mapset ;
        char **ptr;
	char buf[1024] ;
	int i, stat ;
	int color,fill;
        int line_cat;
	char map_name[128] ;
	struct Option *opt1, *opt2, *opt3;
	struct Flag *flag1;
	struct line_pnts *Points;


	opt1 = G_define_option() ;
	opt1->key        = "map" ;
	opt1->type       = TYPE_STRING ;
	opt1->required   = YES ;
	opt1->multiple   = NO ;
	opt1->gisprompt  = "old,dig,vector" ;
	opt1->description= "Name of existing vector map to be displayed" ;

	opt2 = G_define_option() ;
	opt2->key        = "color" ;
	opt2->type       = TYPE_STRING ;
	opt2->answer     = "white" ;
	opt2->options = "white,red,orange,yellow,green,blue,indigo,violet,magenta,brown,gray,black";
	opt2->description= "Color desired for drawing map" ;

        opt3 = G_define_option();
        opt3->key	= "cat" ;
        opt3->type	= TYPE_INTEGER ;
	opt3->required	= YES ;
	opt3->multiple	= YES ;
	opt3->description= "Vector category type to be displayed" ;

	flag1 = G_define_flag();
	flag1->key 	= 'f';
	flag1->description= "Fill areas";

	/* Initialize the GIS calls */
	G_gisinit(argv[0]) ;


	/* Check command line */
	if (G_parser(argc, argv))
		exit(-1);

	fill=0;
	fill=flag1->answer;

	strcpy(map_name, opt1->answer);

	color = D_translate_color(opt2->answer);

	/* Make sure map is available */
	mapset = G_find_file2 ("dig", map_name, "") ;
	if (mapset == NULL)
	{
		sprintf(buf,"Vector file [%s] not available", map_name);
		G_fatal_error(buf) ;
		exit(-1);
	}

	R_open_driver();

	D_setup(0);

	R_standard_color(color) ;


        Points = Vect_new_line_struct ();
        
/********** Force use of level 2 vector read ***********/

       if((ptr = opt3->answers) != NULL)   /* Use opt#->answers for multiple */
          for (; *ptr != NULL; ptr++)
            {
               line_cat = atoi(*ptr);
               printf("\nCategory = %d\n",line_cat);
	       stat = plotCat (map_name, mapset, Points, line_cat, fill);
/*
	       stat = plotCat (map_name, mapset, Points, line_cat);
*/
	       D_add_to_list(G_recreate_command()) ;
             }

    Vect_destroy_line_struct (Points);

	R_close_driver();
	exit(stat);
}

