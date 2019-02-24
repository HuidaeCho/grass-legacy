#include "glob.h"
cur_from_db()
{
    char name[30];
    char *mapset;
    struct Cell_head window;
    char *err;
    char *G__get_window();

    mapset = G_ask_old_ext ("Select a region",
	name,window_dir,"region","with region values", lister);
    if (!mapset) return 1;

    if (err = G__get_window (&window, window_dir, name, mapset))
	fprintf (stderr, "region [%s] in mapset [%s] %s. can't select.\n",
		name, mapset, err);
    else
    {
	if(!edit_window(&window)) return 1;
	set_window (&window, name);
    }
    return 0;
}
