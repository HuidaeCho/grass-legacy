#include "pattern.h"
struct _parms_
{
    int cellfd;	/* cell file descriptor */
    int outlinefd;
    int outline_color;
    RASTER_MAP_TYPE map_type;
    struct Categories pcats;
    struct Colors pcolr;
    CELL min_color, max_color;
    int with_colortable;
    int colormode;
    char *cellname;
    char *cellmapset;
    char celltitle[200];
    PATTERN **pattern;
    int need_stats; /* do we need to collect stats for ctable, or legend? */

    char *commentfile;
    char *plfile;

    char scaletext[100];

    int grid;
    int grid_color;
    int grid_numbers;
    int grid_numbers_color;

    int npanels;
    int startpanel, endpanel;
};

#ifdef MAIN
    struct _parms_ parms;
#else
    extern struct _parms_ parms;
#endif
