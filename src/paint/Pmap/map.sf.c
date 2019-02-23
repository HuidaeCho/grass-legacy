#include "gis.h"
#include "dlg.h"
#include "graphics.h"
#include "parms.h"
#include "vector.h"
#include "misc.h"
#include "fullwindow.h"

#define MARGIN 2

map(window)
    struct Cell_head *window;   /* current window */
{
    struct Cell_stats statf;
    int next_raster_row;
    int rasrow;
    unsigned char *rasptr;

    char *date;

    int extra_lines;       /* number of raster lines needed
			    * to paint vector info at end of map
			    */
    CELL *cell;	/* buffer for reading cell file	*/
    CELL *cptr;
    CELL *olcell1;	/* buffers for reading outline cell file	*/
    CELL *olcell2;
    CELL *maskcell;	/* buffer for reading MASK file	*/

    int prows, pcols;
    int max;		/* max number of printer pixels	*/
    int pixels;
    int npix;		/* # of pixels remaining in panel */
    int row;
    int col;
    int scol;
    int count;

    unsigned char black;    /* black catagory number      */
    unsigned char white;    /* white catagory number      */
    int npanels;  /* number of panels in map    */
    int panel;    /* current panel being printed */
    int i;
    uchar *red, *blu, *grn;
    short *red_carry_above;	/* color carryovers */
    short *grn_carry_above;
    short *blu_carry_above;
    int red_carry_left;
    int grn_carry_left;
    int blu_carry_left;
    int red_carry_save;
    int grn_carry_save;
    int blu_carry_save;

    int move_abs(), cont_abs();


/************************ begin *****************************/

/* get date */

    date = G_date();

/* build color tables needed for spreading the color around */
    build_color_tables();

/* Determine max number of pixels
 * allow for a border at left and right edges of panels
 */

    Pnpixels (&prows, &pcols);
    init_graphics (pcols);

    max = pcols;
    max -= MARGIN;

    if (parms.cellfd >= 0)
    {
	red_carry_above = (short *)G_calloc (pcols+2, sizeof (short));
	grn_carry_above = (short *)G_calloc (pcols+2, sizeof (short));
	blu_carry_above = (short *)G_calloc (pcols+2, sizeof (short));
	red = (uchar *)G_malloc (pcols);
	grn = (uchar *)G_malloc (pcols);
	blu = (uchar *)G_malloc (pcols);
    }


    white = WHITE;
    black = BLACK;

    cell = NULL ;
    olcell1 = NULL ;
    olcell2 = NULL ;
    maskcell = NULL ;


/*
 * adjust the window based on the scale
 */

    if (prows)
	unscaled (window, prows-MARGIN, max);
    else
	scale (window, max+MARGIN, MARGIN, parms.scaletext);

    fullwindow.ns_res = window->ns_res;
    fullwindow.ew_res = window->ew_res;
    fullwindow.rows   = window->rows;
    fullwindow.cols   = window->cols;

/* determine how many panels this map will require */

    npanels = (fullwindow.cols + max - 1) / max;

/*
 * determine the number of raster rows for the vector lines in the legend
 * the lines will drawn at the same width as in the map (plus lines of white
 * before and after)
 */
    extra_lines = 0;
    if (prows == 0)
    {
	for (i=0; i < vector.count; i++)
	    extra_lines += vector_info_lines(i);
	extra_lines += 2;
    }

/* initialize the stats */
    G_init_cell_stats (&statf);

/* begin! */

    G_setup_plot ((double) 0.0,
		  (double) fullwindow.rows,
		  (double) 0.0,
		  (double) fullwindow.cols,
		  move_abs, cont_abs);

/* print each panel */

