#include "list.h"
do_copy(n,old,mapset,new)
    char *old;
    char *mapset;
    char *new;
{
    int i;
    int len;
    char command[2048];
    char path[1024];

    printf ("COPY [%s in %s] to [%s]\n", old, mapset, new);

    len = get_description_len(n);

    hold_signals(1);
    for (i = 0; i < list[n].nelem; i++)
    {
	printf (" %-*s ", len, list[n].desc[i]);
	fflush (stdout);


	G__make_mapset_element (list[n].element[i]);
	G__file_name (path, list[n].element[i], old, mapset);
	if (access (path, 0) != 0)
	{
	    printf ("MISSING\n");
	    continue;
	}
	sprintf (command, "%s/etc/copy '%s' '", G_gisbase(), path);
	G__file_name (path, list[n].element[i], new, G_mapset());
	strcat (command, path);
	strcat (command, "'");
	system (command);
	printf("\n");
    }

/* special case: remove (yes, remove) the secondary color table, if it exists */
    if (strcmp (list[n].element[0], "cell") == 0)
    {
	char colr2[50];

	sprintf (colr2, "colr2/%s", G_mapset());
	G_remove (colr2, new);
    }
    hold_signals(0);
}
