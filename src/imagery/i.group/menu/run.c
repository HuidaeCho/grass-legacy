#include "imagery.h"

run (pgm, group)
    char *pgm;
    char *group;
{
    char buf[1024];
    int stat;

    sprintf (buf, "%s/etc/imagery/%s %s", G_gisbase(), pgm, group);
    if(stat=G_system (buf))
	sleep(3);
    return stat;
}
