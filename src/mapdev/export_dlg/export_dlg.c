/* %W% %G% */
/*
**  Written by Dave Gerdes  6/89
**  US Army Construction Engineering Research Lab
*/

#include  "gis.h"
#include "digit.h"
#include "dig_head.h"


#define V_PROG  "Vexport.dlg"   /* non-interactive version */

struct Map_info Map;
struct head Head;

main(argc, argv)
    char *argv[];
{
	FILE *out;
	int att, line;
	register int num, i;
	char  name[150] ;
	char  command[200];
	char  out_name[150] ;
	char  in_name[150] ;
	char  *mapset ;
	char  *tmapset ;
	char *X, *Y;
	int n_points;

	G_gisinit(argv[0]);


	mapset = G_ask_old( " VECTOR (DIGIT) FILE TO EXPORT FROM",
		name, "dig", "vector") ;

	if ( ! mapset)
		exit(0) ;

	/*
	G__file_name( in_name, "dig", name, mapset) ;
	*/
	strcpy (in_name, name);

	tmapset = G_ask_new( "Resultant DLG file", name, "dlg", "dlg") ;

	if ( ! tmapset)
		exit(0) ;

	sprintf (command, "%s/bin/%s \"%s in %s\" %s", G_gisbase (), V_PROG, in_name, mapset, name);
	system (command);
	printf ("hit RETURN to continue -->");
	gets (command);
	exit (0);
}
