#include <string.h>
#include <stdlib.h>
#include "gis.h"

int main (int argc, char *argv[])
{
    char command[1024];
    char name[256], *mapset;
    char temp[256];

    G_gisinit(argv[0]);

    strcpy (command, "r.mode");

/* base map */
    mapset = G_ask_cell_old ("Enter base map", name);
    if (mapset == NULL) exit(0);
    sprintf (temp, " base='%s'", G_fully_qualified_name(name, mapset));
    strcat (command, temp);

/* cover map */
    mapset = G_ask_cell_old ("Enter cover map", name);
    if (mapset == NULL) exit(0);
    sprintf (temp, " cover='%s'", G_fully_qualified_name(name, mapset));
    strcat (command, temp);
/* -c not a supported option for r.mode yet
    sprintf (temp, "Are the values to be looked up from the category file for %s? ", name);
    if (G_yes(temp, -1))
	strcat (command, " -c");
*/

/* output map */
    if (!G_ask_cell_new("Enter name for the output map", name))
	exit(0);
    sprintf (temp, " output='%s'", name);
    strcat (command, temp);

/* run the command-line version */
    exit(system (command));
}
