/*
**  Written by Dave Gerdes  5/1988
**  US Army Construction Engineering Research Lab
*/
#include <unistd.h>
#include "Vect.h"
#include "debug.h"
#include "raster.h"
#include "display_line.h"
#include "Map_proto.h"
#include "line_pnts.h"
#include "dig_curses.h"
#include "local_proto.h"

/* returns new line number */
static int _do_break_line (
    struct Map_info *map,
    struct line_pnts *Points,
    int label,
    char type,
    struct new_node *node)
{
    int nline;
    int att;
    double x, y;

/*DEBUG*/ debugf ("calling check_nodes\n");
    /*
    dig_check_nodes (map, &node, Points);
    */
/*DEBUG*/ debugf ("calling new_line\n");
    nline = new_line (map, type, node, Points);
    if (nline < 0)
    {
	BEEP;
	Write_info (2, "Error creating new line.");
	sleep (4);
	return (-1);
    }
    Changes_Made = 1;
    /* put the label back on */
    if (label)
    {
	get_line_center (&x, &y, Points);
	att = dig_new_att (map, x, y, type, nline, label);
	if (att < 0)
	    return (-1);
	map->Line[nline].att = att;
    }
    /*
    display_line (type, Points, nline, map);
    */
    return (nline);
}

int 
break_line (struct Map_info *map)
{
    int line_from;
    int line;
    double ux, uy;

    while (1)
    {
	Clear_info ();

	/* find_line_with_mouse  fills Gpoints */
	line_from = find_line_with_mouse (LINE|AREA, "Line to Break:", NULL);
	line = line_from;
	if (line_from <= 0)
	    return (0);


	get_point (&ux, &uy, "Select break point on line:");
	if ( ux == 0.0 && uy == 0.0)
	{
	    /* reset the highlit line */
	    display_line (map->Line[line].type, &Gpoints, line, map);
	    continue;
	}

	break_line_w_point (map, line, ux, uy);
	/* let break_line redraw the new lines */

	Clear_info();

	Changes_Made = 1;
    }
    /* NOTREACHED */
}

int 
break_line_w_point (struct Map_info *map, int line, double ux, double uy)
{
    struct new_node node;
    char type;
    register int i;
    P_LINE *Line;
    double dist;
    int segment;
    static struct line_pnts Points;
    static struct line_pnts NPoints;
    static int first_time = 1;
    int n_points;
    int label;
    int N1, N2;
    int line1, line2;

    if (first_time)	/* change 10/90   was buggy in 3.1  */
    {
	first_time = 0;
	Points.alloc_points = 0;
	NPoints.alloc_points = 0;
    }

    Line = &(map->Line[line]);

    /* hold on to node numbers for new lines */
    N1 = Line->N1;
    N2 = Line->N2;

    type = Line->type;
    if (0 > (V1_read_line (map, &Points, Line->offset)))
    {
	BEEP;
	Write_info (2, "Error reading line.");
	sleep (3);
	return (-1);
    }

    /* save attribute info */
    /* NOTE for now, I don't bother trying to restore area atts */
    if (Line->att)
    {
	label = map->Att[Line->att].cat;
    }
    else
	label = 0;
    type = Line->type;

    /* segment  ranges from 1 -> */
    segment = dig__check_dist (map, &Points, ux, uy, &dist);
/*DEBUG*/ debugf ("BREAK: segment = %d original points = %d\n", segment, Points.n_points);

    R_standard_color( dcolors[CLR_ERASE]);
    _Blot (&(ux), &(uy));

    /* get nearest point ON line */
    /* replace ux, uy with the line intersection */
    dig_xy_distance2_point_to_line (&ux, &uy, 
	    Points.x[segment-1], Points.y[segment-1], 
	    Points.x[segment], Points.y[segment]);
/*DEBUG*/ debugf ("returned ok from distance\n");

    /* if new intersection is past end of line, blow it off */
    if ((ux == Points.x[0] && uy == Points.y[0]) || 
       (ux == Points.x[Points.n_points-1] && uy == Points.y[Points.n_points-1]))
    {
	display_line (type, &Gpoints, line, map);
	Write_info (2, "Line not broken");
	sleep (2);
	return (-1);
    }

    erase_line (type, &Gpoints, line, map);

    /* get rid of old line */
    /*
    _remove_line (map, line);
    */


    /* do first line */
    n_points = segment + 1;
/*DEBUG*/ debugf ("BREAK: n_points 1 = %d\n", n_points);
    if (0 > dig_alloc_points (&NPoints, n_points))
    {
	Write_info (2, "Out of memory.  Failed");
	return (-1);
    }
    for (i = 0 ; i < n_points-1 ; i++)
    {
	NPoints.x[i] = Points.x[i];
	NPoints.y[i] = Points.y[i];
    }
    NPoints.x[n_points-1] = ux ; 
    NPoints.y[n_points-1] = uy ; 
    NPoints.n_points = n_points;
/*DEBUG*/ debugf ("calling 1st _do_break\n");
    node.N1 = N1;
    node.N2 = map->n_nodes + 1;
    line1 = _do_break_line (map, &NPoints, label, type, &node);
/*DEBUG*/ debugf ("returned ok from 1st _do_break\n");

    /* do second line */
    n_points = Points.n_points - segment + 1;
/*DEBUG*/ debugf ("BREAK: n_points 2 = %d\n", n_points);
    if (0 > dig_alloc_points (&NPoints, n_points))
    {
	Write_info (2, "Out of memory.  Failed");
	sleep (1);
	return (-1);
    }
    NPoints.x[0] = ux ; 
    NPoints.y[0] = uy ; 
    NPoints.n_points = n_points;
    for (i = 1 ; i < n_points ; i++)
    {
	NPoints.x[i] = Points.x[segment-1+i];
	NPoints.y[i] = Points.y[segment-1+i];
    }
/*DEBUG*/ debugf ("calling 2nd _do_break\n");
    node.N1 = node.N2;
    node.N2 = N2;
    line2 = _do_break_line (map, &NPoints, label, type, &node);
/*DEBUG*/ debugf ("returned ok from 2nd _do_break\n");

    /* get rid of old line */
    /* remove old line AFTER adding new lines, to hold on to nodes that
    ** would have been deleted otherwise
    */
    _remove_line (map, line);
    Changes_Made = 1;

    display_line (type, &NPoints, line2, map);
    if (0 >(V1_read_line (map, &NPoints, map->Line[line1].offset)))
    {
	BEEP;
	Write_info (2, "Error reading line.");
	sleep (3);
	return (-1);
    }
    display_line (type, &NPoints, line1, map);
    return (0);
}
