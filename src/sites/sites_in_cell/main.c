/* %W% %G% */
#include "gis.h"
#include "site.h"
#include "site_count.h"

main (argc, argv) 	char *argv[];
{
    struct RCLIST *rc, *utm_to_rc();
    int count;
    SITE_LIST site_list;
    struct Cell_head window;
    char *desc;
    char title[200];
    char layer[40];
    char *mapset;
    int quad;
    int cellfd;
    int north;
    int east;
    int n;

    if (argc != 2)
	exit(0);
    G_gisinit (argv[0]);

    new_screen();

    G_get_window (&window);

    initialize_site_list (&site_list);
    if (!get_site_list (&site_list, argv[1]))
	exit (-1);

    quad = 0;

    new_screen();

    mapset = G_ask_cell_new ("", layer);
    if (!mapset)
	exit(0);
    do
	printf("\nenter a title for cell map <%s>: ", layer);
    while (!G_gets(title));
    G_strip (title);

    printf ("creating empty cell file ...\n");
    if ((cellfd = G_open_cell_new_random (layer)) < 0)
    {
	printf("can't create cellfile <%s>\n", layer);
	exit(-1);
    }

    counter_reset ("transfering sites to cell file ...", 0);
    counter (0);

    rc = NULL;
    count = 0;

    rewind_site_list (&site_list);
    for (n = 1; next_site (&site_list, &north, &east, &desc); n++)
    {
        rc = utm_to_rc(&window,north,east,rc,&count);
	counter (n);
    }
    for(n=0;n<count;n++)
      G_put_map_row_random (cellfd,&rc[n].val,(int)rc[n].row,(int)rc[n].col,1);
    printf("\n");
    printf ("creating support files for %s\n", layer);
    G_close_cell (cellfd);
    if (*title)
	G_put_cell_title (layer, title);

    { char buf[400];
      printf ("compressing cell file ...\n");
      sprintf (buf, "compress %s > /dev/null", layer);
      system (buf);
    }

    printf("\n<%s> cell file complete\n", layer);
    exit (1);
}
