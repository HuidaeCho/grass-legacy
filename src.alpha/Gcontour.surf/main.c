/* this program makes an elevation cell file from a contour map
** that has been converted to cell form.
** last modified 04/06/90
** by  Chuck Ehlschlaeger
*/

#define MAIN
#include "contour.h"
#undef MAIN

main(argc,argv)
int argc;
char *argv[];
{
	SHORT	r,c;
	CELL	con1, con2;
	double	d1, d2;
	CELL	*alt_row;
	char	con_name[40], alt_name[40], *con_mapset, con_f, alt_f, s_f;
	CELL	*readmap();
	int	file_fd;
	int	incr;
	char	buf[80];
	CELL	value;
	FLAG	*flag_create();

	G_gisinit(argv[0]);
	con_f = alt_f = s_f = 0;
	on = 1;
	off = 0;
	for (r = 1; r < argc; r++)
	{
		if (sscanf (argv[r], "input=%[^\n]", con_name) == 1) 
		{
			con_f++;
		}
		else if (sscanf (argv[r], "output=%[^\n]",alt_name) == 1)
		{
			alt_f++;
		}
		else if (strcmp (argv[r], "-f") == 0)
		{
			s_f++;
		}
		else 
		{
			sprintf(buf,"bad input: %s\n",argv[r]);
			G_fatal_error(buf);
		}
	}
	if((!con_f) || (!alt_f))
	{
		sprintf(buf,"missing inputs: Gcontour.surf [-f] input=contour_cell_map output=surface_cell_map\n");
		G_fatal_error(buf);
	}
	con_mapset = G_find_cell2(con_name, "");
	if (!con_mapset)
	{
		sprintf(buf, "contour cell file [%s] not found\n",
			con_name);
		G_fatal_error (buf);
		exit(1);
	}
	nrows = G_window_rows();
	ncols = G_window_cols();
	i_val_l_f = nrows + ncols;
	cseg_open (&con, 16, 16, 8);
	cseg_read_cell (&con, con_name, con_mapset);
	if (s_f)
	{
		seen = flag_create (nrows, ncols);
	}
	else
	{
		bseg_open (&bseen, 64, 64, 16);
	}
	alt_row = (CELL *)G_malloc(ncols * sizeof(CELL));
	zero = (NODE *)G_malloc(INIT_AR * sizeof(NODE));
	minc = minr = 0;
	maxc = ncols - 1;
	maxr = nrows - 1;
	array_size = INIT_AR;
	fprintf (stderr, "Doing %d rows\n", (int) nrows);
	incr = 2;
	file_fd = G_open_cell_new(alt_name);
	if(!file_fd)
		exit(2);
	for (r=0; r<nrows; incr--, r++)	
	{
		if (incr == 0)
		{
			fprintf (stderr, 
				"Starting row %d of %d\n", 
				(int) r, (int) nrows);
			incr = 10;
		}
		for (c=0; c<ncols; c++)
		{
			cseg_get (&con, r, c, &value);
			if (value != 0) 
			{
				alt_row[c] = value;
			}
			else
			{
			    if (s_f)
			    {
			    	find_con(r,c,&d1,&d2,&con1,&con2);
			    }
			    else
			    {
				find_con_slow(r,c,&d1,&d2,&con1,&con2);
			    }
			    if(con2 > 0)
			    {
				alt_row[c] = (int)(d2 * con1 / (d1 + d2) +
					d1 * con2 / (d1 + d2) + 0.5);
			    }
			    else alt_row[c] = con1;
			}
		}
		G_put_map_row(file_fd, alt_row, r);
	}
	cseg_close (&con);
	if (s_f)
		flag_destroy (seen);
	else
		bseg_close (&bseen);
	G_close_cell(file_fd);
	exit(0);
}
