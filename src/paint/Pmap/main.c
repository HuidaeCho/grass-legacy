#include "gis.h"
#define MAIN
#include "sites.h"
#include "dlg.h"
#include "vector.h"
#include "graphics.h"
#include "fullwindow.h"
#include "labels.h"
#include "parms.h"
#include "misc.h"
#include "colormode.h"

FILE *tracefd;
FILE *inputfd;

#define KEY(x) (strcmp(key,x)==0)

static char *help[]=
{
    "cell       cellfile              setcolor   cat(s) color",
    "sites      sitefile              defpat     name",
    "vect       vectorfile            setpat     cat pattern | builtin | all",
    "labels     labelfile             text       east north text",
    "window     windowfile            line       east north east north",
    "grid       spacing               point      east north",
    "outline",
    "colortable [y|n]",
    "comments   [unix-file]",
    "read       unix-file",
    "verbose    [0|1|2]",
    "scale      1:#|# inches|# panels|1 inch = # miles",
    "colormode  best | approx",
    ""
};

#include <signal.h>
main(argc,argv) char *argv[];
{
    struct Cell_head window;
    double Ptextscale();
    char buf[1024];
    char name[100], mapset[50];
    int i;
    int iflag;
    int can_reset_scale ;
    int can_reset_colormode ;
    int exit();

/**************** begin ******************************/

signal (SIGINT, exit);
signal (SIGTERM, exit);

    setbuf (stderr, NULL);
    G_gisinit("PAINT MAP") ;

/* open the printer */
    Pconnect();
    Popen();
    ncolors = Pncolors();
    BLACK = Pcolornum (0.0, 0.0, 0.0);
    WHITE = Pcolornum (1.0, 1.0, 1.0);
    GREY  = Pcolornum (0.75, 0.75, 0.75);
    set_text_scale (Ptextscale());

/* initialize */
    tracefd = NULL;
    inputfd = stdin;
    iflag = 0;
    verbose = 2;
    labels.count = 0;
    labels.other = NULL;
    vector.count = 0;
    site.count = 0;
    parms.startpanel = 0;
    parms.endpanel = 0;
    parms.cellfd = -1;
    parms.outlinefd = -1;
    parms.with_colortable = 0;
    parms.grid = 0;
    strcpy (parms.scaletext, "1 panel");
    can_reset_scale = 1;
    can_reset_colormode = 1;
    parms.cellname = NULL;
    parms.cellmapset = NULL;
    parms.commentfile = NULL;
    parms.plfile = NULL;
    parms.pattern = NULL;
    graphics.linestyle.table = NULL;

    set_line_style_solid();

/* arguments */
    for (i = 1; i < argc; i++)
	if (strcmp (argv[i],"trace") == 0)
	    tracefd = stdout;
	else if (sscanf (argv[i],"trace=%[^\n]",buf) == 1)
	{
	    tracefd = fopen (buf,"a");
	    if (tracefd == NULL)
		error (argv[i],"","can't open tracefile");
	}
	else if (sscanf (argv[i],"scale=%[^\n]",buf) == 1)
	{
	    can_reset_scale = isatty(0);
	    if (check_scale (buf))
		strcpy (parms.scaletext, buf);
	    else
		error (argv[i],"","illegal scale request");
	}
	else if (sscanf (argv[i],"colormode=%[^\n]",buf) == 1)
	{
	    can_reset_colormode = isatty(0);
	    if (!set_colormode(buf))
		error (argv[i],"","illegal colormode request");
	}
	else
	    error (argv[i],"??","illegal parameter");

/* get the window */
    G_get_window (&window);
    G_get_window (&fullwindow);

    while (1)
    {
	char *key;
	char *data;

	if (!input (1, buf, help))
	{
	    if (!iflag) break;
	    iflag = 0;
	    continue;
	}
	if(!key_data (buf, &key, &data))
	    continue ;

	if (KEY("session"))
	{
	    print_session(stdout);
	    reject();
	    continue;
	}

	if (KEY("read"))
	{
	    if (inputfd != stdin)
		fclose (inputfd);

	    if (sscanf (data, "%s", name) != 1)
	    {
		error (key,data,"no file specified");
		inputfd = stdin;
	    }
	    else if ((inputfd = fopen (name, "r")) == NULL)
	    {
		error (key,data,"unable to open");
		inputfd = stdin;
	    }
	    else
		iflag = 1;
	    continue;
	}

	if (KEY("verbose"))
	{
	    if (sscanf (data,"%d",&verbose) != 1)
		verbose = 2;
	    continue;
	}

	if (KEY("defpat"))
	{
	    if (sscanf (data, "%s %1s", name, mapset) == 1)
		input_pattern (name);
	    else
	    {
		error (key,data,"illegal defpat request");
		gobble_input();
	    }
	    continue;
	}

	if (KEY("setpat"))
	{
	    int cat ;

	    if (parms.cellfd < 0)
		error (key,data,"no cell file selected yet");
	    else if (sscanf (data,"%s",name) == 1 && strcmp (name, "builtin") == 0)
		builtin_patterns();
	    else if (sscanf (data,"%s",name) == 1 && strcmp (name, "all") == 0)
	    {
		if (!any_patterns())
		    error (key,data,"no patterns defined");
		else
		    set_all_patterns();
	    }
	    else if (sscanf (data,"%d %s", &cat, name) != 2 || cat < parms.pcolr.min || cat > parms.pcolr.max)
		error (key,data,"illegal setpat request");
	    else  if (!set_pattern (cat, name))
		error ("pattern",name,"not found");
	    continue;
	}

	if (KEY("setcolor"))
	{
	    int r,g,b;
	    int color;
	    int count;
	    int *list;
	    int min,max;
	    char colorbuf[100];
	    char catsbuf[100];

	    if (parms.cellfd < 0)
	    {
		error (key,data,"no cell file selected yet");
		continue;
	    }
	    if (sscanf (data, "%s %[^\n]", catsbuf, colorbuf) == 2)
	    {
		if (!scan_color (colorbuf, &color, &r,&g,&b))
		{
		    error (key,data,"illegal color");
		    continue;
		}
		if ((count = parse_number_list (catsbuf, &list)) < 0)
		{
		    error (key,data,"illegal category list");
		    continue;
		}
		for (i = 0; i < count; i += 2)
		{
		    min = list[i];
		    max = list[i+1];
		    while (min <= max)
		    {
			if (cat_color_num(min) >= 0)
			    G_set_color ((CELL)min, r, g, b, &parms.pcolr);
			min++;
		    }
		}
		free (list);
	    }
	    continue;
	}

	if (KEY("colortable"))
	{
	    parms.with_colortable = 0;
	    if (parms.cellfd < 0)
		error (key,data,"no cell file selected yet");
	    else
		parms.with_colortable = yesno (key,data);
	    if (parms.with_colortable)
		getcats();
	    continue;
	}
	if (KEY("text"))
	{
	    double e,n;
	    char east[50], north[50];
	    char text[200];

	    if (sscanf (data,"%s %s %[^\n]",east,north,text) == 3
	    && (scan_easting (east, &e) && scan_northing(north, &n)))
		record_label (east,north,text);
	    else
	    {
		gobble_input();
		error (key,data,"illegal text request");
	    }
	    continue;
	}

	if (KEY("point"))
	{
	    double e,n;
	    char east[50], north[50];

	    if (sscanf (data,"%s %s", east, north) == 2
	    && (scan_easting (east, &e) && scan_northing(north, &n)))
		record_point (e, n);
	    else
	    {
		gobble_input();
		error (key,data,"illegal point request");
	    }
	    continue;
	}

	if (KEY("line"))
	{
	    char east1[50], north1[50];
	    char east2[50], north2[50];
	    double e1, n1, e2, n2;

	    if (sscanf (data,"%s %s %s %s", east1, north1, east2, north2) == 4
	    && (scan_easting  (east1, &e1)  && scan_easting  (east2,&e2)
	    &&  scan_northing (north1, &n1) && scan_northing (north2,&n2)))
		record_line (e1,n1,e2,n2);
	    else
	    {
		gobble_input();
		error (key,data,"illegal line request");
	    }
	    continue;
	}

	if (KEY("comments"))
	{
	    switch (sscanf (data, "%s %s", name, mapset))
	    {
	    case 1: commentfile(name); break;
	    case 2: error (key,data,"illegal comments request"); break;
	    default: commentfile(""); break;
	    }
	    continue;
	}

	if (KEY("startpanel"))
	{
	    if (sscanf (data, "%d", &parms.startpanel) != 1 || parms.startpanel <= 0)
	    {
		parms.startpanel = 0;
		error (key,data,"illegal startpanel");
	    }
	    continue;
	}

	if (KEY("endpanel"))
	{
	    if (sscanf (data, "%d", &parms.endpanel) != 1 || parms.endpanel <= 0)
	    {
		parms.endpanel = 0;
		error (key,data,"illegal endpanel");
	    }
	    continue;
	}

	if (KEY ("scale"))
	{
	    if (!can_reset_scale) continue;
	    if (check_scale (data))
		strcpy (parms.scaletext, data);
	    else
	    {
		strcpy (parms.scaletext, "1 panel");
		error (key,data,"illegal scale request");
	    }
	    continue;
	}

	if (KEY ("colormode"))
	{
	    if (!can_reset_colormode) continue;
	    if (!set_colormode (data))
		error (key,data,"illegal colormode request");
	    continue;
	}

	if (KEY ("labels"))
	{
	    if (scan_gis ("paint/labels","label",key,data,name,mapset,1))
		labelfile (name, mapset);
	    continue;
	}

	if (KEY ("sites"))
	{
	    if (scan_gis ("site_lists","site list",key,data,name,mapset,1))
		sitefile (name, mapset);
	    continue;
	}

	if (KEY ("outline"))
	{
	    if (parms.cellfd < 0)
	    {
		error (key,data,"no cell file selected yet");
		gobble_input();
	    }
	    else
	    {
		parms.outlinefd = parms.cellfd;
		outlinefile();
	    }
	    continue;
	}

	if (KEY ("cell"))
	{
	    if (scan_gis ("cell","cell",key,data,name,mapset,0))
		cellfile (name, mapset);
	    continue;
	}

	if (KEY ("vector") || KEY ("vect"))
	{
	    if (scan_gis ("dig","vector",key,data,name,mapset,1))
		vectfile (name, mapset);
	    continue;
	}

	if (KEY ("window"))
	{
	    if (scan_gis ("windows","window",key,data,name,mapset,1))
		windfile (name, mapset);
	    continue;
	}

	if (KEY("grid"))
	{
	    parms.grid = -1;
	    parms.grid_numbers = 0;
	    sscanf (data, "%d", &parms.grid);
	    if (parms.grid < 0)
	    {
		parms.grid = 0;
		error (key,data,"illegal grid spacing");
		gobble_input();
	    }
	    else
		getgrid();
	    continue;
	}

	if (*key)
	    error(key,"","illegal request");
    }

    unlink_pattern_file();

    map (&window);

    Pclose();
    Pdisconnect();
    exit(0);
}
