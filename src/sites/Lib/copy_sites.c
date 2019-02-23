#include "site.h"
#include "gis.h"
copy_sites (a, b, clipped)

	SITE_LIST *a, *b;
{
	int north, east;
	char *desc;
	struct Cell_head window;

	if (clipped)
		if (G_get_window(&window) == -1)
		{
			fprintf(stderr,"warning - can't read current window\n");
			clipped = 0;
		}

	rewind_site_list (a);
	initialize_site_list (b);

	strcpy (b->name, a->name);
	strcpy (b->desc, a->desc);
	while (next_site (a, &north, &east, &desc))
		if (!clipped || within_window (north, east, &window))
			add_site (b, north, east, desc);
}
