/****************************************************************************
*
* MODULE:       r.out.gdal
* AUTHOR(S):    Vytautas Vebra <olivership@gmail.com>
* PURPOSE:      exports GRASS raster to many GDAL suported formats;
*               based on GDAL library.
*               Replace r.out.gdal.sh script based on gdal_translate
*               executable.
* COPYRIGHT:    (C) 2006-2007 by the GRASS Development Team
*
*               This program is free software under the GNU General Public
*   	    	License (>=v2). Read the file COPYING that comes with GRASS
*   	    	for details.
*
*****************************************************************************/

/* Undefine this if you do not want any extra funtion calls before G_parse() */
#define __ALLOW_DYNAMIC_OPTIONS__

#include <stdlib.h>
#include <unistd.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include <grass/gis.h>
#include <grass/imagery.h>
#include <grass/gprojects.h>
#include <grass/glocale.h>
#include <grass/dbmi.h>

#include "cpl_string.h"
#include "gdal.h"

#define GRASS_MAX_COLORS 100000  /* what is the right value ?*/

void supported_formats(char **formats)
{
    /* Code taken from r.in.gdal */

    int iDr;

    dbString gdal_formats;

    db_init_string(&gdal_formats);

    if (*formats)
	fprintf(stdout, _("Supported formats:\n"));

    for (iDr = 0; iDr < GDALGetDriverCount(); iDr++) {

	GDALDriverH hDriver = GDALGetDriver(iDr);
	const char *pszRWFlag;

	if (GDALGetMetadataItem(hDriver, GDAL_DCAP_CREATE, NULL))
	    pszRWFlag = "rw+";
	else if (GDALGetMetadataItem(hDriver, GDAL_DCAP_CREATECOPY, NULL))
	    pszRWFlag = "rw";
	else
	    pszRWFlag = "ro";

	if (*formats)
	    fprintf(stdout, "  %s (%s): %s\n", GDALGetDriverShortName(hDriver),
		    pszRWFlag, GDALGetDriverLongName(hDriver));
	else {
	    if (GDALGetMetadataItem(hDriver, GDAL_DCAP_CREATE, NULL) ||
		GDALGetMetadataItem(hDriver, GDAL_DCAP_CREATECOPY, NULL)) {
		if (db_sizeof_string(&gdal_formats) > 0)
		    db_append_string(&gdal_formats, ",");

		db_append_string(&gdal_formats,
				 (char *)GDALGetDriverShortName(hDriver));
	    }
	}
    }

    if (db_sizeof_string(&gdal_formats) > 0)
	*formats = G_store(db_get_string(&gdal_formats));

    return;
}

