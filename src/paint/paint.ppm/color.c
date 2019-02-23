static int red_nlevels, red_mult,
	   grn_nlevels, grn_mult,
	   blu_nlevels, blu_mult;
static int *red_level_to_255, red_level[256], red_value[256],
           *grn_level_to_255, grn_level[256], grn_value[256],
           *blu_level_to_255, blu_level[256], blu_value[256];

/* for dithering! */
static short red_low[256], red_hi[256], red_extra[256],
             grn_low[256], grn_hi[256], grn_extra[256],
             blu_low[256], blu_hi[256], blu_extra[256];

#define DITHER_ROWS 8
#define DITHER_COLS 8

static short dither_matrix[DITHER_ROWS][DITHER_COLS] =
    {
       { 0, 24, 36, 60,  2, 26, 38, 62},
       {44, 52,  8, 16, 46, 54, 10, 18},
       {28,  4, 56, 32, 30,  6, 58, 34},
       {48, 40, 20, 12, 50, 42, 22, 14},
       { 3, 27, 39, 63,  1, 25, 37, 61},
       {47, 55, 11, 19, 45, 53,  9, 17},
       {31,  7, 59, 35, 29,  5, 57, 33},
       {51, 43, 23, 15, 49, 41, 21, 13}

/*
	{ 0,  6,  9, 15},
        {11, 13,  2,  4},
        { 7,  1, 14,  8},
        {12, 10,  5,  3}
*/
    };

#define DITHER_SIZE DITHER_ROWS * DITHER_COLS

build_color_tables()
{
    char *xalloc();

/* get the info from the printer driver */
    Pcolorlevels      (&red_nlevels, &grn_nlevels, &blu_nlevels);
    Pcolormultipliers (&red_mult,    &grn_mult,    &blu_mult);

/* these next tables will be used to
   convert a printer level to a number in the range 0-255
*/
    red_level_to_255 = (int *) xalloc (red_nlevels * sizeof (int));
    grn_level_to_255 = (int *) xalloc (grn_nlevels * sizeof (int));
    blu_level_to_255 = (int *) xalloc (blu_nlevels * sizeof (int));

    build (red_nlevels, red_mult, red_level, red_value, red_level_to_255);
    build (grn_nlevels, grn_mult, grn_level, grn_value, grn_level_to_255);
    build (blu_nlevels, blu_mult, blu_level, blu_value, blu_level_to_255);
}

build_dither_tables()
{
    mix(red_low, red_hi, red_extra, red_level_to_255, red_nlevels, DITHER_SIZE);
    mix(grn_low, grn_hi, grn_extra, grn_level_to_255, grn_nlevels, DITHER_SIZE);
    mix(blu_low, blu_hi, blu_extra, blu_level_to_255, blu_nlevels, DITHER_SIZE);
}

static
build (nlevels, mult, level, value, level_to_255)
    int nlevels, mult;
    int *level, *value, *level_to_255;
{
    int i;
    int first, last;

    first = 0;
    for (i = 0; i < nlevels; i++)
    {
	last = (255.0/nlevels) * (i+1);
	while (first <= last)
	{
	    level[first] = i;
	    value[first] = i*mult;
	    first++;
	}
	level_to_255[i] = i * 255.0/(nlevels-1);
    }
    while (first <= 255)
    {
	level[first] = (nlevels-1);
	value[first] = (nlevels-1) * mult;
	first++;
    }
}

/* this routine converts rgb color to a printer color number
   the rgb are from 0-255. 
*/
printer_color_number (red, grn, blu)
    register int red, grn, blu;
{
    return red_value[red] + grn_value[grn] + blu_value[blu];
}

/* this next routine supports matrix dithering
 * given an input color intensity, it computes the exact printer
 * color intensities above and below, plus a ratio to mix the
 * two to get the input colors.
 *
 */
static
mix (low, hi, extra, level_to_255, nlevels, steps)
    short *low, *hi, *extra;
    int *level_to_255;
{
    int color;
    int i;

    for (color = 0; color < 256; color++, low++, hi++, extra++)
    {
	for (i = 1; i < nlevels; i++)
	{
	    if (color >= level_to_255[i-1] && color < level_to_255[i])
		break;
	}
	if (i < nlevels)
	{
	    *low = level_to_255[i-1];
	    *hi  = level_to_255[i];
	    *extra = (color - *low) * steps / (*hi - *low);
	}
	else
	{
	    *hi = *low = 255;
	    *extra = 0;
	}
    }
}

static
dither (color, row, col, ncols, low, hi, extra)
    unsigned char *color;
    short *low, *hi, *extra;
{
    short *dp;

    dp =  dither_matrix[row%DITHER_ROWS];
    col = DITHER_COLS - (col%DITHER_COLS);

    while (--ncols >= 0)
    {
	if (extra[*color] > dp[--col])
	    *color = hi[*color];
	else
	    *color = low[*color];
	if (col == 0) col = DITHER_COLS;
	color++;
    }
}

red_dither (color, row, col, ncols)
    unsigned char *color;
{
    dither (color, row, col, ncols, red_low, red_hi, red_extra);
}

grn_dither (color, row, col, ncols)
    unsigned char *color;
{
    dither (color, row, col, ncols, grn_low, grn_hi, grn_extra);
}

blu_dither (color, row, col, ncols)
    unsigned char *color;
{
    dither (color, row, col, ncols, blu_low, blu_hi, blu_extra);
}
