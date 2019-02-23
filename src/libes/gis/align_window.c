#include "gis.h"
/*
 * char *
 * G_align_window (window, ref)
 *     struct Cell_head *window, *ref;
 *
 * Modifies 'window' to have the same resolution as 'ref'
 * and aligns the window edges (north, south, east, west) to
 * align with the 'ref' grid.
 *
 * Returns: NULL if ok,
 *          error message if not (do NOT free this message).
 */
char *
G_align_window (window, ref)
    struct Cell_head *window, *ref;
{
    double floor(), ceil();
    double G_northing_to_row();
    double G_row_to_northing();
    double G_easting_to_col();
    double G_col_to_easting();
    char *G_adjust_Cell_head();

    window->ns_res = ref->ns_res;
    window->ew_res = ref->ew_res;

    window->north =
	G_row_to_northing (floor(G_northing_to_row (window->north, ref)), ref);
    window->south =
	G_row_to_northing (ceil(G_northing_to_row (window->south, ref)), ref);
    window->east =
	G_col_to_easting (floor(G_easting_to_col (window->east, ref)), ref);
    window->west =
	G_col_to_easting (ceil(G_easting_to_col (window->west, ref)), ref);
    
    return G_adjust_Cell_head (window, 0, 0);
}
