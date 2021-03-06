
/**
 * \file plus.c
 *
 * \brief Vector library - update topo structure (lower level functions)
 *
 * Lower level functions for reading/writing/manipulating vectors.
 *
 * This program is free software under the GNU General Public License
 * (>=v2). Read the file COPYING that comes with GRASS for details.
 *
 * \author CERL (probably Dave Gerdes), Radim Blazek
 *
 * \date 2001-2006
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <grass/gis.h>
#include <grass/Vect.h>
#include <grass/glocale.h>

/*!
 * \brief Init head structure
 *
 * \param[in,out] Plus pointer to Plus_head structure
 *
 * \return 1
 */
int dig_init_plus(struct Plus_head *Plus)
{
    G_debug(3, "dig_init_plus()");

    Plus->Version_Major = 0;
    Plus->Version_Minor = 0;
    Plus->Back_Major = 0;
    Plus->Back_Minor = 0;
    Plus->with_z = 0;

    Plus->box.N = 0;
    Plus->box.S = 0;
    Plus->box.E = 0;
    Plus->box.W = 0;
    Plus->box.T = 0;
    Plus->box.B = 0;

    Plus->built = GV_BUILD_NONE;

    Plus->Node = NULL;
    Plus->Line = NULL;
    Plus->Area = NULL;
    Plus->Isle = NULL;

    Plus->n_nodes = 0;
    Plus->n_edges = 0;
    Plus->n_lines = 0;
    Plus->n_areas = 0;
    Plus->n_isles = 0;
    Plus->n_volumes = 0;
    Plus->n_holes = 0;

    Plus->alloc_nodes = 0;
    Plus->alloc_edges = 0;
    Plus->alloc_lines = 0;
    Plus->alloc_areas = 0;
    Plus->alloc_isles = 0;
    Plus->alloc_volumes = 0;
    Plus->alloc_holes = 0;

    Plus->n_plines = 0;
    Plus->n_llines = 0;
    Plus->n_blines = 0;
    Plus->n_clines = 0;
    Plus->n_flines = 0;
    Plus->n_klines = 0;

    Plus->Node_offset = 0L;
    Plus->Edge_offset = 0L;
    Plus->Line_offset = 0L;
    Plus->Area_offset = 0L;
    Plus->Isle_offset = 0L;
    Plus->Volume_offset = 0L;
    Plus->Hole_offset = 0L;

    Plus->Node_spidx_offset = 0L;
    Plus->Edge_spidx_offset = 0L;
    Plus->Line_spidx_offset = 0L;
    Plus->Area_spidx_offset = 0L;
    Plus->Isle_spidx_offset = 0L;
    Plus->Volume_spidx_offset = 0L;
    Plus->Hole_spidx_offset = 0L;

    dig_spidx_init(Plus);
    dig_cidx_init(Plus);

    return 1;
}

/*!
 * \brief Free Plus->Node structure
 *
 * \param[in] Plus pointer to Plus_head structure
 */
void dig_free_plus_nodes(struct Plus_head *Plus)
{
    int i;
    P_NODE *Node;

    G_debug(2, "dig_free_plus_nodes()");

    /* Nodes */
    if (Plus->Node) {		/* it may be that header only is loaded */
	for (i = 1; i <= Plus->n_nodes; i++) {
	    Node = Plus->Node[i];
	    if (Node == NULL)
		continue;

	    if (Node->alloc_lines > 0) {
		G_free(Node->lines);
		G_free(Node->angles);
	    }
	    G_free(Node);
	}
	G_free(Plus->Node);
    }
    Plus->Node = NULL;
    Plus->n_nodes = 0;
    Plus->alloc_nodes = 0;
}

/*!
 * \brief Free Plus->Line structure
 *
 * \param[in] Plus pointer to Plus_head structure
 */
void dig_free_plus_lines(struct Plus_head *Plus)
{
    int i;
    P_LINE *Line;

    G_debug(2, "dig_free_plus_lines()");

    /* Lines */
    if (Plus->Line) {		/* it may be that header only is loaded */
	for (i = 1; i <= Plus->n_lines; i++) {
	    Line = Plus->Line[i];
	    if (Line == NULL)
		continue;

	    G_free(Line);
	}
	G_free(Plus->Line);
    }

    Plus->Line = NULL;
    Plus->n_lines = 0;
    Plus->alloc_lines = 0;

    Plus->n_plines = 0;
    Plus->n_llines = 0;
    Plus->n_blines = 0;
    Plus->n_clines = 0;
    Plus->n_flines = 0;
    Plus->n_klines = 0;
}

