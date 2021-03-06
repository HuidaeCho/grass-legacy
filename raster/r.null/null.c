/****************************************************************************
 *
 * MODULE:       r.null
 * AUTHOR(S):    U.S.Army Construction Engineering Research Laboratory
 * PURPOSE:      Manages NULL-values of given raster map.
 * COPYRIGHT:    (C) 2008, 2011 by the GRASS Development Team
 *
 *               This program is free software under the GNU General
 *               Public License (>=v2). Read the file COPYING that
 *               comes with GRASS for details.
 *
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <grass/gis.h>
#include <grass/glocale.h>
#include "mask.h"
#include "local_proto.h"

d_Mask d_mask;
DCELL new_null;

static struct Cell_head cellhd;
static int parse_vallist(char **, d_Mask *);

int main(int argc, char *argv[])
{
    char *name, *mapset;
    char rname[GNAME_MAX], rmapset[GMAPSET_MAX];
    char path[GPATH_MAX];
    int row, col, null_fd;
    unsigned char *null_bits;
    RASTER_MAP_TYPE map_type;
    int change_null = FALSE, create, remove, only_int, only_fp, only_null;
    int is_reclass;

    struct GModule *module;
    struct
    {
	struct Option *map;
	struct Option *setnull;
	struct Option *null;
    } parms;
    struct
    {
	struct Flag *f;
	struct Flag *n;
	struct Flag *i;
	struct Flag *c;
	struct Flag *r;
    } flags;

    G_gisinit(argv[0]);

    module = G_define_module();
    module->keywords = _("raster, null data");
    module->description = _("Manages NULL-values of given raster map.");

    parms.map = G_define_standard_option(G_OPT_R_MAP);
    parms.map->description = _("Name of raster map for which to edit null file");

    parms.setnull = G_define_option();
    parms.setnull->key = "setnull";
    parms.setnull->key_desc = "val[-val]";
    parms.setnull->type = TYPE_STRING;
    parms.setnull->required = NO;
    parms.setnull->multiple = YES;
    parms.setnull->description = _("List of cell values to be set to NULL");
    parms.setnull->guisection = _("Modify");
 
    parms.null = G_define_option();
    parms.null->key = "null";
    parms.null->type = TYPE_DOUBLE;
    parms.null->required = NO;
    parms.null->multiple = NO;
    parms.null->description = _("The value to replace the null value by");
    parms.null->guisection = _("Modify");

    flags.f = G_define_flag();
    flags.f->key = 'f';
    flags.f->description = _("Only do the work if the map is floating-point");
    flags.f->guisection = _("Check");

    flags.i = G_define_flag();
    flags.i->key = 'i';
    flags.i->description = _("Only do the work if the map is integer");
    flags.i->guisection = _("Check");

    flags.n = G_define_flag();
    flags.n->key = 'n';
    flags.n->description =
	_("Only do the work if the map doesn't have a NULL-value bitmap file");
    flags.n->guisection = _("Check");

    flags.c = G_define_flag();
    flags.c->key = 'c';
    flags.c->description =
	_("Create NULL-value bitmap file validating all data cells");

    flags.r = G_define_flag();
    flags.r->key = 'r';
    flags.r->description = _("Remove NULL-value bitmap file");
    flags.r->guisection = _("Remove");

    if (G_parser(argc, argv))
	exit(EXIT_FAILURE);

    only_int = flags.i->answer;
    only_fp = flags.f->answer;
    only_null = flags.n->answer;
    create = flags.c->answer;
    remove = flags.r->answer;

    name = parms.map->answer;
    mapset = G_find_cell2(name, "");
    if (mapset == NULL)
	G_fatal_error(_("Raster map <%s> not found"), name);

    is_reclass = (G_is_reclass(name, mapset, rname, rmapset) > 0);
    if (is_reclass)
	G_fatal_error(_("Raster map <%s> is a reclass of map <%s@%s>. "
			"Consider to generate a copy with r.mapcalc. Exiting."),
		      name, rname, rmapset);


    if (strcmp(mapset, G_mapset()) != 0)
	G_fatal_error(_("Raster map <%s> is not in your mapset <%s>"),
		      name, G_mapset());
    
    if (parms.null->answer) {
	if (sscanf(parms.null->answer, "%lf", &new_null) == 1)
	    change_null = TRUE;
	else
	    G_fatal_error(_("%s is illegal entry for null"),
			  parms.null->answer);
    }

    map_type = G_raster_map_type(name, mapset);

    if (only_null && G_find_file_misc("cell_misc", "null", name, mapset))
	G_fatal_error(_("Raster map <%s> already has a null bitmap file"), name);

    if (map_type == CELL_TYPE) {
	if (only_fp)
	    G_fatal_error(_("<%s> is integer raster map (CELL)"),
			  name);

	if ((double)((int)new_null) != new_null) {
	    G_warning(_("<%s> is integer raster map (CELL). Using null=%d."),
		      name, (int)new_null);
	    new_null = (double)((int)new_null);
	}
    }
    else if (only_int)
	G_fatal_error(_("<%s> is floating pointing raster map"),
		      name);

    parse_vallist(parms.setnull->answers, &d_mask);

    if (G_get_cellhd(name, mapset, &cellhd) < 0)
	G_fatal_error(_("Unable to read header of raster map <%s>"),
		      G_fully_qualified_name(name, mapset));

    if (create) {
	/* write a file of no-nulls */
	null_bits = (unsigned char *)G__allocate_null_bits(cellhd.cols);
	/* init all cells to 0's */
	for (col = 0; col < G__null_bitstream_size(cellhd.cols); col++)
	    null_bits[col] = 0;

	null_fd = G_open_new_misc("cell_misc", "null", name);

	G_verbose_message(_("Writing new null file for raster map <%s>..."),
			  name);

	for (row = 0; row < cellhd.rows; row++) {
	    G_percent(row, cellhd.rows, 1);
	    if (G__write_null_bits(null_fd, null_bits, row, cellhd.cols, 0) < 0)
		G_fatal_error(_("Error writing null row %d"), row);
	}
	G_percent(row, cellhd.rows, 1);
	close(null_fd);

	G_done_msg(_("Raster map <%s> modified."), name);

	exit(EXIT_SUCCESS);
    }

    if (remove) {
	/* write a file of no-nulls */
	G_verbose_message(_("Removing null file for raster map <%s>..."),
			   name);
	null_fd = G_open_new_misc("cell_misc", "null", name);
	G__file_name_misc(path, "cell_misc", "null", name, mapset);
	unlink(path);

	G_done_msg(_("Raster map <%s> modified."), name);

	exit(EXIT_SUCCESS);
    }

    process(name, mapset, change_null, map_type);

    exit(EXIT_SUCCESS);
}

