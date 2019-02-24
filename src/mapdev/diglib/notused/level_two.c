/*
**  Written by: Dave Gerdes 5 1988
**  US Army Construction Engineering Research Lab
**
**  Modified by Dave Gerdes  1/1991  for  dig_head/fileno stuff
*/

/*
**
**INTERFACE LEVEL II
**==================
**
*/

#include "digit.h"
#include "dig_head.h"
#include "gis.h"


static int Writable = 0;	/* Open Plus file for WRITE/READONLY */
static char *RW_str = "r";
			

Vect_P_init (name, mapset, map)
    char *name;
    char *mapset; 
    struct Map_info *map; 
{
    char *error;

    if (NULL != (error = Vect__P_init (map, name, mapset)))
    {
	fprintf (stderr, "%s\n", error);
	exit (-1);
    }
    return (0);
}

/***************************************************************************/
/*     Undocumented Feature:  I.E.  use at your own risk! 
*/

/*
**  Simply ensure that files are opened as read/write.  everything
**  else is up to programmer.  Note that dig_att file is not openned
**  by P_init ().
**
**  This is used buy Vcadlabel and Vclean to modify an existing
**    dig_plus file.  You have to know what is going on in the background
**    to safely use this.
** 
**   The standard method of creating and modifying dig files
**    is to use only the level_one I/O calls to read and write dig files
**    and att files only.
*/
Vect__P_writeable (x)
    int x;
{
    Writable = x;
    if (x)
	RW_str = "r+";
    else
	RW_str = "r";
}

/***************************************************************************/



/* 
**  Returns NULL   or a pointer to an error message
*/
char *
Vect__P_init (map, name, mapset)
    char *name;
    char *mapset; 
    struct Map_info *map; 
{
    FILE *plusfp;
    int have_old, have_plus, have_attr;
    char file[100];

    Vect_init ();

    /* need dig open,  need to open plus for load_plus, and att for verify */


    have_old = have_plus = have_attr = 0;
    G__file_name (file, "dig", name, mapset);
    map->digit_file = G_store (file);
    if ( (map->dig_fp = fopen(file, RW_str)) != NULL )
	have_old = 1;
    else
	return ("Cannot open digit file");



    G__file_name (file, "dig_plus", name, mapset);
    map->plus_file = G_store (file);


    if ((plusfp = fopen (file, RW_str)) != NULL)
    {
	fclose (plusfp);
	have_plus = 1;
    }
    else
	return ("Cannot open dig_plus file");


    G__file_name (file, "dig_att", name, mapset);
    map->att_file = G_store (file);

    /*  set up level_one access to head structure */
    /*dig__set_head (map->dig_fp, &(map->head));*/

    Vect__read_head_binary(map, &(map->head));



    map->Line = NULL;
    map->Area = NULL;
    map->Isle = NULL;
    map->Att = NULL;
    map->Node = NULL;

    if (0 > dig_load_plus (map, map->dig_fp, 1))
	return ("Error reading dig_plus file");

    /* OK */
    return (NULL);
}



dig_P_num_lines (map)
    struct Map_info *map;
{
    return (map->n_lines);
}

dig_P_num_areas (map)
    struct Map_info *map;
{
    return (map->n_areas);
}


/* returns category of line */
/* or 0 on any error */
dig_P_line_att (map, line)
    struct Map_info *map;
    int line;
{
    P_LINE *Line;

    Line = &(map->Line[line]);

    if (line <= 0 || line > map->n_lines || !LINE_ALIVE (Line) || ! Line->att)
	return (0);
    return (map->Att[Line->att].cat);
}
dig_P_area_att (map, area)
    struct Map_info *map;
    int area;
{
    P_AREA *Area;

    if (area <= 0 || area > map->n_areas)
	return (0);

    Area = &(map->Area[area]);
    if (!AREA_ALIVE (Area) || ! Area->att)
	return (0);
    return (map->Att[Area->att].cat);
}

/* returns -1 on error */
/* note all areas may not be labeled */
/* use get_area_att  > 0 for test of labelled */

dig_P_get_area (map, num, Area)
    struct Map_info *map;
    P_AREA **Area;
    int num;
{
    if (num <= 0 || num > map->n_areas)
	return (-1);
    if (!AREA_ALIVE (&(map->Area[num])))
	return (-1);
    *Area = &(map->Area[num]);
    return (0);
}

/* get Area bounding box info in NSEW */
dig_P_get_area_bbox (map, area, N, S, E, W)
    struct Map_info *map;
    int area;
    double *N, *S, *E ,*W;
{
    P_AREA *Area;

    if (area <= 0 || area > map->n_areas)
	return (-1);
    if (!AREA_ALIVE (&(map->Area[area])))
	return (-1);
    Area = &(map->Area[area]);
    *N = Area->N;
    *E = Area->E;
    *W = Area->W;
    *S = Area->S;
    return (0);
}

/* get Line bounding box info in NSEW */
dig_P_get_line_bbox (map, line, N, S, E, W)
    struct Map_info *map;
    int line;
    double *N, *S, *E ,*W;
{
    P_LINE *Line;

    if (line <= 0 || line > map->n_lines)
	return (-1);
    if (!LINE_ALIVE (&(map->Line[line])))
	return (-1);
    Line = &(map->Line[line]);
    *N = Line->N;
    *E = Line->E;
    *W = Line->W;
    *S = Line->S;
    return (0);
}
