/*  %W%  %G%  */

#include "gis.h"
opencell (fullname, name, mapset)
    char *fullname;
    char *name;
    char *mapset;
{
    char *m;
    char temp[100];
    int fd;

    m = G_find_cell (fullname, "");
    if (m == NULL)
    {
	fprintf (stderr, "warning: %s - cell file not found\n", fullname);
	return -1;
    }
    sscanf (fullname, "%s", name);
    strcpy (mapset, m);
    fd = G_open_cell_old (name, mapset);
    if (fd < 0)
	fprintf (stderr, "warning: unable to open [%s] in [%s]\n",
	    name, mapset);
    return fd;
}

openvect (fullname, name, mapset)
    char *fullname;
    char *name;
    char *mapset;
{
    char *m;
    char temp[100];
    int fd;

    /*
    fprintf (stderr, "openvect ('%s', '%s', '%s'\n)", fullname, name, mapset);
    */
    m = G_find_vector (fullname, "");

    if (m == NULL)
    {
	fprintf (stderr, "warning: %s - vector file not found\n", fullname);
	return -1;
    }
    /*
    sscanf (fullname, "%s", name);
    */
    strcpy (name, fullname);
    strcpy (mapset, m);
    /*
    fd = G_open_cell_old (name, mapset);
    if (fd < 0)
	fprintf (stderr, "warning: unable to open [%s] in [%s]\n", name, mapset);
    return fd;
    */
    return (0);
}