static int parse_vallist(char **vallist, d_Mask * d_mask)
{
    char buf[1024];
    char x[2];
    FILE *fd;

    init_d_mask_rules(d_mask);
    if (vallist == NULL)
	return 1;

    for (; *vallist; vallist++) {
	if (*vallist[0] == '/') {
	    fd = fopen(*vallist, "r");
	    if (fd == NULL) {
		perror(*vallist);
		G_usage();
		exit(1);
	    }
	    while (fgets(buf, sizeof buf, fd)) {
		if (sscanf(buf, "%1s", x) != 1 || *x == '#')
		    continue;
		parse_d_mask_rule(buf, d_mask, *vallist);
	    }
	    fclose(fd);
	}
	else
	    parse_d_mask_rule(*vallist, d_mask, (char *)NULL);
    }

    return 0;
}

int parse_d_mask_rule(char *vallist, d_Mask * d_mask, char *where)
{
    double a, b;
    char junk[128];

    /* #-# */
    if (sscanf(vallist, "%lf-%lf", &a, &b) == 2)
	add_d_mask_rule(d_mask, a, b, 0);
    /* inf-# */
    else if (sscanf(vallist, "%[^ -\t]-%lf", junk, &a) == 2)
	add_d_mask_rule(d_mask, a, a, -1);

    /* #-inf */
    else if (sscanf(vallist, "%lf-%[^ \t]", &a, junk) == 2)
	add_d_mask_rule(d_mask, a, a, 1);

    /* # */
    else if (sscanf(vallist, "%lf", &a) == 1)
	add_d_mask_rule(d_mask, a, a, 0);

    else {
	if (where)
	    G_fatal_error(_("%s: %s: illegal value spec"), where, vallist);
	else
	    G_fatal_error(_("%s: illegal value spec"), vallist);
    }

    return 0;
}

int
process(char *name, char *mapset, int change_null, RASTER_MAP_TYPE map_type)
{
    struct Colors colr;
    struct History hist;
    struct Categories cats;
    struct Quant quant;
    int colr_ok;
    int hist_ok;
    int cats_ok;
    int quant_ok;

    G_suppress_warnings(1);
    colr_ok = G_read_colors(name, mapset, &colr) > 0;
    hist_ok = G_read_history(name, mapset, &hist) >= 0;
    cats_ok = G_read_raster_cats(name, mapset, &cats) >= 0;

    if (map_type != CELL_TYPE) {
	G_quant_init(&quant);
	quant_ok = G_read_quant(name, mapset, &quant);
	G_suppress_warnings(0);
    }

    if (doit(name, mapset, change_null, map_type))
	return 1;

    if (colr_ok) {
	G_write_colors(name, mapset, &colr);
	G_free_colors(&colr);
    }
    if (hist_ok)
	G_write_history(name, &hist);
    if (cats_ok) {
	cats.num = G_number_of_cats(name, mapset);
	G_write_raster_cats(name, &cats);
	G_free_cats(&cats);
    }
    if (map_type != CELL_TYPE && quant_ok)
	G_write_quant(name, mapset, &quant);

    return 0;
}

int doit(char *name, char *mapset, int change_null, RASTER_MAP_TYPE map_type)
{
    int new, old, row;
    void *rast;

    G_set_window(&cellhd);

    old = G_open_cell_old(name, mapset);
    if (old < 0)
	G_fatal_error(_("Unable to open raster map <%s>"), name);

    new = G_open_raster_new(name, map_type);

    if (new < 0)
	G_fatal_error(_("Unable to create raster map <%s>"), name);

    rast = G_allocate_raster_buf(map_type);

    G_verbose_message(_("Writing new data for raster map <%s>..."), name);

    /* the null file is written automatically */
    for (row = 0; row < cellhd.rows; row++) {
	G_percent(row, cellhd.rows, 1);

	if (G_get_raster_row_nomask(old, rast, row, map_type) < 0) {
	    G_warning(_("Unable to read raster map <%s> row %d"),
		      name, row);
	    break;
	}

	mask_raster_array(rast, cellhd.cols, change_null, map_type);

	if (G_put_raster_row(new, rast, map_type) < 0) {
	    G_warning(_("Failed writing raster map <%s> row %d"),
		      name, row);
	    break;
	}
    }
    G_percent(row, cellhd.rows, 1);
    G_free(rast);
    G_close_cell(old);
    if (row < cellhd.rows) {
	G_unopen_cell(new);
	return 1;
    }
    G_close_cell(new);

    return 0;
}
