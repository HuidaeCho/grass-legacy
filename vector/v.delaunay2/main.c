/***************************************************************
 *
 * MODULE:       v.delaunay
 *
 * AUTHOR(S):    Martin Pavlovsky (Paul Kelly mentor)
 *
 * PURPOSE:      creates a Delaunay triangulation vector map
 *
 * COPYRIGHT:    (C) 2008 by the GRASS Development Team
 *
 *               This program is free software under the
 *               GNU General Public License (>=v2).
 *               Read the file COPYING that comes with GRASS
 *               for details.
 *
 **************************************************************/
#define MAIN
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <grass/gis.h>
#include <grass/Vect.h>
#include <grass/glocale.h>
#include "defs.h"
#include "data_types.h"
#include "memory.h"
#include "geometry.h"
#include "edge.h"
#include "in_out.h"

int main(int argc, char *argv[])
{
    /* GRASS related variables */
    char *mapset;
    struct Map_info In, Out;
    struct GModule *module;
    struct Flag *reg_flag, *line_flag;
    struct Option *in_opt, *out_opt;
    struct Cell_head Window;
    BOUND_BOX Box;

    struct line_pnts *Points;
    struct line_cats *Cats;
    int nareas, area;

    int Type;
    int complete_map;
    int mode3d;

    /* ---------------------- */

    unsigned int n;
    struct edge *l_cw = NULL, *r_ccw = NULL;

    /* GRASS related manipulations */
    G_gisinit(argv[0]);
    module = G_define_module();
    module->keywords = _("vector");
    module->description = _("Creates a Delaunay triangulation from an input "
			    "vector map containing points or centroids.");

    in_opt = G_define_standard_option(G_OPT_V_INPUT);
    out_opt = G_define_standard_option(G_OPT_V_OUTPUT);

    reg_flag = G_define_flag();
    reg_flag->key = 'r';
    reg_flag->description = _("Use only points in current region");

    line_flag = G_define_flag();
    line_flag->key = 'l';
    line_flag->description =
	_("Output triangulation as a graph (lines), not areas");

    if (G_parser(argc, argv))
	exit(EXIT_FAILURE);

    if (line_flag->answer)
	Type = GV_LINE;
    else
	Type = GV_BOUNDARY;

    complete_map = reg_flag->answer ? 0 : 1;

    Points = Vect_new_line_struct();
    Cats = Vect_new_cats_struct();

    /* open files */
    if ((mapset = G_find_vector2(in_opt->answer, "")) == NULL)
	G_fatal_error(_("Vector map <%s> not found"), in_opt->answer);

    Vect_set_open_level(2);
    Vect_open_old(&In, in_opt->answer, mapset);

    /* check if we have a 3D input points map */
    mode3d = Vect_is_3d(&In);

    if (0 > Vect_open_new(&Out, out_opt->answer, mode3d))
	G_fatal_error(_("Unable to create vector map <%s>"), out_opt->answer);

    Vect_hist_copy(&In, &Out);
    Vect_hist_command(&Out);

    /* initialize working region */
    G_get_window(&Window);
    Vect_region_box(&Window, &Box);

    n = read_sites(mode3d, complete_map, In, Box);

    Vect_set_release_support(&In);
    Vect_close(&In);

    if (n < 3)
	G_fatal_error(_("no points to triangulate"));

    /* sort sites */
    qsort(sites, n, sizeof(struct vertex), cmp);

    remove_duplicates(&n);

    /* Triangulate. */
    G_verbose_message("Delaunay triangulation...");
    divide(0, n - 1, &l_cw, &r_ccw);

    G_verbose_message("writing edges...");
    output_edges(n, mode3d, Type, Out);

    free_memory();

    if (Type == GV_BOUNDARY) {
	Vect_build_partial(&Out, GV_BUILD_AREAS);
	nareas = Vect_get_num_areas(&Out);
	G_debug(3, "nareas = %d", nareas);
	/*  Assign centroid to each area */
	G_verbose_message("assign centroids...");
	for (area = 1; area <= nareas; area++) {
	    double x, y, z, angle, slope;
	    int ret;

	    G_percent(area, nareas, 2);
	    Vect_reset_line(Points);
	    Vect_reset_cats(Cats);
	    ret = Vect_get_point_in_area(&Out, area, &x, &y);
	    if (ret < 0) {
		G_warning(_("Unable to calculate area centroid"));
		continue;
	    }
	    ret = Vect_tin_get_z(&Out, x, y, &z, &angle, &slope);
	    G_debug(3, "area centroid z: %f", z);
	    if (ret < 0) {
		G_warning(_("Unable to calculate area centroid z coordinate"));
		continue;
	    }
	    Vect_append_point(Points, x, y, z);
	    Vect_cat_set(Cats, 1, area);
	    Vect_write_line(&Out, GV_CENTROID, Points, Cats);
	}
    }
    Vect_build(&Out);
    Vect_close(&Out);

    exit(EXIT_SUCCESS);
}
