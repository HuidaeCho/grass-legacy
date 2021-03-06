/*
 ****************************************************************************
 *
 * MODULE:       v.vol.rst: program for 3D(volume) interpolation and geometry
 *               analysis from scattered point data using regularized spline
 *               with tension
 *
 * AUTHOR(S):    Original program (1989) and various modifications:
 *               Lubos Mitas
 *
 *               GRASS 4.2, GRASS 5.0 version and modifications:
 *               H. Mitasova,  I. Kosinovsky, D. Gerdes, J. Hofierka
 *
 * PURPOSE:      v.vol.rst interpolates the values to 3-dimensional grid from
 *               point data (climatic stations, drill holes etc.) given in a
 *               sites file named input. Output grid3 file is elev. 
 *               Regularized spline with tension is used for the
 *               interpolation.
 *
 * COPYRIGHT:    (C) 1989, 1993, 2000 L. Mitas,  H. Mitasova,
 *               I. Kosinovsky, D. Gerdes, J. Hofierka
 *
 *               This program is free software under the GNU General Public
 *              License (>=v2). Read the file COPYING that comes with GRASS
 *              for details.
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

#define USERMAIN

#include <grass/gis.h>
#include <grass/site.h>
#include <grass/Vect.h>
#include "userglobs.h"

/* include G3d defs */
#include <grass/G3d.h>

#include "oct.h"
#include "surf.h"
#include "dataoct.h"
#include "user.h"
#include <grass/bitmap.h>
#include <grass/dbmi.h>
#include <grass/glocale.h>

/* pargr */
double ns_res, ew_res, tb_res;
double dmin, ertre;
int nsizr, nsizc, nsizl, KMAX2, KMIN, KMAX, KMAXPOINTS;

/* datgr */
double *az, *adx, *ady, *adxx, *adyy, *adxy, *adz, *adxz, *adyz, *adzz;
double ertot, ertre, zminac, zmaxac, wmult, zmult, zminacell, zmaxacell;
struct octtree *root;

int wtotal = 0;
int NPOINT = 0;
int OUTRANGE = 0;
int NPT = 0;

double DETERM;
int NERROR, cond1, cond2;
char fncdsm[32];
char filnam[10];

FILE *fdredinp, *fdzout, *fddxout, *fddyout, *fddzout,
    *fdxxout, *fdyyout, *fd4, *fxyout;
FILE *fddev = NULL;
int fdcell, fdcout;

/*
   x,y,z - input data
   npoint - number of input data
   fi - tension parameter
   b - coef. of int. function
   A - matrix of system of linear equations
   az- interpolated values z for output grid
   adx,ady, ... - estimation of derivatives for output grid
   nsizr,nsizc,nsizl - number of rows,columns and levels for output grid
   xmin ... - coordinates of corners of output grid
   scx,scy,scz - scale factors

   subroutines
   INPUT - input of data x,y,z,w (test function or measured data)
   interp_call - divides region on segments
   COGRR1 - interpolation of w-values and derivatives to grid
   secpar_loop - computation of secondary(morphometric) parameters
   OUTGR - output of gridded data and derivatives/sec.parameters
   POINT - interpolation of z-values to given point x,y
 */

/*
   Now reads site files using the new sites API (mca 2/12/96)
   Now writes 3d outputs using g3d format rather than grid3 (mca 2/13/96)

   03/11/00 (jh) - re-written I/O operations, several bug fixes
   including 3D topo parameters, and 2nd RST derivatives

   12/07/00 (MN) - added field selection parameter for sites lists

   02/03/03 (jh) - added deviation site file to the output

   13/05/2004 (MN) - updated to 5.7 (renamed field -> colnum to avoid confusion)

   04/01/2005 (jh) - variable smoothing and CV
 */

char *input;
char *cellinp = NULL;
char *cellout = NULL;
char *mapset = NULL;

char *scol = NULL;
char *outz = NULL;
char *gradient = NULL;
char *aspect1 = NULL;
char *aspect2 = NULL;
char *ncurv = NULL;
char *gcurv = NULL;
char *mcurv = NULL;

char *maskmap = NULL;
char *redinp = NULL;
char *devi = NULL;
char *cvdev = NULL;

int sdisk, disk;
FILE *Tmp_fd_z = NULL;
char *Tmp_file_z = NULL;

