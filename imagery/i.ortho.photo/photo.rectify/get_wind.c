#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "global.h"

int get_target_window(void)
{
    char name[30], mapset[30];
    struct Cell_head cellhd;

    fprintf(stderr, "\n\n");
    while (1) {
	char buf[100];

	fprintf(stderr, "Please select one of the following options\n");
	fprintf(stderr,
		" 1. Use the current window in the target location\n");
	fprintf(stderr,
		" 2. Determine the smallest window which covers the image\n");
	fprintf(stderr, "> ");
	if (!G_gets(buf))
	    continue;
	G_strip(buf);

	if (strcmp(buf, "1") == 0) {

	    /**ask_window (&target_window);**/
	    return 1;
	}
	if (strcmp(buf, "2") == 0)
	    break;
    }
    ask_file_from_list(name, mapset);

    G_debug(1, "ask_file: %s in %s", name, mapset);

    if (G_get_cellhd(name, mapset, &cellhd) < 0)
	exit(EXIT_FAILURE);


    G_debug(1, "current window: n s = %f %f,", cellhd.north,
	    cellhd.south);
    G_debug(1, "current window: w e = %f %f,", cellhd.west,
	    cellhd.east);

    georef_window(&cellhd, &target_window);
    ask_window(&target_window);

/**
    if(!G_yes("Would you like this window saved as the window in the target location?\n", -1))
	return 0;
**/
    select_target_env();
    if (G_put_window(&target_window) >= 0)
	fprintf(stderr, "Window Saved!\n");
    select_current_env();
    return 0;
}

