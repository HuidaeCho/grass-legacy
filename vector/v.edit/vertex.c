/****************************************************************
 *
 * MODULE:     v.edit
 *
 * AUTHOR(S):  GRASS Development Team
 *             Jachym Cepicky <jachym  les-ejk cz>
 *             Martin Landa
 *
 * PURPOSE:    This module edits vector maps. 
 *             Vertex operations.
 *
 * COPYRIGHT:  (C) 2006-2007 by the GRASS Development Team
 *
 *             This program is free software under the
 *             GNU General Public License (>=v2).
 *             Read the file COPYING that comes with GRASS
 *             for details.
 *
 ****************************************************************/

#include "global.h"

/**
   \brief Move all vertices in bounding box(es)

   \param[in] Map vector map
   \param[in] BgMap, nbgmaps list of background vector maps for snapping
   \param[in] List list of selected features
   \param[in] coord points location
   \param[in] thresh threshold value (also size of bounding boxes) (>0)
   \param[in] move_x,move_y X,Y direction for moving
   \param[in] move_first move only first vertex found in the bounding box
   \param[in] snap allow snapping (see global.h)

   \return number of moved verteces
   \return -1 on error
 */
int do_move_vertex(struct Map_info *Map, struct Map_info **BgMap, int nbgmaps,
		   struct ilist *List,
		   struct line_pnts* coord, double thresh,
		   double move_x, double move_y,
		   int move_first, int snap)
{
    int nvertices_moved, nlines_modified, nvertices_snapped;

    int i, j, k;
    int line, type, rewrite;
    int npoints;
    double east, north, dist;
    double *x, *y, *z;
    char *moved;

    struct line_pnts *Points, *Points_snap;
    struct line_cats *Cats;
    
    nlines_modified = 0;
    nvertices_moved = nvertices_snapped = 0;
    moved = NULL;

    Points = Vect_new_line_struct();
    Points_snap = Vect_new_line_struct();
    Cats = Vect_new_cats_struct();

    for (i = 0; i < List -> n_values; i++) {
	line = List -> value[i];
	
	if (!Vect_line_alive (Map, line))
	    continue;
	
	type = Vect_read_line(Map, Points, Cats, line);

	if (!(type & GV_LINES))
	    continue;

	npoints = Points -> n_points;
	x = Points -> x;
	y = Points -> y;
	z = Points -> z;

	/* vertex moved 
	   0 not moved
	   1 moved
	   2 moved and snapped
	 */
	moved = (char *) G_realloc ((void *) moved, Points -> n_points * sizeof (char));
	G_zero ((void *) moved, Points -> n_points * sizeof (char));
	
	rewrite = 0;
	for (j = 0; j < coord -> n_points; j++) {
	    east  = coord->x[j];
	    north = coord->y[j];
	    
	    /* move all vertices in the bounding box */
	    for (k = 0; k < Points -> n_points; k++) {
		if (moved[k] == 0) {
		    dist = Vect_points_distance (east, north, 0.0,
						 x[k], y[k], z[k],
						 WITHOUT_Z);
		    if (dist <= thresh) {
			G_debug (3, "do_move_vertex(): line=%d; x=%f, y=%f -> x=%f, y=%f",
				 line, x[k], y[k], x[k] + move_x, y[k] + move_y);
			x[k] += move_x;
			y[k] += move_y;
			moved[k] = 1;

			G_debug (3, "line=%d, point=%d moved", line, k);
			
			if (snap != NO_SNAP) {
			    if (do_snap_point(Map, line, &x[k], &y[k], &z[k], thresh,
					      (snap == SNAPVERTEX) ? 1 : 0) == 0) {
				/* check also background maps */
				int bgi;
				for (bgi = 0; bgi < nbgmaps; bgi++) {
				    if (do_snap_point(BgMap[i], line, &x[k], &y[k], &z[k], thresh,
						      (snap == SNAPVERTEX) ? 1 : 0))
					moved[k] = 2;
					break; /* snapped, don't continue */
				}
			    }
			    else {
				moved[k] = 2;
			    }
			}
			
			rewrite = 1;
			nvertices_moved++;

			if (move_first)
			    break;
		    }
		}
	    } /* for each line vertex */

	    /* close line or boundary */
	    if ((type & GV_LINES) &&
		Vect_points_distance(x[0], y[0], z[0],
				     x[npoints-1], y[npoints-1], z[npoints-1],
				     WITHOUT_Z) <= thresh) {

		if (moved[0] == 1) { /* first node moved */
		    x[0] = x[npoints-1];
		    y[0] = y[npoints-1];
		    z[0] = z[npoints-1];
		}
		else if (moved[npoints-1] == 1) { /* last node moved */
		    x[npoints-1] = x[0];
		    y[npoints-1] = y[0];
		    z[npoints-1] = z[0];
		}
	    }
	} /* for each coord */
	
	if (rewrite) {
	    if (Vect_rewrite_line (Map, line, type, Points, Cats) < 0)  {
		G_warning(_("Unable to rewrite line %d"), line);
		return -1;
	    }
	    
	    nlines_modified++;
	}
    } /* for each selected line */

    /* destroy structures */
    Vect_destroy_line_struct(Points);
    Vect_destroy_line_struct(Points_snap);
    Vect_destroy_cats_struct(Cats);
/*     G_free ((void *) moved); */

    return nvertices_moved;
}