int import_band(GDALDatasetH hMEMDS, int band, char *name, char *mapset,
		struct Cell_head *cellhead, RASTER_MAP_TYPE maptype,
		double nodataval, char *nodatakey)
{

    struct Colors sGrassColors;
    GDALColorTableH hCT;
    int iColor;
    int bHaveMinMax;
    double dfCellMin;
    double dfCellMax;
    struct FPRange sRange;
    int fd;
    int cols = cellhead->cols;
    int rows = cellhead->rows;

    /* Open GRASS raster */
    fd = G_open_cell_old(name, mapset);
    if (fd < 0) {
	G_warning(_("Unable to open raster map <%s>"), name);
	return -1;
    }

    /* Get raster band  */
    GDALRasterBandH hBand = GDALGetRasterBand(hMEMDS, band);
    if (hBand == NULL) {
	G_warning(_("Unable to get raster band"));
	return -1;
    }

    /* Get min/max values. */
    if( G_read_fp_range(name, mapset, &sRange ) == -1 )
    {
        bHaveMinMax = FALSE;
    }
    else
    {
        bHaveMinMax = TRUE;
        G_get_fp_range_min_max( &sRange, &dfCellMin, &dfCellMax );
    }

    /* suppress useless warnings */
    CPLPushErrorHandler( CPLQuietErrorHandler );
    GDALSetRasterColorInterpretation(hBand, GPI_RGB);
    CPLPopErrorHandler();

    if( G_read_colors(name, mapset, &sGrassColors ) >= 0 )
    {
	int maxcolor, i;
	CELL min, max;
	char key[200], value[200];
	int rcount;

	G_get_color_range ( &min, &max, &sGrassColors);
        if ( bHaveMinMax ) {
	    if ( max < dfCellMax ) {
	       maxcolor = max;
            } else {
	       maxcolor = (int) ceil ( dfCellMax );
	    }
	    if ( maxcolor > GRASS_MAX_COLORS ) { 
		maxcolor = GRASS_MAX_COLORS;
                G_warning("Too many values, color table cut to %d entries", maxcolor );
	    }
	} else {
	    if ( max < GRASS_MAX_COLORS ) {
	       maxcolor = max;
            } else {
	       maxcolor = GRASS_MAX_COLORS;
               G_warning("Too many values, color table set to %d entries", maxcolor );
	    }
        }
	
	rcount = G_colors_count( &sGrassColors );
	    
	G_debug(3, "dfCellMin: %f, dfCellMax: %f, maxcolor: %d", dfCellMin, dfCellMax, maxcolor);

	hCT = GDALCreateColorTable(GPI_RGB);
        for( iColor = 0; iColor <= maxcolor; iColor++ )
        {
            int	nRed, nGreen, nBlue;
            GDALColorEntry sColor;

            if( G_get_color( iColor, &nRed, &nGreen, &nBlue, &sGrassColors ) )
            {
                sColor.c1 = nRed;
                sColor.c2 = nGreen;
                sColor.c3 = nBlue;
                sColor.c4 = 255;

                G_debug(3, "G_get_color: Y, rcount %d, nRed %d, nGreen %d, nBlue %d", rcount, nRed, nGreen, nBlue);
                GDALSetColorEntry( hCT, iColor, &sColor );
            }
            else
            {
                sColor.c1 = 0;
                sColor.c2 = 0;
                sColor.c3 = 0;
                sColor.c4 = 0;

                G_debug(3, "G_get_color: N, rcount %d, nRed %d, nGreen %d, nBlue %d", rcount, nRed, nGreen, nBlue);
		GDALSetColorEntry( hCT, iColor, &sColor );
            }
        }

	if (rcount > 0)
	{
	    /* Create metadata entries for color table rules */
	    sprintf ( value, "%d", rcount );
	    GDALSetMetadataItem(hBand, "COLOR_TABLE_RULES_COUNT", value, NULL);
	}

	/* Add the rules in reverse order */
	for ( i = rcount-1; i >= 0; i-- ) {
	    DCELL val1, val2;
	    unsigned char r1, g1, b1, r2, g2, b2;

	     G_get_f_color_rule ( &val1, &r1, &g1, &b1, &val2, &r2, &g2, &b2, &sGrassColors, i );
		

	     sprintf ( key, "COLOR_TABLE_RULE_RGB_%d", rcount-i-1 );
	     sprintf ( value, "%e %e %d %d %d %d %d %d", val1, val2, r1, g1, b1, r2, g2, b2 );
	     GDALSetMetadataItem(hBand, key, value, NULL);
	}
	GDALSetRasterColorTable(hBand, hCT);
    } else {
        hCT = GDALCreateColorTable(GPI_RGB);
	GDALSetMetadataItem(hBand, "COLOR_TABLE_RULES_COUNT", "0", NULL);
        GDALSetRasterColorTable(hBand, hCT);
    }

    /* Create GRASS raster buffer */
    void *bufer = G_allocate_raster_buf(maptype);
    if (bufer == NULL) {
	G_warning(_("Unable to allocate buffer for reading raster map"));
	return -1;
    }
    char *nulls = (char *)G_malloc(cols);
    if (nulls == NULL) {
	G_warning(_("Unable to allocate buffer for reading raster map"));
	return -1;
    }

    /* Copy data form GRASS raster to memory raster */
    int row, col;
    int n_nulls = 0;
    if (maptype == FCELL_TYPE) {

	/* Source datatype understandible by GDAL */
	GDALDataType datatype = GDT_Float32;
	FCELL fnullval = (FCELL) nodataval;
	for (row = 0; row < rows; row++) {

	    if (G_get_raster_row(fd, bufer, row, maptype) < 0) {
		G_warning(_("Unable to read raster map <%s> row %d"),
			  name, row);
		return -1;
	    }
	    G_get_null_value_row(fd, nulls, row);
	    for (col = 0; col < cols; col++)
		if (nulls[col]) {
		    ((FCELL *) bufer)[col] = fnullval;
		    if (n_nulls == 0) {
			GDALSetRasterNoDataValue(hBand, nodataval);
		    }
		    n_nulls++;
		}

	    if (GDALRasterIO
		(hBand, GF_Write, 0, row, cols, 1, bufer, cols, 1, datatype, 0,
		 0) >= CE_Failure) {
		G_warning(_("Unable to write GDAL raster file"));
		return -1;
	    }
	    G_percent(row + 1, rows, 2);
	}
    }
    else if (maptype == DCELL_TYPE) {

	GDALDataType datatype = GDT_Float64;
	DCELL dnullval = (DCELL) nodataval;
	for (row = 0; row < rows; row++) {

	    if (G_get_raster_row(fd, bufer, row, maptype) < 0) {
		G_warning(_("Unable to read raster map <%s> row %d"),
			  name, row);
		return -1;
	    }
	    G_get_null_value_row(fd, nulls, row);
	    for (col = 0; col < cols; col++)
		if (nulls[col]) {
 		    ((DCELL *) bufer)[col] = dnullval;
		    if (n_nulls == 0) {
			GDALSetRasterNoDataValue(hBand, nodataval);
		    }
		    n_nulls++;
		}

	    if (GDALRasterIO
		(hBand, GF_Write, 0, row, cols, 1, bufer, cols, 1, datatype, 0,
		 0) >= CE_Failure) {
		G_warning(_("Unable to write GDAL raster file"));
		return -1;
	    }
	    G_percent(row + 1, rows, 2);
	}
    }
    else {

	GDALDataType datatype = GDT_Int32;
	CELL inullval = (CELL) nodataval;
	for (row = 0; row < rows; row++) {

	    if (G_get_raster_row(fd, bufer, row, maptype) < 0) {
		G_warning(_("Unable to read raster map <%s> row %d"),
			  name, row);
		return -1;
	    }
	    G_get_null_value_row(fd, nulls, row);
	    for (col = 0; col < cols; col++)
		if (nulls[col]) {
 		    ((CELL *) bufer)[col] = inullval;
		    if (n_nulls == 0) {
			GDALSetRasterNoDataValue(hBand, nodataval);
		    }
		    n_nulls++;
		}

	    if (GDALRasterIO
		(hBand, GF_Write, 0, row, cols, 1, bufer, cols, 1, datatype, 0,
		 0) >= CE_Failure) {
		G_warning(_("Unable to write GDAL raster file"));
		return -1;
	    }
	    G_percent(row + 1, rows, 2);
	}
    }

    if (n_nulls > 0) {
	if (maptype == CELL_TYPE)
	    G_warning(_("Input raster map constains cells with NULL-value (no-data). "
			"The value %d was used to represent no-data values in the input map."
			"You can specify nodata value by %s parameter."),
		      (int) nodataval, nodatakey);
	else
	    G_warning(_("Input raster map constains cells with NULL-value (no-data). "
			"The value %g was used to represent no-data values in the input map."
			"You can specify nodata value by %s parameter."),
		      nodataval, nodatakey);
    }

    return 0;
}

