/*  %W%  %G%  */
/*
**  Written by Mike Higgins
**     Last modified by Dave Gerdes  5/1988
**  US Army Construction Engineering Research Lab
*/

#include "digit.h"

/* delete a line, erasing it from the screen  
**  on entry Gpoints must contain the line 
**  -  updates dig file
**  -  does not delete its attribute
*/

extern struct line_pnts Gpoints;

_remove_line (map, line)
    struct Map_info *map;
    int line;
{
    char type;
    int att;

    if (!LINE_ALIVE (&(map->Line[line])) )
	return (0);
#ifdef AREAS_TOO
    if (map->Line[line].type == AREA)
    {
	/* remove any bounded areas */
	if (map->Line[line].right > 0)
	    Del_area (map, map->Line[line].right);
	if (map->Line[line].right < 0)			/* ISLE */
	    Del_isle (map, ABS(map->Line[line].right));
	if (map->Line[line].left > 0)
	    Del_area (map, map->Line[line].left);
	if (map->Line[line].left < 0)   		/* ISLE */
	    Del_isle (map, ABS (map->Line[line].left));
    }
#endif
    if (0 > dig__Read_line (&Gpoints, map->digit, map->Line[line].offset))
	return (-1);

    dig_node_del_line (&(map->Node[map->Line[line].N1]), line);
    dig_node_del_line (&(map->Node[map->Line[line].N2]), -line);

    /* map->n_lines does not change, as they are still allocated */
    /* till compress () */
    switch(map->Line[line].type)
    {
	case AREA:
	    type = DEAD_AREA;
	    map->n_alines--;
	    break;
	case LINE:
	    type = DEAD_LINE;
	    map->n_llines--;
	    break;
	case DOT:
	    type = DEAD_DOT;
	    map->n_plines--;
	    break;
	default:
/*DEBUG*/ debugf ("REMOVE: BAD 'TYPE' Code.  %d\n", (int) map->Line[line].type);
	    type = DEAD_LINE;
	    break;
    }
    map->Line[line].type = type;
    map->n_points -= Gpoints.n_points;

    /* delete its attribute */
    if (att = map->Line[line].att)
	dig_del_att (map, att);

    dig__Rewrite_line (map->digit, map->Line[line].offset, type, &Gpoints);
}