/**
   \brief Add new vertex to line.

   Shape of line is not changed.
 
   \param[in] Map vector map
   \param[in] List list of features
   \param[in] coord points location
   \param[in] thresh find line in given threshold

   \return number of add verteces
   \return -1 on error
*/
int do_add_vertex (struct Map_info *Map, struct ilist *List,
		   struct line_pnts* coord, double thresh)
{
    int i, j;
    int type, line, seg;
    int nlines_modified, nvertices_added, rewrite;
    double east, north, dist;
    double *x, *y, *z;
    double px, py;

    struct line_pnts *Points;
    struct line_cats *Cats;

    nlines_modified = 0;
    nvertices_added = 0;
    Points  = Vect_new_line_struct();
    Cats    = Vect_new_cats_struct();

    for (i = 0; i < List -> n_values; i++) {
	line = List -> value[i];

	if (!Vect_line_alive (Map, line))
	    continue;

        type = Vect_read_line(Map, Points, Cats, line);

	if (!(type & GV_LINES))
	    continue;

	x = Points -> x;
	y = Points -> y;
	z = Points -> z;
	rewrite = 0;
	for (j = 0; j < coord -> n_points; j++) {
	    east  = coord->x[j];
	    north = coord->y[j];

	    seg = Vect_line_distance (Points,
				      east, north, 0.0,    /* standpoint */
				      WITHOUT_Z,
				      &px, &py, NULL,      /* point on line */
				      &dist,               /* distance to line */
				      NULL, NULL);
	    
	    if (dist <= thresh &&
		Vect_points_distance (px, py, 0.0, x[seg], y[seg], z[seg], WITHOUT_Z) > 0 &&
		Vect_points_distance (px, py, 0.0, x[seg-1], y[seg-1], z[seg-1], WITHOUT_Z) > 0) {
		/* add new vertex */
		Vect_line_insert_point (Points, seg, px, py, 0.0);
		G_debug (3, "do_add_vertex(): line=%d; x=%f, y=%f, index=%d", line, px, py, seg);
		rewrite = 1;
		nvertices_added++;
	    }
	} /* for each point */

	/* rewrite the line */
	if (rewrite) {
	    Vect_line_prune (Points);
	    if (Vect_rewrite_line (Map, line, type, Points, Cats) < 0) {
		G_warning(_("Unable to rewrite line %d"), line);
		return -1;
	    }
		
	    nlines_modified++;
	}
    } /* for each line */

    /* destroy structures */
    Vect_destroy_line_struct(Points);
    Vect_destroy_cats_struct(Cats);

    return nvertices_added;
}

/**
   \brief Remove vertex from line
   
   \param[in] Map vector map
   \param[in] List list of selected features
   \param[in] coord points location
   \param[in] thresh threshold value to find a line

   \return number of removed vertices
   \return -1 on error
*/
int do_remove_vertex(struct Map_info *Map, struct ilist *List,
		     struct line_pnts *coord, double thresh)
{
    int i, j, k;
    int type, line;
    int nvertices_removed, rewrite, nlines_modified;
    double east, north;
    double dist;
    double *x, *y, *z;

    struct line_pnts *Points;
    struct line_cats *Cats;

    nvertices_removed = nlines_modified = 0;

    Points = Vect_new_line_struct();
    Cats = Vect_new_cats_struct();
    
    for (i = 0; i < List -> n_values; i++) {
	line = List -> value[i];
	
	if (!Vect_line_alive (Map, line))
	    continue;
	
        type = Vect_read_line(Map, Points, Cats, line);

	if (!(type & GV_LINES))
	    continue;

	x = Points -> x;
	y = Points -> y;
	z = Points -> z;
	rewrite = 0;
	for (j = 0; j < coord -> n_points; j++) {
	    east  = coord->x[j];
	    north = coord->y[j];
	    
	    for (k = 0; k < Points -> n_points; k++) {
		dist = Vect_points_distance (east, north, 0.0,
					     x[k], y[k], z[k],
					     WITHOUT_Z);
		if (dist <= thresh) {
		    /* remove vertex */
		    Vect_line_delete_point (Points, k);
		    G_debug (3, "do_remove_vertex(): line=%d; x=%f, y=%f, index=%d", line, x[k], y[k], k);
		    k--;
		    nvertices_removed++;
		    rewrite = 1;
		}
	    } /* for each point */
	} /* for each bounding box */
	
	if (rewrite) {
	    /* rewrite the line */
	    if (Vect_rewrite_line (Map, line, type, Points, Cats) < 0) {
		G_warning (_("Unable to rewrite line %d"), line);
		return -1;
	    }
	    
	    nlines_modified++;
	}
    } /* for each line */

    /* destroy structures */
    Vect_destroy_line_struct(Points);
    Vect_destroy_cats_struct(Cats);
    
    return nvertices_removed;
}
