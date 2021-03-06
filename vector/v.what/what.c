#include <string.h>
#include <unistd.h>
#include <math.h>
#include <grass/gis.h>
#include <grass/raster.h>
#include <grass/display.h>
#include <grass/colors.h>
#include <grass/Vect.h>
#include <grass/form.h>
#include <grass/dbmi.h>
#include <grass/glocale.h>
#include "what.h"
static int nlines = 50;

#define WDTH 5

int what(double east, double north, double maxdist, int width,
	 int mwidth, int topo, int showextra, int script)
{
    int type;
    char east_buf[40], north_buf[40];
    double sq_meters, sqm_to_sqft, acres, hectares, sq_miles;
    double z, l;
    int notty = 0;
    int getz;
    struct field_info *Fi;
    plus_t line, area, centroid;
    int i;
    struct line_pnts *Points;
    struct line_cats *Cats;
    char buf[1000], *str;
    dbString html;
    char *form;

    Points = Vect_new_line_struct();
    Cats = Vect_new_cats_struct();
    db_init_string(&html);

    /* always use plain feet not US survey ft */
    /*  if you really want USfeet, try G_database_units_to_meters_factor()
        here, but then watch that sq_miles is not affected too */
    sqm_to_sqft = 1 / ( 0.0254 * 0.0254 * 12 * 12 );


    for (i = 0; i < nvects; i++) {
	/* init variables */
	area = 0;
	getz = 0;
	z = 0;
	l = 0;

	Vect_reset_cats(Cats);
	Vect_reset_line(Points);

	/* Try to find point first and only if no one was found try lines,
	 *  otherwise point on line could not be selected and similarly for areas */
	line =
	    Vect_find_line(&Map[i], east, north, 0.0, GV_POINT | GV_CENTROID,
			   maxdist, 0, 0);
	if (line == 0) {
	    line = Vect_find_line(&Map[i], east, north, 0.0,
				  GV_LINE | GV_BOUNDARY | GV_FACE, maxdist, 0, 0);
	}

	if (line == 0) {
	    area = Vect_find_area(&Map[i], east, north);
	    getz = Vect_tin_get_z(&Map[i], east, north, &z, NULL, NULL);
	}

	G_debug(2, "line = %d area = %d", line, area);

	if (!i) {
	    G_format_easting(east, east_buf, G_projection());
	    G_format_northing(north, north_buf, G_projection());
	    if (line + area > 0 || G_verbose() >= G_verbose_std()) {
		if (script) {
		    fprintf(stdout, "East=%s\nNorth=%s\n", east_buf,
			    north_buf);
		}
		else {
		    fprintf(stdout, "\nEast: %s\nNorth: %s\n", east_buf,
			    north_buf);
		}
		if (notty)
		    fprintf(stderr, "\nEast: %s\nNorth: %s\n", east_buf,
			    north_buf);
	    }
	    nlines++;
	}

	strcpy(buf, vect[i]);
	if ((str = strchr(buf, '@')))
	    *str = 0;

	if (line + area > 0 || G_verbose() >= G_verbose_std()) {
	    if (script) {
		fprintf(stdout, "Map=%s\nMapset=%s\n", Map[i].name,
			Map[i].mapset);
	    }
	    else {
		fprintf(stdout, "\nMap: %s \nMapset: %s\n", Map[i].name,
			Map[i].mapset);
	    }
	    if (notty)
		fprintf(stderr, "\nMap: %s \nMapset: %s\n", Map[i].name,
			Map[i].mapset);
	}
	nlines++;

	if (line + area == 0) {
	    if (line + area > 0 || G_verbose() >= G_verbose_std()) {
		fprintf(stdout, _("Nothing Found.\n"));
		if (notty)
		    fprintf(stderr, _("Nothing Found.\n"));
	    }
	    nlines++;
	    continue;
	}

	if (line > 0) {
	    type = Vect_read_line(&Map[i], Points, Cats, line);
	    switch (type) {
	    case GV_POINT:
		sprintf(buf, "Point\n");
		break;
	    case GV_LINE:
		sprintf(buf, "Line\n");
		break;
	    case GV_BOUNDARY:
		sprintf(buf, "Boundary\n");
		break;
	    case GV_FACE:
		sprintf(buf, "Face\n");
		break;
	    case GV_CENTROID:
		sprintf(buf, "Centroid\n");
		break;
	    default:
		sprintf(buf, "Unknown\n");
	    }
	    if (type & GV_LINES) {
		if (G_projection() == 3)
		    l = Vect_line_geodesic_length(Points);
		else
		    l = Vect_line_length(Points);
	    }


	    if (topo) {
		int n, node[2], nnodes, nnlines, nli, nodeline, left, right;
		float angle;

		Vect_get_line_areas(&(Map[i]), line, &left, &right);
		if (script) {
		    fprintf(stdout, "Feature_max_distance=%f\n", maxdist);
		    fprintf(stdout,
			    "Id=%d\nType=%s\nLeft=%d\nRight=%d\n",
			    line, buf, left, right);
		}
		else {
		    fprintf(stdout, "Looking for features within: %f\n",
			    maxdist);
		    fprintf(stdout,
			    _("Id: %d\nType: %s\nLeft: %d\nRight: %d\n"),
			    line, buf, left, right);
		}
		if (type & GV_LINES) {
		    nnodes = 2;
		    fprintf(stdout, _("Length: %f\n"), l);
		}
		else {		/* points */
		    nnodes = 1;
		    if (!script)
			fprintf(stdout, "\n");
		}

		Vect_get_line_nodes(&(Map[i]), line, &node[0], &node[1]);

		for (n = 0; n < nnodes; n++) {
		    double nx, ny, nz;

		    nnlines = Vect_get_node_n_lines(&(Map[i]), node[n]);

		    Vect_get_node_coor(&(Map[i]), node[n], &nx, &ny, &nz);
		    if (script) {
			fprintf(stdout,
				_("Node[%d]=%d\nNumber_lines=%d\nCoordinates=%.6f,%.6f,%.6f\n"),
				n, node[n], nnlines, nx, ny, nz);
		    }
		    else {
			fprintf(stdout,
				_("Node[%d]: %d\nNumber of lines: %d\nCoordinates: %.6f, %.6f, %.6f\n"),
				n, node[n], nnlines, nx, ny, nz);
		    }

		    for (nli = 0; nli < nnlines; nli++) {
			nodeline =
			    Vect_get_node_line(&(Map[i]), node[n], nli);
			angle =
			    Vect_get_node_line_angle(&(Map[i]), node[n], nli);
			if (script) {
			    fprintf(stdout, "Id=%5d\nAngle=%.8f\n",
				    nodeline, angle);
			}
			else {
			    fprintf(stdout, _("Id: %5d\nAngle: %.8f\n"),
				    nodeline, angle);
			}
		    }
		}

	    }
	    else {
		if (script) {
		    fprintf(stdout, "Type=%s\n", buf);
		    fprintf(stdout, "Id=%d\n", line);
		    if (type & GV_LINES)
			fprintf(stdout, "Length=%f\n", l);
		}
		else {
		    fprintf(stdout, _("Type: %s"), buf);
		    fprintf(stdout, _("Id: %d\n"), line);
		    if (type & GV_LINES)
			fprintf(stdout, _("Length: %f\n"), l);
		}
	    }

	    /* Height */
	    if (Vect_is_3d(&(Map[i]))) {
		int j;
		double min, max;

		if (type & GV_POINTS) {
		    if (script) {
			fprintf(stdout, "Point_height=%f\n", Points->z[0]);
		    }
		    else {
			fprintf(stdout, _("Point height: %f\n"),
				Points->z[0]);
		    }
		}
		else if (type & GV_LINES) {
		    min = max = Points->z[0];
		    for (j = 1; j < Points->n_points; j++) {
			if (Points->z[j] < min)
			    min = Points->z[j];
			if (Points->z[j] > max)
			    max = Points->z[j];
		    }
		    if (min == max) {
			if (script) {
			    fprintf(stdout, "Line_height=%f\n", min);
			}
			else {
			    fprintf(stdout, _("Line height: %f\n"), min);
			}
		    }
		    else {
			if (script) {
			    fprintf(stdout,
				    "Line_height_min=%f\nLine_height_max=%f\n",
				    min, max);
			}
			else {
			    fprintf(stdout,
				    _("Line height min: %f\nLine height max: %f\n"),
				    min, max);
			}
		    }
		}
	    }			/* if height */
	}			/* if line > 0 */

	if (area > 0) {
	    if (Map[i].head.with_z && getz) {
		if (script) {
		    fprintf(stdout, "Type=Area\nArea_height=%f\n", z);
		}
		else {
		fprintf(stdout, _("Type: Area\nArea height: %f\n"), z);
		}
	    }
	    else {
		if (script) {
		    fprintf(stdout, "Type=Area\n");
		}
		else {
		    fprintf(stdout, _("Type: Area\n"));
		}
	    }


	    sq_meters = Vect_get_area_area(&Map[i], area);
	    hectares  = sq_meters / 10000.;
	    /* 1 acre = 1 chain(66') * 1 furlong(10 chains),
		or if you prefer ( 5280 ft/mi ^2 / 640 acre/sq mi ) */
	    acres = (sq_meters * sqm_to_sqft) / (66 * 660);
	    sq_miles = acres / 640.;


	    if (topo) {
		int nisles, isleidx, isle, isle_area;

		nisles = Vect_get_area_num_isles(&Map[i], area);
		if (script) {
		    fprintf(stdout, "Area=%d\nNumber_isles=%d\n", area,
			    nisles);
		}
		else {
		    fprintf(stdout, _("Area: %d\nNumber of isles: %d\n"),
			    area, nisles);
		}

		for (isleidx = 0; isleidx < nisles; isleidx++) {
		    isle = Vect_get_area_isle(&Map[i], area, isleidx);
		    if (script) {
			fprintf(stdout, "Isle[%d]=%d\n", isleidx, isle);
		    }
		    else {
			fprintf(stdout, _("Isle[%d]: %d\n"), isleidx, isle);
		    }
		}

		isle = Vect_find_island(&Map[i], east, north);

		if (isle) {
		    isle_area = Vect_get_isle_area(&Map[i], isle);
		    if (script) {
			fprintf(stdout, "Island=%d\nIsland_area=%d\n", isle,
				isle_area);
		    }
		    else {
			fprintf(stdout, _("Island: %d In area: %d\n"), isle,
				isle_area);
		    }
		}
	    }
	    else {
		if (script) {
		    fprintf(stdout, "Sq_Meters=%.3f\nHectares=%.3f\n",
			    sq_meters, hectares);
		    fprintf(stdout, "Acres=%.3f\nSq_Miles=%.4f\n",
			    acres, sq_miles);
		}
		else {
		    fprintf(stdout, _("Sq Meters: %.3f\nHectares: %.3f\n"),
			    sq_meters, hectares);
		    fprintf(stdout, _("Acres: %.3f\nSq Miles: %.4f\n"),
			    acres, sq_miles);
		}
		if (notty) {
		    fprintf(stderr,
			    _("Sq Meters: %.3f\nHectares: %.3f\n"),
			    sq_meters, hectares);
		    fprintf(stderr,
			    _("Acres: %.3f\nSq Miles: %.4f\n"),
			    acres, sq_miles);
		}
		nlines += 3;
	    }
	    centroid = Vect_get_area_centroid(&Map[i], area);
	    if (centroid > 0) {
		Vect_read_line(&Map[i], Points, Cats, centroid);
	    }
	}			/* if area > 0 */

	if (Cats->n_cats > 0) {
	    int j;
	    char *formbuf1, *formbuf2;

	    for (j = 0; j < Cats->n_cats; j++) {
		G_debug(2, "field = %d  category = %d\n", Cats->field[j],
			Cats->cat[j]);
		if (script) {
		    fprintf(stdout, "Layer=%d\nCategory=%d\n", Cats->field[j],
			    Cats->cat[j]);
		}
		else {
		    fprintf(stdout, _("Layer: %d\nCategory: %d\n"),
			    Cats->field[j], Cats->cat[j]);
		}
		Fi = Vect_get_field(&(Map[i]), Cats->field[j]);
		if (Fi != NULL && showextra) {
		    int format = F_TXT, edit_mode = F_VIEW;

		    if (script) {
			fprintf(stdout,
				"Driver=%s\nDatabase=%s\nTable=%s\nKey_column=%s\n",
				Fi->driver, Fi->database, Fi->table, Fi->key);
		    }
		    else {
			fprintf(stdout,
				_("\nDriver: %s\nDatabase: %s\nTable: %s\nKey column: %s\n"),
				Fi->driver, Fi->database, Fi->table, Fi->key);
		    }
		    F_generate(Fi->driver, Fi->database, Fi->table,
			       Fi->key, Cats->cat[j], NULL, NULL,
			       edit_mode, format, &form);

		    if (script) {
			formbuf1 = G_str_replace(form, " : ", "=");
			formbuf2 = G_str_replace(formbuf1, " ", "_");
			fprintf(stdout, "%s", formbuf2);
			G_free(formbuf1);
			G_free(formbuf2);
		    }
		    else
			fprintf(stdout, "%s", form);
		    G_free(form);
		    G_free(Fi);
		}
	    }
	}
    }				/* for nvects */

    fflush(stdout);

    return 0;
}
