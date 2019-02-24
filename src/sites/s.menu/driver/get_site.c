#include "gis.h"
#include "site.h"
#include "dir.h"
get_site(site_list)

	SITE_LIST *site_list;
{
	char name[40];
	char *mapset;
	int stat;
	FILE *fd;

	stat = 0;
	if (mapset = G_ask_old ("",name,SITE_DIR,"site list"))
	{
		fd = G_fopen_old (SITE_DIR, name, mapset);
		if (!fd)
		{
			printf("unable to open site list %s\n", name);
			return -1;
		}
		announce ("reading site list <");
		announce (name);
		announce ("> ...");
		if(!read_site_list (site_list, fd))
			stat = -1;
		fclose (fd);
		strcpy (site_list->name, name);
		announce ("\n");
	}
	return(stat);
}
