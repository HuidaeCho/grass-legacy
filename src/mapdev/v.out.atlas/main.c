
/* 12/17/92 */

#define MAIN
#include "gis.h"
#include "digit.h"
#include "dig_head.h"

#define  USAGE  "v.out.atlas dig_file atlas_file type\n"

struct Map_info Map;

int main(argc, argv)
    int argc;
    char **argv;
{
int	done=0, ret;
char    prefix[1000],
    	msg[1000],
    	*mapset,
    	name[1000],
    	dig_filepath[1000],
        dig_filename[1000],
    	att_filename[1000],
    	cat_filename[1000],
    	lin_filename[1000];
FILE	*dig_fp,
	*lin_file;

struct Option *opt1;
struct Option *opt2;
struct Option *opt3;

G_gisinit("Export ATLAS");

opt1 = G_define_option();
opt1->key = "dig_name";
opt1->description = "dig file";
opt1->type = TYPE_STRING;
opt1->required = YES;
 
opt2 = G_define_option();
opt2->key = "atl_name";
opt2->description = "atlas file";
opt2->type = TYPE_STRING;
opt2->required = YES;

opt3 = G_define_option();
opt3->key = "type";
opt3->description = "Type: a=Area, l=Line";
opt3->type = TYPE_STRING;
opt3->required = NO;
opt3->answer = "A";
opt3->options = "A,a,L,l";

/*  check args and set flags  */
	
    if(G_parser (argc, argv))
    {
        fprintf (stderr, "%s: Command line error.\n\n Usage: %s\n",
		argv[0], USAGE);
        exit (-1);
    }

/* Show advertising */
    printf("\n\n   Export from GRASS Vector to ATLAS GIS format.\n\n") ;

    if ((mapset = G_find_file2 ("dig", opt1->answer, "")) == NULL)
	G_fatal_error ("Could not find DIG file %s\n", opt1->answer);
    
    G__make_mapset_element("atlas") ;
    G__file_name(prefix, "atlas", opt2->answer, G_mapset()) ;

    G_strcpy(lin_filename,prefix);
    G_strcat(lin_filename,".bna");

    lin_file = fopen(lin_filename,"w");

printf("ATLAS data being created\n");
       write_lines(opt1->answer,mapset,Map,lin_file,opt3->answer);

	printf("Done processing.\n");
    exit(0);
}
