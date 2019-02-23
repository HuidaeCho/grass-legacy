/*  @(#)p_to_line.c    2.1  6/26/87  */
#include "digit.h"
#include <math.h>

/*
*  functions: point_to_area(),
*  point_to_area() loops thru all the areas, calling in_area_bbox() to see
*  if the line is in the area bounding_box. and if it is the closest area
*  so far.
*/

/* returns area number or 0 if one not found 
*/
int
dig_point_to_area (map, ux, uy)
    struct Map_info *map;
    double ux, uy;
{
    register int area;
    int choice;
    char buffer[128];
    double new_dist;
    double cur_dist;
    int gotone;
    P_AREA *Area;

    gotone = 0;
    choice = 0;

    for(area = 1 ; area <= map->n_areas ; area++)
    {
	Area = &(map->Area[area]);
	if (AREA_ALIVE (Area))
	{
#ifdef FOO
/*DEBUG*/ fprintf (stderr, "checking Area %d (%lf, %lf)\n", area, ux, uy);
#endif
	    if (dig_in_area_bbox (Area, ux, uy))
	    {
		if (0.0 != (new_dist = dig_point_in_area (map, ux, uy, Area)))
		{
		    if ( (++gotone == 1) || (new_dist <= cur_dist) )
		    {
			/* if (new_dist == cur_dist) ... ; else */
			{
			    choice = area;
			    cur_dist = new_dist;
			}
		    }
		}
	    }
	}
    }

    return (choice);
}

/* 
** min_dist returns the distance point is from area.
** must be passed a -1.0 to start with.  It can be used to limit
** the search area to areas whose minimum distance is farther than min_dist
*/

int
dig_point_to_next_area (map, ux, uy, min_dist)
    struct Map_info *map;
    double ux, uy;
    double *min_dist;
{
    register int area;
    int choice;
    char buffer[128];
    double new_dist;
    double cur_dist;
    int gotone;
    P_AREA *Area;

    gotone = 0;
    choice = 0;

    for(area = 1 ; area <= map->n_areas ; area++)
    {
	Area = &(map->Area[area]);
	if (AREA_ALIVE (Area))
	{
#ifdef FOO
/*DEBUG*/ fprintf (stderr, "pnt_t_n_area: checking Area %d (%lf, %lf)\n", area, ux, uy);
#endif
	    if (dig_in_area_bbox (Area, ux, uy))
	    {
/*DEBUG*/ 
/*DEBUG*/ debugf ("found in BBOX for area %d\n", area);
		if (0.0 != (new_dist = dig_point_in_area (map, ux, uy, Area)))
		{
/*DEBUG*/ debugf ("point WAS in area %d\n", area);
		    if (new_dist > *min_dist)
		    {
			if ( (++gotone == 1) || (new_dist <= cur_dist) )
			{
			    /* if (new_dist == cur_dist) ... ; else */
			    {
				choice = area;
				cur_dist = new_dist;
			    }
			}
		    }
		}
	    }
	}
    }

    *min_dist = cur_dist;
    return (choice);
}
