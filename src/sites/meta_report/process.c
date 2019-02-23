/* %W% %G% */

#include "gis.h"
#include "site.h"

process (meta_report, site_list, layer_list, quadsize, with_stats)
    FILE *meta_report;
    SITE_LIST *site_list;
    FILE *layer_list;
{
    char layer[100];
    char mapset[100];
    int n;
    int r, c;
    int north, east;
    char *desc;
    struct Cell_head window;


    G_get_window (&window);

    fprintf (meta_report,"location|%s|%s\n", G_location(), G_myname());
    fprintf (meta_report,"mapset|%s\n", G_mapset());
    fprintf (meta_report,"window north|%lf\n",window.north);
    fprintf (meta_report,"window south|%lf\n",window.south);
    fprintf (meta_report,"window west|%lf\n",window.west);
    fprintf (meta_report,"window east|%lf\n",window.east);
    fprintf (meta_report,"ns resolution|%lf\n",window.ns_res);
    fprintf (meta_report,"ew resolution|%lf\n",window.ew_res);

/* print the site list */

    fprintf(meta_report,"site list|%s|%s\n",
	    site_list->name, site_list->desc);

    fprintf(meta_report,"\n# point <site reference> <easting> <northing> <site description>\n\n");

    rewind_site_list (site_list);
    for (n = 1; next_site (site_list, &north, &east, &desc); n++)
	    fprintf(meta_report,"point|%d|%d|%d|%s\n",
		    n, east, north, desc);

/* print layer names */

    fprintf(meta_report,"\n# layer <layer reference> <layer name> <full name>\n\n");

    for (n = 1; fscanf (layer_list, "%s %s", layer, mapset) == 2; n++)
    {
	fprintf(meta_report,"layer|%d|%s|%s\n", n, layer,
		G_get_cell_title (layer, mapset));
    }

/* do layer stats next */

    new_report_screen();
    printf("data extraction phase 1 - category info");
    if (with_stats)
	    printf(" (with stats)");
    printf("\n\n");

    fprintf(meta_report, "\n# cat <layer reference> <category> <cell count> <category name>\n\n");

    fseek (layer_list, 0L, 0);
    for (n = 1; fscanf (layer_list, "%s %s", layer, mapset) == 2; n++)
	    layer_stats (meta_report, layer, mapset, n, with_stats);
    printf("\ncomplete\n");
    sleep (2) ;

/* do site data next */

    new_report_screen();
    printf("data extraction phase 2 - site data (%d sites)\n\n",
	    count_sites(site_list,0));


    fprintf(meta_report,"\n# matrix { <east displacement> <south displacement> } ...\n\n");

    n = quadsize*2 + 1;
    fprintf(meta_report,"matrix size|%d\n\n", n*n);

    for (r = -quadsize; r <= quadsize; r++)
    {
	fprintf(meta_report,"matrix");
	for (c = -quadsize; c <= quadsize; c++)
	    fprintf(meta_report,"|%d|%d", c, r);
	fprintf(meta_report,"\n");
    }

    fprintf(meta_report,"\n# data <layer reference> <site reference> <category> ...\n\n");

    fseek (layer_list, 0L, 0);
    for (n = 1; fscanf (layer_list, "%s %s", layer, mapset) == 2; n++)
	site_data (meta_report, site_list, layer, mapset, n, &window, quadsize);
    
    printf("\ncomplete\n");
    sleep (3);
}