static void AttachMetadata(GDALDatasetH hDS, char **papszMetadataOptions)
/* function taken from gdal_translate */
{
    int nCount = CSLCount(papszMetadataOptions);
    int i;

    for (i = 0; i < nCount; i++) {
	char *pszKey = NULL;
	const char *pszValue;

	pszValue = CPLParseNameValue(papszMetadataOptions[i], &pszKey);
	GDALSetMetadataItem(hDS, pszKey, pszValue, NULL);
	CPLFree(pszKey);
    }

    CSLDestroy(papszMetadataOptions);
}


int main(int argc, char *argv[])
{

    struct GModule *module;
    struct Flag *flag_l;
    struct Option *input, *format, *type, *output, *createopt, *metaopt, *nodataopt;

    struct Cell_head cellhead;
    struct Ref ref;
    char *mapset, *gdal_formats = NULL;
    RASTER_MAP_TYPE maptype;
    int bHaveMinMax;
    double dfCellMin;
    double dfCellMax;
    struct FPRange sRange;

    G_gisinit(argv[0]);

    module = G_define_module();
    module->description = _("Exports GRASS raster map into GDAL supported formats.");
    module->keywords = _("raster, export");

    flag_l = G_define_flag();
    flag_l->key = 'l';
    flag_l->description = _("List supported output formats");
    flag_l->guisection = _("Print");

    input = G_define_standard_option(G_OPT_R_INPUT);
    input->required = NO;
    input->description = _("Name of raster map (or group) to export");
    input->guisection = _("Required");

    format = G_define_option();
    format->key = "format";
    format->type = TYPE_STRING;
    format->description =
	_("GIS format to write (case sensitive, see also -l flag)");

#ifdef __ALLOW_DYNAMIC_OPTIONS__
    /* Init GDAL */
    GDALAllRegister();
    supported_formats(&gdal_formats);
    if (gdal_formats)
	format->options = G_store(gdal_formats);
    /* else
     * G_fatal_error (_("Unknown GIS formats")); */
#else
    gdal_formats =
	"AAIGrid,BMP,BSB,DTED,ELAS,ENVI,FIT,GIF,GTiff,HFA,JPEG,MEM,MFF,MFF2,NITF,PAux,PNG,PNM,VRT,XPM";
    format->options = gdal_formats;
#endif
    format->answer = "GTiff";
    format->required = NO;

    type = G_define_option();
    type->key = "type";
    type->type = TYPE_STRING;
    type->description = _("File type");
    type->options =
	"Byte,Int16,UInt16,UInt32,Int32,Float32,Float64,CInt16,CInt32,CFloat32,CFloat64";
    type->required = NO;

    output = G_define_standard_option(G_OPT_R_OUTPUT);
    output->required = NO;
    output->gisprompt = "new_file,file,output";
    output->description = _("Name for output raster file");
    output->guisection = _("Required");

    createopt = G_define_option();
    createopt->key = "createopt";
    createopt->type = TYPE_STRING;
    createopt->description = _("Creation option to the output format driver. "
			       "Multiple options may be listed.");
    createopt->multiple = YES;
    createopt->required = NO;

    metaopt = G_define_option();
    metaopt->key = "metaopt";
    metaopt->type = TYPE_STRING;
    metaopt->description = _("Metadata key passed on the output dataset "
			     "if possible");
    metaopt->multiple = YES;
    metaopt->required = NO;

    nodataopt = G_define_option();
    nodataopt->key = "nodata";
    nodataopt->type = TYPE_DOUBLE;
    nodataopt->description = _("Assign a specified nodata value to output bands");
    nodataopt->multiple = NO;
    nodataopt->required = NO;

    if (G_parser(argc, argv))
	exit(EXIT_FAILURE);

#ifndef __ALLOW_DYNAMIC_OPTIONS__
    /* Init GDAL */
    GDALAllRegister();
#endif

    if (flag_l->answer) {
	supported_formats(&gdal_formats);
	exit(EXIT_SUCCESS);
    }

    if (!input->answer) {
	G_fatal_error(_("Required parameter <%s> not set"),
		      input->key);
    }

    /* Try to open input GRASS raster.. */
    mapset = G_find_cell2(input->answer, "");

    if (mapset != NULL) {
	/* Add input to "group". "Group" whith 1 raster (band) will exist only in memory. */
	I_init_group_ref(&ref);
	I_add_file_to_group_ref(input->answer, mapset, &ref);
    }
    else {
	/* Maybe input is group. Try to read group file */
	if (I_get_group_ref(input->answer, &ref) != 1)
	    G_fatal_error(_("Raster map or group <%s> not found"),
			  input->answer);
    }
    if (ref.nfiles == 0)
	G_fatal_error(_("No raster maps in group <%s>"), input->answer);

    /* Read project and region data */
    struct Key_Value *projinfo = G_get_projinfo();
    struct Key_Value *projunits = G_get_projunits();
    char *srswkt = GPJ_grass_to_wkt(projinfo, projunits, 0, 0);
    G_get_window(&cellhead);

    /* Try to create raster data drivers. If failed - exit. */
    GDALDriverH hDriver = NULL, hMEMDriver = NULL;
    hDriver = GDALGetDriverByName(format->answer);
    if (hDriver == NULL)
	G_fatal_error(_("Unable to get <%s> driver"), format->answer);
    /* Does driver support GDALCreate ? */
    if (GDALGetMetadataItem(hDriver, GDAL_DCAP_CREATE, NULL) == NULL) {
	/* If not - create MEM driver for intermediate dataset. 
	 * Check if raster can be created at all (with GDALCreateCopy) */
	if (GDALGetMetadataItem(hDriver, GDAL_DCAP_CREATECOPY, NULL)) {
	    G_message(_("Driver <%s> does not support direct writing. "
			"Using MEM driver for intermediate dataset."),
		      format->answer);
	    hMEMDriver = GDALGetDriverByName("MEM");
	    if (hMEMDriver == NULL)
		G_fatal_error(_("Unable to get in-memory raster driver"));

	}
	else
	    G_fatal_error(_("Driver <%s> does not support creating rasters"),
			  format->answer);
    }

    /* Determine raster data type */
    GDALDataType datatype = GDT_Unknown;
    double nodataval;

    if (type->answer) {
	/* reduce number of strcmps ... */
	if (type->answer[0] == 'B') {
	    datatype = GDT_Byte;
	    maptype = CELL_TYPE;
	    nodataval = (double)(unsigned char)0xFFu;
	}
	else if (type->answer[0] == 'I') {
	    if (strcmp(type->answer, "Int16") == 0) {
		datatype = GDT_Int16;
		maptype = CELL_TYPE;
		nodataval = (double)(short)0x8000;
	    }
	    else if (strcmp(type->answer, "Int32") == 0) {
		datatype = GDT_Int32;
		maptype = CELL_TYPE;
		nodataval = (double)(int)0x80000000;
	    }
	}
	else if (type->answer[0] == 'U') {
	    if (strcmp(type->answer, "UInt16") == 0) {
		datatype = GDT_UInt16;
		maptype = CELL_TYPE;
		nodataval = (double)(unsigned short)0xFFFFu;
	    }
	    else if (strcmp(type->answer, "UInt32") == 0) {
		datatype = GDT_UInt32;
		maptype = DCELL_TYPE;
		nodataval = (double)(unsigned int)0xFFFFFFFFu;
	    }
	}
	else if (type->answer[0] == 'F') {
	    if (strcmp(type->answer, "Float32") == 0) {
		datatype = GDT_Float32;
		maptype = FCELL_TYPE;
		nodataval = -1E37f;
	    }
	    else if (strcmp(type->answer, "Float64") == 0) {
		datatype = GDT_Float64;
		maptype = DCELL_TYPE;
		nodataval = -1E37f;
	    }
	}
	else if (type->answer[0] == 'C') {
	    if (strcmp(type->answer, "CInt16") == 0) {
		datatype = GDT_CInt16;
		maptype = CELL_TYPE;
		nodataval = (double)(short)0x8000;
	    }
	    else if (strcmp(type->answer, "CInt32") == 0) {
		datatype = GDT_CInt32;
		maptype = CELL_TYPE;
		nodataval = (double)(int)0x80000000;
	    }
	    else if (strcmp(type->answer, "CFloat32") == 0) {
		datatype = GDT_CFloat32;
		maptype = FCELL_TYPE;
		nodataval = -1E37f;
	    }
	    else if (strcmp(type->answer, "CFloat64") == 0) {
		datatype = GDT_CFloat64;
		maptype = DCELL_TYPE;
		nodataval = -1E37f;
	    }
	}
    }

    /* force nodata-value if needed */
    if (nodataopt->answer) {
	nodataval = atof(nodataopt->answer);
    }

    /* If file type not set by user ... */
    /* Get min/max values. */
    if( G_read_fp_range(ref.file[0].name, ref.file[0].mapset, &sRange ) == -1 ) {
        bHaveMinMax = FALSE;
    } else {
        bHaveMinMax = TRUE;
        G_get_fp_range_min_max( &sRange, &dfCellMin, &dfCellMax );
    }
    G_debug(3, "Range: min: %f, max: %f", dfCellMin, dfCellMax);

    if (datatype == GDT_Unknown) {
	/* ... determine raster data type from first GRASS raster in a group */
	maptype = G_raster_map_type(ref.file[0].name, ref.file[0].mapset);
	if (maptype == FCELL_TYPE) {
	    datatype = GDT_Float32;
	    nodataval = -1E37f;
	}
	else if (maptype == DCELL_TYPE) {
	    datatype = GDT_Float64;
	    nodataval = -1E37f;
	}
	else {
	   /* Special tricks for GeoTIFF color table support and such */
	    if (dfCellMin >= 0 && dfCellMax < 256 ) {
		datatype = GDT_Byte;
		nodataval = (double)(unsigned char)0xFFu;
	    } else {
		if (dfCellMin >= 0 && dfCellMax < 65536 ) {
		    datatype = GDT_UInt16;
		    nodataval = (double)(short)0x8000;
		} else {
		    datatype = GDT_Int32; /* need to furthermore fine tune this? */
		    nodataval = (double)(int)0x80000000;
		}
	    }
	}
    }

    G_debug( 3, "Input map datatype=%s\n",
                    (maptype == CELL_TYPE ? "CELL" :
                     (maptype == DCELL_TYPE ? "DCELL" :
                      (maptype == FCELL_TYPE ? "FCELL" : "??"))));
    G_message(_("Exporting to GDAL data type: %s"), GDALGetDataTypeName(datatype));

    /* Create dataset for output with target driver or, if needed, with in-memory driver */
    char **papszOptions = NULL;
    /* parse dataset creation options */
    if (createopt->answer) {
	int i;
	i = 0;
	while (createopt->answers[i]) {
	    papszOptions = CSLAddString(papszOptions, createopt->answers[i]);
	    i++;
	}
    }

    GDALDatasetH hCurrDS = NULL, hMEMDS = NULL, hDstDS = NULL;
    if(!output->answer)
	G_fatal_error(_("Output file name not specified"));
    if (hMEMDriver) {
	hMEMDS =
	    GDALCreate(hMEMDriver, "", cellhead.cols, cellhead.rows, ref.nfiles,
		       datatype, papszOptions);
	if (hMEMDS == NULL)
	    G_fatal_error(_("Unable to create dataset using "
			    "memory raster driver"));
	hCurrDS = hMEMDS;
    }
    else {
	hDstDS =
	    GDALCreate(hDriver, output->answer, cellhead.cols, cellhead.rows,
		       ref.nfiles, datatype, papszOptions);
	if (hDstDS == NULL)
	    G_fatal_error(_("Unable to create <%s> dataset using <%s> driver"),
			  output->answer, format->answer);
	hCurrDS = hDstDS;
    }

    /* Set Geo Transform  */
    double adfGeoTransform[6];
    adfGeoTransform[0] = cellhead.west;
    adfGeoTransform[1] = cellhead.ew_res;
    adfGeoTransform[2] = 0.0;
    adfGeoTransform[3] = cellhead.north;
    adfGeoTransform[4] = 0.0;
    adfGeoTransform[5] = -1 * cellhead.ns_res;
    if (GDALSetGeoTransform(hCurrDS, adfGeoTransform) >= CE_Failure)
	G_warning(_("Unable to set geo transform"));

    /* Set Projection  */
    CPLErr ret;
    if (srswkt)
	ret = GDALSetProjection(hCurrDS, srswkt);
    if (!srswkt || ret == CE_Failure)
	G_warning(_("Unable to set projection"));

    /* Add metadata */
    AttachMetadata(hCurrDS, metaopt->answers);

    /* Export to GDAL raster */
    int band;
    for (band = 0; band < ref.nfiles; band++) {
	if (ref.nfiles > 1) {
	    G_verbose_message(_("Exporting raster map <%s> (band %d)..."),
			      G_fully_qualified_name(ref.file[band].name, ref.file[band].mapset),
			      band+1);
	}
	if (import_band
	    (hCurrDS, band + 1, ref.file[band].name, ref.file[band].mapset,
	     &cellhead, maptype, nodataval, nodataopt->key) < 0)
	    G_warning(_("Unable to export raster map <%s>"),
		      ref.file[band].name);
    }

    /* Finaly create user required raster format from memory raster if in-memory driver was used */
    if (hMEMDS) {
	hDstDS =
	    GDALCreateCopy(hDriver, output->answer, hMEMDS, FALSE, papszOptions,
			   NULL, NULL);
	if (hDstDS == NULL)
	    G_fatal_error(_("Unable to create raster map <%s> using driver <%s>"),
			  output->answer, format->answer);
    }

    GDALClose(hDstDS);
    if (hMEMDS)
	GDALClose(hMEMDS);

    CSLDestroy(papszOptions);

    G_done_msg (" ");

    exit(EXIT_SUCCESS);
}
