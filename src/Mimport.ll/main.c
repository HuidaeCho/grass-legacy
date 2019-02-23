/* %W% %G% */

#include "geo.h"
#include "rowio.h"
main(argc,argv) char*argv[];
{
    ROWIO geo_data;
    CELL *utm_data, *uptr;

    struct Cell_head utm;
    struct GEO geo;
    int ll_fd;
    int cell_fd;
    char infile[300];
    char outfile[300];
    int n, nr;
    double north, east;
    double lat, lon;
    int row, col;
    int lrow, lcol;
    int getrow();

    G_gisinit (argv[0]);
    G_get_window (&utm);

    if (!getargs (argc, argv, &geo, infile, outfile))
	return usage(argv[0]);

    if (G_legal_filename (outfile) < 0)
    {
	fprintf (stderr,"%s - illegal cell file name\n", outfile);
	exit(1);
    }

    if ((ll_fd = open (infile, 0)) < 0)
    {	
	perror (infile);
	exit(1);
    }

/* cehck input file size */
    nr = geo.nrows * geo.ncols * geo.bpc;
    n = lseek (ll_fd, 0L, 2);
    if (n < nr)
    {
	fprintf (stderr, "%s - file size to small for input specifications\n",
		infile);
	exit(1);
    }
    if (n > nr)
    {
	fprintf (stderr,"warning: %s - file bigger than input specifications\n",
		infile);
    }

/* set the conversion parameters */
    CC_u2ll_spheroid_parameters (geo.a, geo.e);
    CC_u2ll_zone (utm.zone);

/* find out how many rows of input per row of output */
    CC_u2ll_north (utm.north);
    CC_u2ll (utm.east, &lat, &lon);
    row_col (&geo, lat, lon, &lrow, &lcol);
    nr = lrow;
    CC_u2ll (utm.west, &lat, &lon);
    row_col (&geo, lat, lon, &lrow, &lcol);
    nr -= lrow;
    if (nr < 0) nr = -nr;

    CC_u2ll_north (utm.south);
    CC_u2ll (utm.east, &lat, &lon);
    row_col (&geo, lat, lon, &lrow, &lcol);
    n = lrow;
    CC_u2ll (utm.west, &lat, &lon);
    row_col (&geo, lat, lon, &lrow, &lcol);
    n -= lrow;
    if (n < 0) n = -n;
    if (n > nr) nr = n;
    nr += 4;	/* a bit extra */

/* set up the input paging */
    rowio_setup (&geo_data, ll_fd, nr, geo.ncols * geo.bpc, getrow, NULL);

    cell_fd = G_open_cell_new (outfile);
    if (cell_fd < 0)
    {
	fprintf (stderr, "can't open new cell file %s\n", outfile);
	exit(1);
    }

    utm_data = G_allocate_cell_buf();


    north = utm.north - utm.ns_res/2 ;
    for (row = 0; row < utm.rows; row++)
    {
	CC_u2ll_north (north);

	east = utm.west + utm.ew_res/2 ;
	uptr = utm_data;
	for (col = 0; col < utm.cols; col++)
	{
	    CC_u2ll (east, &lat, &lon);
	    *uptr++ = geo_value (lat, lon, &geo, &geo_data);
	    east += utm.ew_res;
	}
	G_put_map_row (cell_fd, utm_data);
	north -= utm.ns_res ;
    }
    G_close_cell (cell_fd);
    exit(0);
}

getrow (fd, buf, row, len)
    char *buf;
{
    lseek (fd, (long) row*len, 0);
    if(read (fd, buf, len) == len)
	return 1;
    fprintf (stderr, "WARNING: error reading row %d of input file\n", row);
    G_zero (buf, len);
    return 1;
}