/*!
 * \brief Free Plus->Area structure
 *
 * \param[in] Plus pointer to Plus_head structure
 */
void dig_free_plus_areas(struct Plus_head *Plus)
{
    int i;
    P_AREA *Area;

    G_debug(2, "dig_free_plus_areas()");

    /* Areas */
    if (Plus->Area) {		/* it may be that header only is loaded */
	for (i = 1; i <= Plus->n_areas; i++) {
	    Area = Plus->Area[i];
	    if (Area == NULL)
		continue;

	    if (Area->alloc_lines > 0)
		G_free(Area->lines);

	    if (Area->alloc_isles > 0)
		G_free(Area->isles);

	    G_free(Area);
	}
	G_free(Plus->Area);
    }
    Plus->Area = NULL;
    Plus->n_areas = 0;
    Plus->alloc_areas = 0;
}

/*!
 * \brief Free Plus->Isle structure
 *
 * \param[in] Plus pointer to Plus_head structure
 */
void dig_free_plus_isles(struct Plus_head *Plus)
{
    int i;
    P_ISLE *Isle;

    G_debug(2, "dig_free_plus_isles()");

    /* Isles */
    if (Plus->Isle) {		/* it may be that header only is loaded */
	for (i = 1; i <= Plus->n_isles; i++) {
	    Isle = Plus->Isle[i];
	    if (Isle == NULL)
		continue;

	    if (Isle->alloc_lines > 0)
		G_free(Isle->lines);

	    G_free(Isle);
	}
	G_free(Plus->Isle);
    }

    Plus->Isle = NULL;
    Plus->n_isles = 0;
    Plus->alloc_isles = 0;
}

/*!
 * \brief Free Plus structure.
 *
 * Structure is not inited and dig_init_plus() should follow.
 *
 * \param[in] Plus pointer to Plus_head structure
 */
void dig_free_plus(struct Plus_head *Plus)
{
    G_debug(2, "dig_free_plus()");
    dig_free_plus_nodes(Plus);
    dig_free_plus_lines(Plus);
    dig_free_plus_areas(Plus);
    dig_free_plus_isles(Plus);

    dig_cidx_free(Plus);
}

/*!
 * \brief Reads topo file to topo structure.
 *
 * \param[in,out] Plus pointer to Plus_head structure
 * \param[in] plus topo file
 * \param[in] head_only read only head
 * 
 * \return 1 on success
 * \return 0 on error
 */
int dig_load_plus(struct Plus_head *Plus, GVFILE * plus, int head_only)
{
    int i;


    G_debug(1, "dig_load_plus()");
    /* TODO
       if (do_checks)
       dig_do_file_checks (map, map->plus_file, map->digit_file);
     */

    /* free and init old */
    dig_init_plus(Plus);

    /* Now let's begin reading the Plus file nodes, lines, areas and isles */

    if (dig_Rd_Plus_head(plus, Plus) == -1)
	return 0;

    if (head_only)
	return 1;

    dig_set_cur_port(&(Plus->port));

    /* Nodes */
    if (dig_fseek(plus, Plus->Node_offset, 0) == -1)
	G_fatal_error(_("Unable read topology for nodes"));

    dig_alloc_nodes(Plus, Plus->n_nodes);
    for (i = 1; i <= Plus->n_nodes; i++) {
	if (dig_Rd_P_node(Plus, i, plus) == -1)
	    G_fatal_error(_("Unable to read topology for node %d"), i);
    }

    /* Lines */
    if (dig_fseek(plus, Plus->Line_offset, 0) == -1)
	G_fatal_error(_("Unable read topology for lines"));

    dig_alloc_lines(Plus, Plus->n_lines);
    for (i = 1; i <= Plus->n_lines; i++) {
	if (dig_Rd_P_line(Plus, i, plus) == -1)
	    G_fatal_error(_("Unable to read topology for line %d"), i);
    }

    /* Areas */
    if (dig_fseek(plus, Plus->Area_offset, 0) == -1)
	G_fatal_error(_("Unable to read topo for areas"));

    dig_alloc_areas(Plus, Plus->n_areas);
    for (i = 1; i <= Plus->n_areas; i++) {
	if (dig_Rd_P_area(Plus, i, plus) == -1)
	    G_fatal_error(_("Unable read topology for area %d"), i);
    }

    /* Isles */
    if (dig_fseek(plus, Plus->Isle_offset, 0) == -1)
	G_fatal_error(_("Unable to read topology for isles"));

    dig_alloc_isles(Plus, Plus->n_isles);
    for (i = 1; i <= Plus->n_isles; i++) {
	if (dig_Rd_P_isle(Plus, i, plus) == -1)
	    G_fatal_error(_("Unable to read topology for isle %d"), i);
    }

    return (1);
}