FILE *Tmp_fd_dx = NULL;
char *Tmp_file_dx = NULL;
FILE *Tmp_fd_dy = NULL;
char *Tmp_file_dy = NULL;
FILE *Tmp_fd_dz = NULL;
char *Tmp_file_dz = NULL;
FILE *Tmp_fd_xx = NULL;
char *Tmp_file_xx = NULL;
FILE *Tmp_fd_yy = NULL;
char *Tmp_file_yy = NULL;
FILE *Tmp_fd_xy = NULL;
char *Tmp_file_xy = NULL;
FILE *Tmp_fd_cell = NULL;
char *Tmp_file_cell = NULL;

struct BM *bitmask;
struct Cell_head cellhd;
G3D_Region out_region;

int main(int argc, char *argv[])
{
    int INPUT();
    int SEGMEN();
    int interp_call();
    int COGRR1();
    int SECPAR();
    int OUTGR();
    int LINEQS();
    int CRSTG();
    double CRST();
    double amax1();
    double amin1();
    int max1();
    int min1();
    int per, npmin;
    int ii, i, n_rows, n_cols, n_levs;
    double x_orig, y_orig, z_orig;
    char dminchar[64];
    struct octdata *data;
    struct octfunc *functions;
    struct octtree *tree;

    /*DEBUG  int testout = 1; */
    struct Map_info In;
    struct History hist;

    struct
    {
	struct Option *input, *colnum, *scol, *wheresql, *rescalex, *fi,
	    *segmax, *dmin1, *npmin, *npmax, *wmult, *outz, *rsm, *maskmap, *zmult,
	    *cvdev, *gradient, *aspect1, *aspect2, *ncurv, *gcurv, *mcurv,
	    *cellinp, *cellout, *devi;
    } parm;

    struct
    {
	struct Flag *cv;
    } flag;

    struct GModule *module;

    G_gisinit(argv[0]);

    module = G_define_module();
    module->keywords = _("vector, interpolation");
    module->description =
	_("Interpolates point data to a 3D raster map using "
	  "regularized spline with tension (RST) algorithm.");

    if (G_get_set_window(&cellhd) == -1)
	G_fatal_error("G_get_set_window() failed");

    G3d_getWindow(&current_region);
    G3d_readWindow(&current_region, NULL);

    ew_res = current_region.ew_res;
    ns_res = current_region.ns_res;
    n_cols = current_region.cols;
    n_rows = current_region.rows;
    x_orig = current_region.west;
    y_orig = current_region.south;
    n_levs = current_region.depths;
    tb_res = current_region.tb_res;
    z_orig = current_region.bottom;

    dmin = amin1(ew_res, ns_res) / 2;
    disk = n_rows * n_cols * sizeof(float);
    sprintf(dminchar, "%lf", dmin);

    nsizr = n_rows;
    nsizc = n_cols;
    nsizl = n_levs;
    n_rows_in = n_rows;		/* fix by JH 04/24/02 */

    parm.input = G_define_option();
    parm.input->key = "input";
    parm.input->type = TYPE_STRING;
    parm.input->required = YES;
    parm.input->gisprompt = "old,vector,vector";
    parm.input->description = _("Name of the vector map with input x,y,z,w");

    parm.cellinp = G_define_option();
    parm.cellinp->key = "cellinp";
    parm.cellinp->type = TYPE_STRING;
    parm.cellinp->required = NO;
    parm.cellinp->gisprompt = "old,cell,raster";
    parm.cellinp->description =
	_("Name of the surface raster map for cross-section");

    parm.colnum = G_define_option();
    parm.colnum->key = "wcolumn";
    parm.colnum->type = TYPE_STRING;
    parm.colnum->required = NO;
    parm.colnum->description =
	_("Name of the column containing w attribute to interpolate");
    parm.colnum->answer = "flt1";

    parm.fi = G_define_option();
    parm.fi->key = "tension";
    parm.fi->type = TYPE_DOUBLE;
    parm.fi->answer = TENSION;
    parm.fi->required = NO;
    parm.fi->description = _("Tension parameter");
    parm.fi->guisection = _("Settings");

    parm.rsm = G_define_option();
    parm.rsm->key = "smooth";
    parm.rsm->type = TYPE_DOUBLE;
    parm.rsm->answer = SMOOTH;
    parm.rsm->required = NO;
    parm.rsm->description = _("Smoothing parameter");
    parm.rsm->guisection = _("Settings");

    parm.scol = G_define_option();
    parm.scol->key = "scolumn";
    parm.scol->type = TYPE_STRING;
    parm.scol->required = NO;
    parm.scol->description =
	_("Name of the column with smoothing parameters");
    parm.scol->guisection = _("Settings");

    parm.wheresql = G_define_standard_option(G_OPT_WHERE);

    parm.devi = G_define_option();
    parm.devi->key = "devi";
    parm.devi->type = TYPE_STRING;
    parm.devi->required = NO;
    parm.devi->gisprompt = "new,vector,vector";
    parm.devi->description = _("Output deviations vector point file");
    parm.devi->guisection = _("Analysis");

    parm.cvdev = G_define_option();
    parm.cvdev->key = "cvdev";
    parm.cvdev->type = TYPE_STRING;
    parm.cvdev->required = NO;
    parm.cvdev->gisprompt = "new,vector,vector";
    parm.cvdev->description = _("Output cross-validation vector map");
    parm.cvdev->guisection = _("Analysis");

    parm.maskmap = G_define_option();
    parm.maskmap->key = "maskmap";
    parm.maskmap->type = TYPE_STRING;
    parm.maskmap->required = NO;
    parm.maskmap->gisprompt = "old,cell,raster";
    parm.maskmap->description = _("Name of the raster map used as mask");

    parm.segmax = G_define_option();
    parm.segmax->key = "segmax";
    parm.segmax->type = TYPE_INTEGER;
    parm.segmax->answer = MAXSEGM;
    parm.segmax->required = NO;
    parm.segmax->description = _("Maximum number of points in a segment");
    parm.segmax->guisection = _("Settings");

    parm.npmin = G_define_option();
    parm.npmin->key = "npmin";
    parm.npmin->type = TYPE_INTEGER;
    parm.npmin->answer = MINPOINTS;
    parm.npmin->required = NO;
    parm.npmin->description =
	_("Minimum number of points for approximation in a segment (>segmax)");
    parm.npmin->guisection = _("Settings");

    parm.npmax = G_define_option();
    parm.npmax->key = "npmax";
    parm.npmax->type = TYPE_INTEGER;
    parm.npmax->answer = MAXPOINTS;
    parm.npmax->required = NO;
    parm.npmax->description =
	_("Maximum number of points for approximation in a segment (>npmin)");
    parm.npmax->guisection = _("Settings");

    parm.dmin1 = G_define_option();
    parm.dmin1->key = "dmin";
    parm.dmin1->type = TYPE_DOUBLE;
    parm.dmin1->answer = dminchar;
    parm.dmin1->required = NO;
    parm.dmin1->description =
	_("Minimum distance between points (to remove almost identical points)");
    parm.dmin1->guisection = _("Settings");

    parm.wmult = G_define_option();
    parm.wmult->key = "wmult";
    parm.wmult->type = TYPE_DOUBLE;
    parm.wmult->answer = ZMULT;
    parm.wmult->required = NO;
    parm.wmult->description =
	_("Conversion factor for w-values used for interpolation");
    parm.wmult->guisection = _("Settings");

    parm.zmult = G_define_option();
    parm.zmult->key = "zmult";
    parm.zmult->type = TYPE_DOUBLE;
    parm.zmult->answer = ZMULT;
    parm.zmult->required = NO;
    parm.zmult->description = _("Conversion factor for z-values");
    parm.zmult->guisection = _("Settings");

    parm.cellout = G_define_option();
    parm.cellout->key = "cellout";
    parm.cellout->type = TYPE_STRING;
    parm.cellout->required = NO;
    parm.cellout->gisprompt = "new,cell,raster";
    parm.cellout->description = _("Output cross-section raster map");
    parm.cellout->guisection = _("Output_options");

    parm.outz = G_define_option();
    parm.outz->key = "elev";
    parm.outz->type = TYPE_STRING;
    parm.outz->required = NO;
    parm.outz->multiple = NO;
    parm.outz->gisprompt = "new,grid3,3d raster";
    parm.outz->description = _("Output elevation g3d-file");

    parm.gradient = G_define_option();
    parm.gradient->key = "gradient";
    parm.gradient->type = TYPE_STRING;
    parm.gradient->required = NO;
    parm.gradient->multiple = NO;
    parm.gradient->gisprompt = "new,grid3,3d raster";
    parm.gradient->description = _("Output gradient magnitude g3d-file");
    parm.gradient->guisection = _("Output_options");

    parm.aspect1 = G_define_option();
    parm.aspect1->key = "aspect1";
    parm.aspect1->type = TYPE_STRING;
    parm.aspect1->required = NO;
    parm.aspect1->multiple = NO;
    parm.aspect1->gisprompt = "new,grid3,3d raster";
    parm.aspect1->description =
	_("Output gradient horizontal angle g3d-file");
    parm.aspect1->guisection = _("Output_options");

    parm.aspect2 = G_define_option();
    parm.aspect2->key = "aspect2";
    parm.aspect2->type = TYPE_STRING;
    parm.aspect2->required = NO;
    parm.aspect2->multiple = NO;
    parm.aspect2->gisprompt = "new,grid3,3d raster";
    parm.aspect2->description = _("Output gradient vertical angle g3d-file");
    parm.aspect2->guisection = _("Output_options");

    parm.ncurv = G_define_option();
    parm.ncurv->key = "ncurv";
    parm.ncurv->type = TYPE_STRING;
    parm.ncurv->required = NO;
    parm.ncurv->multiple = NO;
    parm.ncurv->gisprompt = "new,grid3,3d raster";
    parm.ncurv->description = _("Output change of gradient g3d-file");
    parm.ncurv->guisection = _("Output_options");

    parm.gcurv = G_define_option();
    parm.gcurv->key = "gcurv";
    parm.gcurv->type = TYPE_STRING;
    parm.gcurv->required = NO;
    parm.gcurv->multiple = NO;
    parm.gcurv->gisprompt = "new,grid3,3d raster";
    parm.gcurv->description = _("Output gaussian curvature g3d-file");
    parm.gcurv->guisection = _("Output_options");

    parm.mcurv = G_define_option();
    parm.mcurv->key = "mcurv";
    parm.mcurv->type = TYPE_STRING;
    parm.mcurv->required = NO;
    parm.mcurv->multiple = NO;
    parm.mcurv->gisprompt = "new,grid3,3d raster";
    parm.mcurv->description = _("Output mean curvature g3d-file");
    parm.mcurv->guisection = _("Output_options");

    flag.cv = G_define_flag();
    flag.cv->key = 'c';
    flag.cv->description =
	_("Perform a cross-validation procedure without volume interpolation");

    if (G_parser(argc, argv))
	exit(EXIT_FAILURE);

    per = 1;			/*flag.per->answer; */
    iw2 = 1;
    sig1 = 0;
    input = parm.input->answer;

    cellinp = parm.cellinp->answer;
    cellout = parm.cellout->answer;
    scol = parm.scol->answer;
    maskmap = parm.maskmap->answer;
    outz = parm.outz->answer;
    devi = parm.devi->answer;
    cvdev = parm.cvdev->answer;
    gradient = parm.gradient->answer;
    aspect1 = parm.aspect1->answer;
    aspect2 = parm.aspect2->answer;
    ncurv = parm.ncurv->answer;
    gcurv = parm.gcurv->answer;
    mcurv = parm.mcurv->answer;

    cv = flag.cv->answer;

    ertre = 0.1;
    sscanf(parm.dmin1->answer, "%lf", &dmin);
    sscanf(parm.fi->answer, "%lf", &fi);
    sscanf(parm.rsm->answer, "%lf", &rsm);
    sscanf(parm.segmax->answer, "%d", &KMAX);
    sscanf(parm.npmin->answer, "%d", &npmin);
    sscanf(parm.npmax->answer, "%d", &KMAXPOINTS);
    sscanf(parm.wmult->answer, "%lf", &wmult);
    sscanf(parm.zmult->answer, "%lf", &zmult);

    if (rsm < 0.0)
	G_fatal_error(_("Smoothing must be a positive value"));

    if (parm.scol->answer)
	rsm = -1;		/* used in InterpLib to indicate variable smoothing */

    if (cv != (cvdev != NULL))
	G_fatal_error(_("Both crossvalidation options (-c, cvdev) must be specified"));
    if (cv && devi != NULL)
	G_fatal_error(_("Both crossvalidation and deviations file specified"));
    if (cellinp == NULL && outz == NULL && cellout == NULL && gradient == NULL
	&& aspect1 == NULL && aspect2 == NULL && ncurv == NULL &&
	gcurv == NULL && mcurv == NULL) {
	sig1 = 1;
    }

    if (cv &&
	(cellinp != NULL || outz != NULL || cellout != NULL ||
	 gradient != NULL || aspect1 != NULL || aspect2 != NULL ||
	 ncurv != NULL || gcurv != NULL || mcurv != NULL || devi != NULL))
	G_fatal_error
	    ("The crossvalidation cannot be computed simultanuously with output grids or devi file");

    z_orig_in = z_orig;
    tb_res_in = tb_res;
    z_orig = z_orig * zmult;
    tb_res = tb_res * zmult;

    /*    fprintf (stderr, "DMIN = %f\n", dmin); */

    /************************************/
    KMIN = npmin;

    /***************        KMAX2 = GRADPARAM1*npmax;***************/
    az = (double *)G_malloc(sizeof(double) * (n_cols + 1));
    if (!az)
	G_fatal_error("Not enough memory for az");
    adx = (double *)G_malloc(sizeof(double) * (n_cols + 1));
    if (!adx)
	G_fatal_error("Not enough memory for adx");
    ady = (double *)G_malloc(sizeof(double) * (n_cols + 1));
    if (!ady)
	G_fatal_error("Not enough memory for ady");
    adxx = (double *)G_malloc(sizeof(double) * (n_cols + 1));
    if (!adxx)
	G_fatal_error("Not enough memory for adxx");
    adyy = (double *)G_malloc(sizeof(double) * (n_cols + 1));
    if (!adyy)
	G_fatal_error("Not enough memory for adyy");
    adxy = (double *)G_malloc(sizeof(double) * (n_cols + 1));
    if (!adxy)
	G_fatal_error("Not enough memory for adxy");
    adz = (double *)G_malloc(sizeof(double) * (n_cols + 1));
    if (!adz)
	G_fatal_error("Not enough memory for adz");
    adxz = (double *)G_malloc(sizeof(double) * (n_cols + 1));
    if (!adxz)
	G_fatal_error("Not enough memory for adxz");
    adyz = (double *)G_malloc(sizeof(double) * (n_cols + 1));
    if (!adyz)
	G_fatal_error("Not enough memory for adyz");
    adzz = (double *)G_malloc(sizeof(double) * (n_cols + 1));
    if (!adzz)
	G_fatal_error("Not enough memory for adzz");


    if ((data =
	 data_new(x_orig, y_orig, z_orig, n_rows, n_cols, n_levs, 0)) == NULL)
	G_fatal_error("Unable to create octdata");
    if ((functions =
	 OT_functions_new(oct_compare, oct_divide_data, oct_add_data,
			  oct_intersect, oct_division_check,
			  oct_get_points)) == NULL)
	G_fatal_error("Unable to create octfunc");
    if ((tree = OT_tree_new(data, NULL, NULL, functions, 0)) == NULL)
	G_fatal_error("Unable to create octtree");

    root = tree;

    /*
       if (testout)
     */
    if (TESTOUT) {
	if ((fd4 = fopen("testout", "w+")) == NULL)
	    G_fatal_error("Unable to open testout");
    }

    Vect_set_open_level(1);
    Vect_open_old(&In, input, "");

    if (!Vect_is_3d(&In))
	G_warning(_("Vector is not 3D"));

    ii = INPUT(&In, parm.colnum->answer, parm.scol->answer,
	       parm.wheresql->answer);

    /* now NPOINT is available */
    KMAX2 =(NPOINT >= KMAXPOINTS ? KMAXPOINTS : NPOINT);

    Vect_close(&In);

    if (devi != NULL || cvdev != NULL) {
	Pnts = Vect_new_line_struct();
	Cats = Vect_new_cats_struct();
	db_init_string(&sql);

	if (devi != NULL)
	    Vect_open_new(&Map, devi, 1);
	else
	    Vect_open_new(&Map, cvdev, 1);
	Vect_hist_command(&Map);
	f = Vect_default_field_info(&Map, 1, NULL, GV_1TABLE);
	Vect_map_add_dblink(&Map, 1, NULL, f->table, "cat", f->database,
			    f->driver);
	/* Create new table */
	db_zero_string(&sql);
	sprintf(buf, "create table %s ( ", f->table);
	db_append_string(&sql, buf);
	db_append_string(&sql, "cat integer");
	db_append_string(&sql, ", flt1 double precision");
	db_append_string(&sql, ")");
	G_debug(1, db_get_string(&sql));
	driver = db_start_driver_open_database(f->driver, f->database);
	if (driver == NULL)
	    G_fatal_error(_("Unable to open database <%s> by driver <%s>"),
			  f->database, f->driver);

	if (db_execute_immediate(driver, &sql) != DB_OK) {
	    db_close_database(driver);
	    db_shutdown_driver(driver);
	    G_fatal_error(_("Cannot create table: %s"), db_get_string(&sql));
	}
	count = 1;
    }

    if (ii > 0) {
	if ((cellinp != NULL) && (cellout != NULL)) {
	    mapset = NULL;
	    mapset = G_find_cell2(cellinp, "");
	    if (mapset == NULL)
		G_fatal_error(_("Raster map <%s> not found"), cellinp);
	    fdcell = G_open_cell_old(cellinp, mapset);
	    if (fdcell < 0)
		G_fatal_error(_("Unable to open raster map <%s>"), cellinp);
	    fdcout = G_open_fp_cell_new(cellout);
	    if (fdcout < 0)
		G_fatal_error(_("Cannot open %s"), cellout);
	    zero_array_cell = (FCELL *) G_malloc(sizeof(FCELL) * n_cols);
	    if (!zero_array_cell)
		clean_fatal_error("Not enough memory for zero_array_cell\n");
	    for (i = 0; i < n_cols; i++)
		zero_array_cell[i] = 0;
	    Tmp_file_cell = G_tempfile();
	    if (NULL == (Tmp_fd_cell = fopen(Tmp_file_cell, "w+"))) {
		sprintf(msg, "Can't open temp file [%s] ", Tmp_file_cell);
		clean_fatal_error(msg);
	    }
	    /* filling temp file with zeroes */
	    for (i = 0; i < n_rows; i++) {
		if (!
		    (fwrite
		     (zero_array_cell, sizeof(FCELL), n_cols, Tmp_fd_cell)))
		    clean_fatal_error
			("Not enough disk space--cannot write temp files");
	    }
	} else
	  G_warning("Unable to create cellout raster map without cellinp");
	ertot = 0.;
	if (per)
	    fprintf(stderr, "Percent complete: ");

	out_cond1 = (outz != NULL) || (gradient != NULL) || (aspect1 != NULL)
	    || (aspect2 != NULL) || (gcurv != NULL) || (mcurv != NULL) ||
	    (ncurv != NULL);

	if (outz != NULL) {
	    /* allocating temp array for writing to corresponding temp file */
	    zero_array1 = (float *)G_malloc(sizeof(float) * n_cols);
	    if (!zero_array1)
		clean_fatal_error("Not enough memory for zero_array1\n");
	    for (i = 0; i < n_cols; i++)
		zero_array1[i] = 0.;
	    Tmp_file_z = G_tempfile();
	    if (NULL == (Tmp_fd_z = fopen(Tmp_file_z, "w+"))) {
		sprintf(msg, "Can't open temp file [%s] ", Tmp_file_z);
		clean_fatal_error(msg);
	    }
	    /* filling temp file with zeroes */
	    for (i = 0; i < n_levs * n_rows; i++) {
		if (!(fwrite(zero_array1, sizeof(float), n_cols, Tmp_fd_z)))
		    clean_fatal_error
			("Not enough disk space -- cannot write temp files");
	    }
	}
	if (gradient != NULL) {
	    /* allocating temp array for writing to corresponding temp file */
	    zero_array2 = (float *)G_malloc(sizeof(float) * n_cols);
	    if (!zero_array2)
		clean_fatal_error("Not enough memory for zero_array2\n");
	    for (i = 0; i < n_cols; i++)
		zero_array2[i] = 0.;
	    Tmp_file_dx = G_tempfile();
	    if (NULL == (Tmp_fd_dx = fopen(Tmp_file_dx, "w+"))) {
		sprintf(msg, "Can't open temp file [%s] ", Tmp_file_dx);
		clean_fatal_error(msg);
	    }
	    /* filling temp file with zeroes */
	    for (i = 0; i < n_levs * n_rows; i++) {
		if (!(fwrite(zero_array2, sizeof(float), n_cols, Tmp_fd_dx)))
		    clean_fatal_error
			("Not enough disk space -- cannot write temp files");
	    }
	}
	if (aspect1 != NULL) {
	    /* allocating temp array for writing to corresponding temp file */
	    zero_array3 = (float *)G_malloc(sizeof(float) * n_cols);
	    if (!zero_array3)
		clean_fatal_error("Not enough memory for zero_array3\n");
	    for (i = 0; i < n_cols; i++)
		zero_array3[i] = 0.;
	    Tmp_file_dy = G_tempfile();
	    if (NULL == (Tmp_fd_dy = fopen(Tmp_file_dy, "w+"))) {
		sprintf(msg, "Can't open temp file [%s] ", Tmp_file_dy);
		clean_fatal_error(msg);
	    }
	    /* filling temp file with zeroes */
	    for (i = 0; i < n_levs * n_rows; i++) {
		if (!(fwrite(zero_array3, sizeof(float), n_cols, Tmp_fd_dy)))
		    clean_fatal_error
			("Not enough disk space -- cannot write temp files");
	    }
	}
	if (aspect2 != NULL) {
	    /* allocating temp array for writing to corresponding temp file */
	    zero_array4 = (float *)G_malloc(sizeof(float) * n_cols);
	    if (!zero_array4)
		clean_fatal_error("Not enough memory for zero_array4\n");
	    for (i = 0; i < n_cols; i++)
		zero_array4[i] = 0.;
	    Tmp_file_dz = G_tempfile();
	    if (NULL == (Tmp_fd_dz = fopen(Tmp_file_dz, "w+"))) {
		sprintf(msg, "Can't open temp file [%s] ", Tmp_file_dz);
		clean_fatal_error(msg);
	    }
	    /* filling temp file with zeroes */
	    for (i = 0; i < n_levs * n_rows; i++) {
		if (!(fwrite(zero_array4, sizeof(float), n_cols, Tmp_fd_dz)))
		    clean_fatal_error
			("Not enough disk space -- cannot write temp files");
	    }
	}
	if (ncurv != NULL) {
	    /* allocating temp array for writing to corresponding temp file */
	    zero_array5 = (float *)G_malloc(sizeof(float) * n_cols);
	    if (!zero_array5)
		clean_fatal_error("Not enough memory for zero_array5\n");
	    for (i = 0; i < n_cols; i++)
		zero_array5[i] = 0.;
	    Tmp_file_xx = G_tempfile();
	    if (NULL == (Tmp_fd_xx = fopen(Tmp_file_xx, "w+"))) {
		sprintf(msg, "Can't open temp file [%s] ", Tmp_file_xx);
		clean_fatal_error(msg);
	    }
	    /* filling temp file with zeroes */
	    for (i = 0; i < n_levs * n_rows; i++) {
		if (!(fwrite(zero_array5, sizeof(float), n_cols, Tmp_fd_xx)))
		    clean_fatal_error
			("Not enough disk space -- cannot write temp files");
	    }
	}
	if (gcurv != NULL) {
	    /* allocating temp array for writing to corresponding temp file */
	    zero_array6 = (float *)G_malloc(sizeof(float) * n_cols);
	    if (!zero_array6)
		clean_fatal_error("Not enough memory for zero_array6\n");
	    for (i = 0; i < n_cols; i++)
		zero_array6[i] = 0.;
	    Tmp_file_yy = G_tempfile();
	    if (NULL == (Tmp_fd_yy = fopen(Tmp_file_yy, "w+"))) {
		sprintf(msg, "Can't open temp file [%s] ", Tmp_file_yy);
		clean_fatal_error(msg);
	    }
	    /* filling temp file with zeroes */
	    for (i = 0; i < n_levs * n_rows; i++) {
		if (!(fwrite(zero_array6, sizeof(float), n_cols, Tmp_fd_yy)))
		    clean_fatal_error
			("Not enough disk space -- cannot write temp files");
	    }
	}
	if (mcurv != NULL) {
	    /* allocating temp array for writing to corresponding temp file */
	    zero_array7 = (float *)G_malloc(sizeof(float) * n_cols);
	    if (!zero_array7)
		clean_fatal_error("Not enough memory for zero_array7\n");
	    for (i = 0; i < n_cols; i++)
		zero_array7[i] = 0.;
	    Tmp_file_xy = G_tempfile();
	    if (NULL == (Tmp_fd_xy = fopen(Tmp_file_xy, "w+"))) {
		sprintf(msg, "Can't open temp file [%s] ", Tmp_file_xy);
		clean_fatal_error(msg);
	    }
	    /* filling temp file with zeroes */
	    for (i = 0; i < n_levs * n_rows; i++) {
		if (!(fwrite(zero_array7, sizeof(float), n_cols, Tmp_fd_xy)))
		    clean_fatal_error
			("Not enough disk space -- cannot write temp files");
	    }
	}
	cursegm = 0;
	if (interp_call(root, root)) {
	    if (outz != NULL)
		G_free(zero_array1);
	    if (gradient != NULL)
		G_free(zero_array2);
	    if (aspect1 != NULL)
		G_free(zero_array3);
	    if (aspect2 != NULL)
		G_free(zero_array4);
	    if (ncurv != NULL)
		G_free(zero_array5);
	    if (gcurv != NULL)
		G_free(zero_array6);
	    if (mcurv != NULL)
		G_free(zero_array7);

	    G_message("Finished interpolating");
	    if (fd4 != NULL)
		fprintf(fd4, "max. error found = %f \n", ertot);
	    G_free(root);
	    G_free(az);
	    G_free(adx);
	    G_free(ady);
	    G_free(adz);
	    G_free(adxx);
	    G_free(adyy);
	    G_free(adxy);
	    G_free(adxz);
	    G_free(adyz);
	    G_free(adzz);

	    OUTGR();
	    if ((cellinp != NULL)) {
		G_close_cell(fdcout);
		if ((cellout != NULL)) {
		    G_short_history(cellout, "raster", &hist);
		    /* TODO: next lines need to be verified! */
		    sprintf(hist.edhist[0], "tension=%f, smoothing=%f", fi,
			    rsm);
		    sprintf(hist.edhist[1],
			    "dnorm=%f, dmin=%f, wmult=%f, zmult=%f", dnorm,
			    atof(parm.dmin1->answer), wmult, zmult);
		    sprintf(hist.edhist[2], "segmax=%d, npmin=%d, npmax=%d, rmsdevi=%f",
			    KMAX, npmin, KMAXPOINTS, sqrt(ertot / KMAX2));
		    sprintf(hist.edhist[3], "wmin_data=%f, wmax_data=%f",
			    wmin, wmax);
		    /* ? sprintf (hist.edhist[4], "wmin_int=%f, wmax_int=%f", wminac, wmaxac); */
		    hist.edlinecnt = 5;

		    G_command_history(&hist);
		    G_write_history(cellout, &hist);
		    G_message(_("Raster map <%s> created"), cellout);

		    fclose(Tmp_fd_cell);
		    unlink(Tmp_file_cell);
		}
		G_close_cell(fdcell);
	    }
	    if (outz != NULL) {
		fclose(Tmp_fd_z);
		unlink(Tmp_file_z);
	    }
	    if (gradient != NULL) {
		fclose(Tmp_fd_dx);
		unlink(Tmp_file_dx);
	    }
	    if (aspect1 != NULL) {
		fclose(Tmp_fd_dy);
		unlink(Tmp_file_dy);
	    }
	    if (aspect2 != NULL) {
		fclose(Tmp_fd_dz);
		unlink(Tmp_file_dz);
	    }
	    if (ncurv != NULL) {
		fclose(Tmp_fd_xx);
		unlink(Tmp_file_xx);
	    }
	    if (gcurv != NULL) {
		fclose(Tmp_fd_yy);
		unlink(Tmp_file_yy);
	    }
	    if (mcurv != NULL) {
		fclose(Tmp_fd_xy);
		unlink(Tmp_file_xy);
	    }

	    if (cvdev != NULL || devi != NULL) {
		db_close_database_shutdown_driver(driver);
		Vect_build(&Map);
		Vect_close(&Map);
		G_message(_("Vector map <%s> created"), cvdev);
	    }

	    fprintf(stderr, "\n");
	    fprintf(stderr, "The number of points in vector map is %d\n",
		    NPT);
	    fprintf(stderr,
		    "The number of points outside of 2D/3D region %d\n",
		    OUTRANGE);
	    fprintf(stderr,
		    "The number of points used (after reduction) is %d\n",
		    NPOINT);

	}
	else
	    clean_fatal_error(_("Interpolation failed"));
    }
    else
	clean_fatal_error(_("Input failed"));
    if (fd4 != NULL)
	fclose(fd4);

    return EXIT_SUCCESS;
}
