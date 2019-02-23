/* %W% %G% */
/*
**  Written by Dave Gerdes  6/89
**  US Army Construction Engineering Research Lab
*/
#include "digit.h"
#include "dig_head.h"
#include "gis.h"
#include "export_dlg.h"

#define LINE1_FMT  "%30.30s %20.20s %20.20s\n"
#define LINE2_FMT  "%40.40s %10.10s%c%8ld            \n"
#define LINE3_FMT  "Produce by GRASS Export_DLG                                             \n"
#define LINE4_FMT  "%6d%6d%6d%6d%-18s%6d%6d%6d%6d      \n"
#define LINE5_FMT  "%-24s%-24s%-24s\n"
#define LINE10_FMT "%18s%18s%18s%18s\n"
#define LINE11_FMT "%2s    %12.6lf%12.6lf      %12.2lf%12.2lf            \n"
#define LINE15_FMT "%20s%4d%6d%6d %c%c %6d%6d %c%c%c%6d%6d   %c\n"

/*
#define LINE4_FMT  "%6d%6d%6d%6d%18.11le%6d%6d%6d%6d      \n"
#define LINE5_FMT  "%24.15le%24.15le%24.15le\n"
#define LINE10_FMT "%18.11le%18.11le%18.11le%18.11le\n"
#define LINE11_FMT "%2s    %12.6lf%12.6lf      %12.2lf%12.2lf            \n"
#define LINE15_FMT "%20s%4d%6d%6d %c%c %6d%6d %c%c%c%6d%6d   %c\n"
*/


char *dtype ();
write_dlg_head (map, head, fp)
    struct Map_info *map;
    struct head *head;
    FILE *fp;
{
    line1 (map, head, fp);
    line2 (map, head, fp);
    line3 (map, head, fp);
    line4 (map, head, fp);
    line5_9 (map, head, fp);
    line10 (map, head, fp);
    line11_14 (map, head, fp);
    line15 (map, head, fp);
}

line1 (map, head, fp)
    struct Map_info *map;
    struct head *head;
    FILE *fp;
{
    fprintf (fp, LINE1_FMT, head->organization, head->source_date, head->your_name);
}

line2 (map, head, fp)
    struct Map_info *map;
    struct head *head;
    FILE *fp;
{
    fprintf (fp, LINE2_FMT, head->map_name, head->date, ' ', head->orig_scale);
}

line3 (map, head, fp)
    struct Map_info *map;
    struct head *head;
    FILE *fp;
{
    /*
    fprintf (fp, LINE3_FMT);
    */
    if (strlen (head->line_3) > 72)
	head->line_3[73] = 0;
    fprintf (fp, "%72s\n", head->line_3);
}

#define DLG3_CODE 	3

#define OTH_CODE 	0
#define UTM_CODE 	1
#define SP_CODE 	2  /* guess */

#define METERS_CODE 	2
#define FEET_CODE 	1  /* guess */

#define NUM_TRANSFORMS	4
#define NUM_SIDES	4
#define NUM_CATS	1
#define DIG_UNITS	0.001

line4 (map, head, fp)
    struct Map_info *map;
    struct head *head;
    FILE *fp;
{
    double resolution;
    int units, misc, proj;

    switch (G_projection ()) {
	case 0:				/* XY */
	    proj = OTH_CODE;
	    break;
	case 1:				/* UTM */
	    proj = UTM_CODE;
	    break;
	case 2:				/* State Plane */
	    proj = SP_CODE;
	    break;
	default:
	    proj = UTM_CODE;
	    break;
    }
    switch (G_projection_units (G_projection())) {
	case 0:
	    units = OTH_CODE;
	    break;
	case METERS:
	    units = METERS_CODE;
	    break;
	case FEET:
	    units = FEET_CODE;
	    break;
	default:
	    units = METERS_CODE;
	    break;
    }
    misc = 0;
    resolution = head->orig_scale * dig_unit_conversion () * DIG_UNITS;

    fprintf (fp, LINE4_FMT, 
	DLG3_CODE, 		/* DLG-3 format */
	proj, 			/* Projection (UTM) */
	head->plani_zone,	/* UTM ZONE */
	units, 			/* Units */
	dtype (resolution, 18, 11), 		/* resolution */
	NUM_TRANSFORMS, 	/* # of tranformation params */
	misc, 			/* # of accuracy/misc records */
	NUM_SIDES, 		/* # of sides to window polygon */
	NUM_CATS);		/* # of DLG categories */
}

line5_9 (map, head, fp)
    struct Map_info *map;
    struct head *head;
    FILE *fp;
{
    register int i;

    for (i = 5 ; i <= 9 ; i++)
    {
	switch (i) {
	    case 5:
		fprintf (fp, LINE5_FMT, dtype(0.0, 14, 15), dtype(0.0, 14, 15), dtype(0.0, 14, 15));
		break;
	    default:
		fprintf (fp, LINE5_FMT, dtype(0.0, 14, 15), dtype(0.0, 14, 15), dtype(0.0, 14, 15));
		break;
	}
    }
}

line10 (map, head, fp)
    struct Map_info *map;
    struct head *head;
    FILE *fp;
{
    fprintf (fp, LINE10_FMT, dtype(1.0, 18, 11), dtype(0.0, 18, 11), dtype(0.0, 18, 11), dtype(0.0, 18, 11));
}

line11_14 (map, head, fp)
    struct Map_info *map;
    struct head *head;
    FILE *fp;
{
    register int i;

    fprintf (fp, LINE11_FMT, "SW", 0.0, 0.0, map->Area[1].W, map->Area[1].S);
    fprintf (fp, LINE11_FMT, "NW", 0.0, 0.0, map->Area[1].W, map->Area[1].N);
    fprintf (fp, LINE11_FMT, "NE", 0.0, 0.0, map->Area[1].E, map->Area[1].N);
    fprintf (fp, LINE11_FMT, "SE", 0.0, 0.0, map->Area[1].E, map->Area[1].S);
}

line15 (map, head, fp)
    struct Map_info *map;
    struct head *head;
    FILE *fp;
{
    fprintf (fp, LINE15_FMT, 
	"",			/* Category name */
	0, 			/* Default formating Maj/Min pairs */
	map->n_nodes,		/* # of nodes in file */
	map->n_nodes,		/* # of nodes in file */
	'0',			/* node-area links */
	'1',			/* node-line links */
	map->n_areas,		/* # of areas in file */
	map->n_areas,		/* # of areas in file */
	'0',			/* area-node links */
	'1',			/* area-line links */
	'0',			/* area-coord lists */
	map->n_lines,		/* # of lines in file */
	map->n_lines,		/* # of lines in file */
	'1');			/* line-coord lists */
}

/* uses a circular buffer of 200 bytes */
char *
dtype(value, width, deci)
        double value ;
        int width ;
        int deci ;
{
    static char buff[200];
    static char *p = NULL;
    char *ret;
    char form[64] ;
    char *strchr() ;

    if (p == NULL)
	p = buff;
    if ((buff+200-1) - p < width+1)
	p = buff;
    
    {
	sprintf(form,"%%%d.%dle",width, deci) ;
	sprintf(p,form,value) ;
	*(strchr(p,'e')) = 'D' ;
    }
    ret = p;
    p += strlen (p) + 1;
    return (ret);
}