/*!
 * \brief Writes topo structure to topo file
 *
 * \param[in,out] fp_plus topo file
 * \param[in] Plus pointer to Plus_head structure
 *
 * \return 0 on success
 * \return -1 on error
 */
int dig_write_plus_file(GVFILE * fp_plus, struct Plus_head *Plus)
{

    dig_set_cur_port(&(Plus->port));
    dig_rewind(fp_plus);

    if (dig_Wr_Plus_head(fp_plus, Plus) < 0) {
	G_warning(_("Unable to write head to plus file"));
	return (-1);
    }

    if (dig_write_nodes(fp_plus, Plus) < 0) {
	G_warning(_("Unable to write nodes to plus file"));
	return (-1);
    }

    if (dig_write_lines(fp_plus, Plus) < 0) {
	G_warning(_("Unable to write lines to plus file"));
	return (-1);
    }

    if (dig_write_areas(fp_plus, Plus) < 0) {
	G_warning(_("Unable to write areas to plus file"));
	return (-1);
    }

    if (dig_write_isles(fp_plus, Plus) < 0) {
	G_warning(_("Unable to write isles to plus file"));
	return (-1);
    }

    dig_rewind(fp_plus);
    if (dig_Wr_Plus_head(fp_plus, Plus) < 0) {
	G_warning(_("Unable to write head to plus file"));
	return (-1);
    }

    dig_fflush(fp_plus);
    return (0);
}				/*  write_plus_file()  */

/*!
 * \brief Writes topo structure (nodes) to topo file
 *
 * \param[in,out] fp_plus topo file
 * \param[in] Plus pointer to Plus_head structure
 *
 * \return 0 on success
 * \return -1 on error
 */
int dig_write_nodes(GVFILE * plus, struct Plus_head *Plus)
{
    int i;


    Plus->Node_offset = dig_ftell(plus);

    for (i = 1; i <= Plus->n_nodes; i++) {
	if (dig_Wr_P_node(Plus, i, plus) < 0)
	    return (-1);
    }

    return (0);
}				/*  write_nodes()  */

/*!
 * \brief Writes topo structure (lines) to topo file
 *
 * \param[in,out] fp_plus topo file
 * \param[in] Plus pointer to Plus_head structure
 *
 * \return 0 on success
 * \return -1 on error
 */
int dig_write_lines(GVFILE * plus, struct Plus_head *Plus)
{
    int i;


    Plus->Line_offset = dig_ftell(plus);

    for (i = 1; i <= Plus->n_lines; i++) {
	if (dig_Wr_P_line(Plus, i, plus) < 0)
	    return (-1);
    }

    return (0);

}				/*  write_line()  */

/*!
 * \brief Writes topo structure (areas) to topo file
 *
 * \param[in,out] fp_plus topo file
 * \param[in] Plus pointer to Plus_head structure
 *
 * \return 0 on success
 * \return -1 on error
 */
int dig_write_areas(GVFILE * plus, struct Plus_head *Plus)
{
    int i;


    Plus->Area_offset = dig_ftell(plus);

    for (i = 1; i <= Plus->n_areas; i++) {
	if (dig_Wr_P_area(Plus, i, plus) < 0)
	    return (-1);
    }

    return (0);

}				/*  write_areas()  */

/*!
 * \brief Writes topo structure (isles) to topo file
 *
 * \param[in,out] fp_plus topo file
 * \param[in] Plus pointer to Plus_head structure
 *
 * \return 0 on success
 * \return -1 on error
 */
int dig_write_isles(GVFILE * plus, struct Plus_head *Plus)
{
    int i;


    Plus->Isle_offset = dig_ftell(plus);

    for (i = 1; i <= Plus->n_isles; i++) {
	if (dig_Wr_P_isle(Plus, i, plus) < 0)
	    return (-1);
    }

    return (0);

}				/*  write_isles()  */