    npix = fullwindow.cols;
    scol = 0;
    for (panel = 1; panel <= npanels; panel++)
    {
	if (parms.endpanel > 0 && panel > parms.endpanel)
	    break;
	if (verbose > 1)
	    printf("PAINT: panel %d (of %d)\n", panel, npanels);

/* compute number of pixels in this panel */

	pixels = npix > max ? max : npix;

/* adjust the east of the window for the number of cols */
	window->cols = pixels;
	window->east = window->west + window->ew_res * pixels;

	if (parms.startpanel > 0 && panel < parms.startpanel)
	    goto nextpanel;
	G_set_window (window);
	if (G_window_cols() != pixels) fprintf (stderr, "OOPS: window cols changed!!\n");

	if (cell)
	    free (cell);
	cell = G_allocate_cell_buf ();
	if (maskcell)
	    free (maskcell);
	maskcell = G_allocate_cell_buf ();

	if (parms.outlinefd >= 0)
	{
	    if (olcell1)
		free (olcell1);
	    if (olcell2)
		free (olcell2);
	    olcell1 = G_allocate_cell_buf ();
	    olcell2 = G_allocate_cell_buf ();
	    G_get_map_row (parms.outlinefd, olcell1, 0);
	}
	if (parms.cellfd >= 0)
	{
	    G_zero (red_carry_above, (window->cols+2) * sizeof (short)); 
	    G_zero (grn_carry_above, (window->cols+2) * sizeof (short)); 
	    G_zero (blu_carry_above, (window->cols+2) * sizeof (short)); 
	}

/* put printer in raster mode, define picture */

	Pflush () ;
	Praster ();
	Ppictsize (fullwindow.rows+extra_lines, pixels+MARGIN);
	Pflush () ;

/* one white line to start */
	if (prows == 0)
	{
	    Prle_begin();
	    Prle (white, pixels+MARGIN);
	    Prle_end();
	    Pflush ();
	}

/* some title info */

	if (prows == 0)
	{
	    Palpha ();
	    header (panel, npanels, date);
	    title (parms.celltitle);
	    Praster ();
	    Pflush () ;
	}

/* print top border */

	Prle_begin();
	Prle (black, pixels+MARGIN);
	Prle_end();

/* print each row of the map */

	next_raster_row = -1;
	for (row = 0; row < fullwindow.rows; row++)
	{
	    red_carry_left = red_carry_save = 0;
	    grn_carry_left = grn_carry_save = 0;
	    blu_carry_left = blu_carry_save = 0;

	    if (row >= next_raster_row)	/* must clear raster area */
	    {
		next_raster_row = set_graphics (window, row, scol);

		do_labels (0) ;
		do_vectors (0);
		do_plfile (0);

		mask_vectors (maskcell);

		do_vectors (1);
		do_plfile (1);
		do_sites();
		do_labels (1);


		graphics.drawover = 0;
		do_grid ();
		graphics.drawover = 1;

		if (parms.grid_numbers > 0)
		    do_grid_numbers (panel);

		rasrow = 0 ;
	    }
	    rasptr = graphics.raster[rasrow++] ;

	    if (parms.cellfd >= 0)
	    {
		G_get_map_row (parms.cellfd, cptr = cell, row);
		if (parms.with_colortable)
		    G_update_cell_stats (cell, pixels, &statf);
		G_get_color_array (cell, red, grn, blu, &parms.pcolr, pixels);
	    }
	    if (parms.outlinefd >= 0)
	    {
		CELL *temp;

		temp = olcell1;
		olcell1 = olcell2;
		olcell2 = temp;
		G_get_map_row (parms.outlinefd, olcell1, row);
	    }

	    Prle_begin ();
	    Prle (black, 1);	/* left border */

	    col = scol;
	    for (count = 0; count < pixels; count++)
	    {
		unsigned char c;
		unsigned char ras;
		int n,r,g,b;

		c = white;
		if (parms.cellfd >= 0)
		{

		    r = (int)red[count] + ( 3*red_carry_left +
				              red_carry_above[count] +
				            3*red_carry_above[count+1]+
				              red_carry_above[count+2]
				          ) / 8;

		    g = (int)grn[count] + ( 3*grn_carry_left +
				              grn_carry_above[count] +
				            3*grn_carry_above[count+1]+
				              grn_carry_above[count+2]
				          ) / 8;

		    b = (int)blu[count] + ( 3*blu_carry_left +
				              blu_carry_above[count] +
				            3*blu_carry_above[count+1]+
				              blu_carry_above[count+2]
				          ) / 8;


		    red_carry_above[count] = red_carry_save;
		    red_carry_save = red_carry_left;
		    red_carry_left = red_carryover(r);

		    grn_carry_above[count] = grn_carry_save;
		    grn_carry_save = grn_carry_left;
		    grn_carry_left = grn_carryover(g);

		    blu_carry_above[count] = blu_carry_save;
		    blu_carry_save = blu_carry_left;
		    blu_carry_left = blu_carryover(b);


		    n = lookup_from_pattern(*cptr++,row,col++) ;
		    if (n < 0)
			n = printer_color_number (r,g,b);
		    c = n;
		}
		if (ras = *rasptr++)
		    c = ras - 1;
		else if (parms.outlinefd >= 0)
		{
		    if (olcell1[count] != olcell2[count])
			c = parms.outline_color;
		    else if (count && (olcell1[count] != olcell1[count-1]))
			c = parms.outline_color;
		}
		Prle (c, 1);
	    }

	    Prle (black, 1);	/* right border */

	    Prle_end () ;
	}

/* print bottom border */

	Prle_begin();
	Prle (black, pixels+MARGIN);
	Prle_end();

/* trailer info */

	if (prows == 0)
	{
	    Pflush () ;
	    Palpha  ();
	    trailer (window, parms.grid, parms.scaletext);

	    vector_info (pixels+MARGIN);

	    Praster ();
	    Pflush () ;
	}

/* a trailing white line */
	if (prows == 0)
	{
	    Prle_begin();
	    Prle (white, pixels+MARGIN);
	    Prle_end();
	}

nextpanel:
	npix -= pixels;
	scol += pixels;
	window->west = window->east;
	Pflush () ;
    }

/* if unscaled, skip what follows */
    if (prows)
    {
	if (verbose > 1)
	    printf("PAINT: complete\n");
	return;
    }

/* do the color table */

    if (parms.with_colortable)
    {
	if (verbose > 1)
	    printf("PAINT: printing color table\n");
	ctable (&parms.pcats, &parms.pcolr, &statf);
    }

/* comments */
    if (parms.commentfile != NULL)
    {
	FILE *fd;
	char line[1024];

	if (verbose > 1)
	    printf("PAINT: printing comments\n");

	Palpha ();

	if (fd = fopen (parms.commentfile, "r"))
	{
	    *line = 0;
	    for (i = 0; i < 100; i++)
		strcat (line,"-");
	    Ptext (line);
	    while (G_getl (line, sizeof line, fd))
		Ptext (line);
	    for (i = 0; i < 5; i++)
		Ptext ("");
	    fclose (fd);
	}
	else
	    error ("comment file","","can't open");
    }
    if (verbose > 1)
	printf("PAINT: complete\n");
}
