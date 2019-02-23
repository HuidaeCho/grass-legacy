/* %W% %G% */
#include "run.h"
#include "dir.h"
#include "site.h"
#include "menu.h"
#include "gis.h"

#define GET_SITE              1
#define SITE_EDIT             2
#define SITE_MASK             3
#define REPORTS               4
#define PUT_SITE              5
#define SITE_TO_CELL_BINARY   6
#define SITE_TO_CELL_FREQ     7
#define SITE_DUPS             8

static char title[80];
static char infobuf[3][80];

static MENU menu = {

    title,					0,
    "",						0,
    infobuf[0],					0,
    infobuf[1],					0,
    infobuf[2],					0,
    "",						0,
    "Please select one of the following",	0,
    "",						0,
    "Get an existing site list",		GET_SITE,
    "Mask current site list",			SITE_MASK,
    "Save the current site list in your mapset",PUT_SITE,
    "",						0,
    "Check site list for duplicate sites",	SITE_DUPS,
    "Edit site list using a unix editor",	SITE_EDIT,
    "",						0,
    "Convert site list to cell file (0/1)",	SITE_TO_CELL_BINARY,
    "Convert site list to cell file (frequency of occurence)",	SITE_TO_CELL_FREQ,
    "",						0,
    "Run reports on the current site list",	REPORTS,
    "",						0,
    "",						0,
    "stop  Leave the SITES program",		0,

    0,0};

main()
{
    SITE_LIST site_list, site_list_copy;
    struct Cell_head w;
    char buf[1024];
    int nsites0;
    int nsites1;
    int pause;
    int check;

    G_gisinit ("SITES");
    G__make_mapset_element (SITE_DIR);
    initialize_site_list (&site_list);
    initialize_site_list (&site_list_copy);
    check = 0;
    while (1)
    {
	nsites0 = count_sites (&site_list,0);
	nsites1 = count_sites (&site_list,1);
	if (nsites0 == 0)
	    sprintf(title, "SITES MAIN MENU    (current list: no sites)");
	else if (nsites1 == 0)
	    sprintf(title,
		"SITES MAIN MENU    (current list: %d site%s, none in current window)",
			nsites0, nsites0 != 1 ? "s" : "" );
	else
	    sprintf(title,
		"SITES MAIN MENU    (current list: %d site%s, %d in current window)",
			nsites0, nsites0 != 1 ? "s" : "" , nsites1);

	G_get_window (&w);
	sprintf(infobuf[0], "LOCATION: %-15s WINDOW %10.2lf(N)  %10.2lf(S) %7.2lf(RES)",
		G_location(), w.north, w.south, w.ns_res);
	sprintf(infobuf[1], "MAPSET:   %-15s        %10.2lf(E)  %10.2lf(W) %7.2lf(RES)",
		G_mapset(), w.east, w.west, w.ew_res);
	sprintf(infobuf[2], "MASK:     %-60.60s", G_mask_info());


	pause = 1;
	switch(menu_handler(menu,buf))
	{
	case SITE_TO_CELL_BINARY:
	    if(run (&site_list, "sites_to_cell", RUN_MODIFY, 1, 0) == 0)
		pause = 0;
	    check = 1;
	    break;
	case SITE_TO_CELL_FREQ:
	    if(run (&site_list, "sites_in_cell", RUN_MODIFY, 1, 0) == 0)
		pause = 0;
	    check = 1;
	    break;
	case GET_SITE:
	    if(get_site (&site_list) == 0)
		pause = 0;
	    free_site_list (&site_list_copy);
	    copy_sites (&site_list, &site_list_copy, 0);
	    check = 0;
	    break;
	case PUT_SITE:
	    if(run (&site_list, "put_gis_sl", RUN_MODIFY, 0, 0) == 0)
		pause = 0;
	    free_site_list (&site_list_copy);
	    copy_sites (&site_list, &site_list_copy, 0);
	    check = 0;
	    break;
	case SITE_DUPS:
	    duplicates (&site_list);
	    check = 1;
	    break;
	case SITE_EDIT:
	    if (site_edit (&site_list) == 0)
		pause = 0;
	    check = 1;
	    break;
	case SITE_MASK:
	    if(run (&site_list, "site_mask", RUN_MODIFY, 0, 0)==0)
		pause = 0;
	    check = 1;
	    break;
	case REPORTS:
	    if(run (&site_list, "site_reports", RUN_OUT, 1, 0) == 0)
		pause = 0;
	    check = 1;
	    break;
	default:
	    if (*buf == '!')
	    {
		unix_cmd (buf+1,&site_list);
		check = 1;
		break;
	    }
	    if (strcmp(buf,"stop") == 0)
	    {
		if (!check || equal_sites (&site_list, &site_list_copy))
		    exit(0);
		if(yes("you have modified the site list.\nif you stop now changes will be lost.\n\ndo you really wish to stop?"))
		    exit(0);
	    }
	    pause = 0;
	}
	if(pause)
		hitreturn();
    }
}