int georef_window(struct Cell_head *w1, struct Cell_head *w2)
{
    double n, e, z1, ad;
    double n0, e0;
    double aver_z;
    struct _corner {
        double n, e;
    } nw, ne, se, sw;

    /* get an average elevation from the active control points */
    get_aver_elev(&group.control_points, &aver_z);
    G_debug(1, "Aver elev = %f", aver_z);

    /* compute ortho ref of all corners */

    I_georef(w1->west, w1->north, &e0, &n0, group.E12, group.N12);
    I_inverse_ortho_ref(e0, n0, aver_z, &e, &n, &z1, &group.camera_ref,
			group.XC, group.YC, group.ZC, group.omega, group.phi,
			group.kappa);

    G_debug(1, "NORTH WEST CORNER");
    G_debug(1, "group.E12 = %f %f %f,", group.E12[0], group.E12[1],
	    group.E12[2]);
    G_debug(1, "group.N12 = %f %f %f,", group.N12[0], group.N12[1],
	    group.N12[2]);
    G_debug(1, "image  x = %f y = %f, photo x = %f y = %f", w1->west,
	    w1->north, e0, n0);
    G_debug(1, "target x = %f y = %f", e, n);

    w2->north = w2->south = n;
    w2->west = w2->east = e;
    nw.n = n;
    nw.e = e;

    I_georef(w1->east, w1->north, &e0, &n0, group.E12, group.N12);
    I_inverse_ortho_ref(e0, n0, aver_z, &e, &n, &z1, &group.camera_ref,
			group.XC, group.YC, group.ZC, group.omega, group.phi,
			group.kappa);

    G_debug(1, "NORTH EAST CORNER");
    G_debug(1, "image  x = %f y = %f, photo x = %f y = %f", w1->east,
	    w1->north, e0, n0);
    G_debug(1, "target x = %f y = %f", e, n);


    ne.n = n;
    ne.e = e;
    if (n > w2->north)
	w2->north = n;
    if (n < w2->south)
	w2->south = n;
    if (e > w2->east)
	w2->east = e;
    if (e < w2->west)
	w2->west = e;

    I_georef(w1->west, w1->south, &e0, &n0, group.E12, group.N12);
    I_inverse_ortho_ref(e0, n0, aver_z, &e, &n, &z1, &group.camera_ref,
			group.XC, group.YC, group.ZC, group.omega, group.phi,
			group.kappa);

    G_debug(1, "SOUTH WEST CORNER");
    G_debug(1, "image  x = %f y = %f, photo x = %f y = %f", w1->west,
	    w1->south, e0, n0);
    G_debug(1, "target x = %f y = %f", e, n);

    sw.n = n;
    sw.e = e;
    if (n > w2->north)
	w2->north = n;
    if (n < w2->south)
	w2->south = n;
    if (e > w2->east)
	w2->east = e;
    if (e < w2->west)
	w2->west = e;

    I_georef(w1->east, w1->south, &e0, &n0, group.E12, N12);
    I_inverse_ortho_ref(e0, n0, aver_z, &e, &n, &z1, &group.camera_ref,
			group.XC, group.YC, group.ZC, group.omega, group.phi,
			group.kappa);

    G_debug(1, "SOUTH EAST CORNER");
    G_debug(1, "image  x = %f y = %f, photo x = %f y = %f", w1->east,
	    w1->south, e0, n0);
    G_debug(1, "target x = %f y = %f", e, n);

    se.n = n;
    se.e = e;
    if (n > w2->north)
	w2->north = n;
    if (n < w2->south)
	w2->south = n;
    if (e > w2->east)
	w2->east = e;
    if (e < w2->west)
	w2->west = e;

    /* this results in ugly res values, and ns_res != ew_res */
    /* and is no good for rotation */
    /*
    w2->ns_res = (w2->north - w2->south) / w1->rows;
    w2->ew_res = (w2->east - w2->west) / w1->cols;
    */

    /* alternative: account for rotation and order > 1 */

    /* N-S extends along western and eastern edge */
    w2->ns_res = (sqrt((nw.n - sw.n) * (nw.n - sw.n) +
		      (nw.e - sw.e) * (nw.e - sw.e)) +
		 sqrt((ne.n - se.n) * (ne.n - se.n) +
		      (ne.e - se.e) * (ne.e - se.e))) / (2.0 * w1->rows);

    /* E-W extends along northern and southern edge */
    w2->ew_res = (sqrt((nw.n - ne.n) * (nw.n - ne.n) +
		      (nw.e - ne.e) * (nw.e - ne.e)) +
		 sqrt((sw.n - se.n) * (sw.n - se.n) +
		      (sw.e - se.e) * (sw.e - se.e))) / (2.0 * w1->cols);

    /* make ew_res = ns_res */
    w2->ns_res = (w2->ns_res + w2->ew_res) / 2.0;
    w2->ew_res = w2->ns_res;

    /* nice round values */
    if (w2->ns_res > 1) {
	if (w2->ns_res < 10) {
	    /* round to first decimal */
	    w2->ns_res = (int)(w2->ns_res * 10 + 0.5) / 10.0;
	    w2->ew_res = w2->ns_res;
	}
	else {
	    /* round to integer */
	    w2->ns_res = (int)(w2->ns_res + 0.5);
	    w2->ew_res = w2->ns_res;
	}
    }

    /* adjust extends */
    ad = w2->north > 0 ? 0.5 : -0.5;
    w2->north = (int) (ceil(w2->north / w2->ns_res) + ad) * w2->ns_res;
    ad = w2->south > 0 ? 0.5 : -0.5;
    w2->south = (int) (floor(w2->south / w2->ns_res) + ad) * w2->ns_res;
    ad = w2->east > 0 ? 0.5 : -0.5;
    w2->east = (int) (ceil(w2->east / w2->ew_res) + ad) * w2->ew_res;
    ad = w2->west > 0 ? 0.5 : -0.5;
    w2->west = (int) (floor(w2->west / w2->ew_res) + ad) * w2->ew_res;

    w2->rows = (w2->north - w2->south + w2->ns_res / 2.0) / w2->ns_res;
    w2->cols = (w2->east - w2->west + w2->ew_res / 2.0) / w2->ew_res;

    G_debug(1, "FINAL");
    G_debug(1, "east = %f \n west = %f \n north = %f \n south = %f",
	    w2->east, w2->west, w2->north, w2->south);
    G_debug(1, "RESOLUTION");
    G_debug(1, "EW = %f", w2->ew_res);
    G_debug(1, "NS = %f", w2->ns_res);

    return 0;
}
