/*
 * r3.in.v5d - program for data conversion from the V5D format 
 * of the VIS5D visualization software to G3D GRASS
 * data format.
 *
 * Copyright Jaroslav Hofierka
 * GeoModel,s.r.o., Bratislava, Slovakia 2000
 *
 * Comments or bug reports please send to hofierka@geomodel.sk
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "binio.h"
#include "v5d.h"
#include <grass/gis.h>
#include <grass/G3d.h>
#include <grass/glocale.h>

#define MAX(a,b) (a > b ? a : b)

/*---------------------------------------------------------------------------*/

void *map = NULL;

/*---------------------------------------------------------------------------*/
static void fatalError(char *errorMsg)
{
    if (map != NULL) {
	/* should unopen map here! */
    }

    G3d_fatalError(errorMsg);
}

/*---------------------------------------------------------------------------*/

typedef struct
{
    struct Option *input, *output, *nv;
} paramType;

static paramType param;

static void setParams()
{
    param.input = G_define_option();
    param.input->key = "input";
    param.input->type = TYPE_STRING;
    param.input->required = YES;
    param.input->description = "v5d raster map to be imported";

    param.output = G_define_option();
    param.output->key = "output";
    param.output->type = TYPE_STRING;
    param.output->required = YES;
    param.output->multiple = NO;
    param.output->gisprompt = "any,grid3,3d raster";
    param.output->description = "Name for G3d raster map";

    param.nv = G_define_option();
    param.nv->key = "nv";
    param.nv->type = TYPE_STRING;
    param.nv->required = NO;
    param.nv->multiple = NO;
    param.nv->answer = "none";
    param.nv->description =
	"String representing NULL value data cell (use 'none' if no such value)";
}

/*---------------------------------------------------------------------------*/

static void
getParams(char **input, char **output, int *convertNull, double *nullValue)
{
    *input = param.input->answer;
    *output = param.output->answer;
    *convertNull = (strcmp(param.nv->answer, "none") != 0);
    if (*convertNull)
	if (sscanf(param.nv->answer, "%lf", nullValue) != 1)
	    fatalError("getParams: NULL-value value invalid");
}

/*---------------------------------------------------------------------------*/

void convert(char *openFile, G3D_Region * region, int convertNull,
	     double nullValue)
{
    v5dstruct v5d;
    int time, var;
    int nrncnl, cnt;
    int x, y, z;
    float value;
    float res_r, res_c, res_l;
    float *data1;

    if (!v5dOpenFile(openFile, &v5d)) {
	printf("Error: couldn't open %s for reading\n", openFile);
	exit(0);
    }


    /* Eventually change to write the time and/or vars series of 3d raster maps.. */
    /*   for (time=0; time<v5d.NumTimes; time++) {

       for (var=0; var<v5d.NumVars; var++) {
     */

    for (time = 0; time < 1; time++) {

	for (var = 0; var < 1; var++) {

	    nrncnl = v5d.Nr * v5d.Nc * v5d.Nl[var];
	    region->rows = v5d.Nr;
	    region->cols = v5d.Nc;
	    region->depths = v5d.Nl[var];
	    region->north = v5d.ProjArgs[0];
	    res_r = v5d.ProjArgs[2];
	    res_c = v5d.ProjArgs[3];
	    res_l = v5d.VertArgs[1];
	    region->south = region->north - region->rows * res_r;
	    region->west = v5d.ProjArgs[1];
	    region->east = region->west + region->cols * res_c;
	    region->bottom = v5d.VertArgs[0];
	    region->top = region->bottom + region->depths * res_l;

	    data1 = (float *)G_malloc(nrncnl * sizeof(float));
	    if (!data1)
		G_fatal_error("Not enough memory for data1");

	    if (!v5dReadGrid(&v5d, time, var, data1)) {
		printf("Error while reading grid (time=%d,var=%s)\n",
		       time + 1, v5d.VarName[var]);
		exit(0);
	    }
	    cnt = 0;

	    for (z = 0; z < region->depths; z++) {
		for (y = 0; y < region->rows; y++) {
		    for (x = 0; x < region->cols; x++) {
			value = data1[cnt++];
			if (convertNull && (value == MISSING))
			    G3d_setNullValue(&value, 1, FCELL_TYPE);
			G3d_putFloat(map, x, y, z, value);
		    }
		}
	    }

	    G_free(data1);

	}
    }


    v5dCloseFile(&v5d);
}

/*---------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    char *input, *output;
    int convertNull;
    double nullValue;
    int useTypeDefault, type, useLzwDefault, doLzw, useRleDefault, doRle;
    int usePrecisionDefault, precision, useDimensionDefault, tileX, tileY,
	tileZ;
    G3D_Region region;
    struct GModule *module;

    map = NULL;

    G_gisinit(argv[0]);
    module = G_define_module();
    module->keywords = _("raster3d, voxel, import");
    module->description =
	"  import of 3-dimensional Vis5D files (i.e. the v5d file with 1 variable and 1 time step)";

    setParams();
    G3d_setStandard3dInputParams();

    if (G_parser(argc, argv))
	exit(1);

    getParams(&input, &output, &convertNull, &nullValue);
    if (!G3d_getStandard3dParams(&useTypeDefault, &type,
				 &useLzwDefault, &doLzw,
				 &useRleDefault, &doRle,
				 &usePrecisionDefault, &precision,
				 &useDimensionDefault, &tileX, &tileY,
				 &tileZ))
	fatalError("main: error getting standard parameters");

    G3d_getWindow(&region);
    map = G3d_openCellNew(output, FCELL_TYPE, G3D_USE_CACHE_XY, &region);
    if (map == NULL)
	fatalError(_("Error opening 3d raster map"));

    convert(input, &region, convertNull, nullValue);

    if (!G3d_closeCell(map))
	fatalError(_("Error closing 3d raster map"));
    map = NULL;

    return 0;
}

/*---------------------------------------------------------------------------*/
