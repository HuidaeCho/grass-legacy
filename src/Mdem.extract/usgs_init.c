/* %W% %G% */
#include "usgs.h"
#define   max(a,b) (a>b?a:b)

usgs_init()
{
	buffer    = G_malloc(blocksize + 1024);
	buf_start = buffer;
	buf_end   = buf_start + blocksize;

/* get full window information  */

	G_get_set_window(&cellhd);

/* adjust borders to correspond to cell centers */

	n = cellhd.north - cellhd.ns_res/2;
	s = cellhd.south + cellhd.ns_res/2;
	e = cellhd.east - cellhd.ew_res/2;
	w = cellhd.west + cellhd.ew_res/2;

/*  allocate column profile buffer */

	profile_buf_size = max(cellhd.cols,cellhd.rows) * sizeof(CELL);
	profile_buf = (CELL *) G_malloc(profile_buf_size);

	inf = G_tempfile();
	of = G_tempfile();
	fd = creat(inf,0660);
	
/*  open tape drive */

	if((tapefile =  open(tapename,0))== -1)
	{
		G_fatal_error("can't open tape drive");
		exit(1);
	}
	window_list(stdout);

/*  zero out the entire file that will receive data   */

	G_zero(profile_buf,profile_buf_size);
	for(col = 0; col < cellhd.cols; col++)
	    write(fd,profile_buf,cellhd.rows * sizeof(CELL));
}
