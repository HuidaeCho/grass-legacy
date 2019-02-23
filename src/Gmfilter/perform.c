/* %W% %G% */
#include "filter.h"
#include "rowio.h"
#include "glob.h"

perform_filter (in_name, in_mapset, out_name, filter, nfilters, repeat)
    char *in_name, *in_mapset, *out_name;
    FILTER *filter;
{
    int in;
    int out;
    int n;
    int pass;
    ROWIO r;
    char *tmp1, *tmp2;
    int count;
    int getmaprow();
    int getrow();
    int  row;
    CELL *cell;


    cell = G_allocate_cell_buf();

if (!silent)
{printf ("FILTERING [%s] in [%s]", in_name, in_mapset);
 if (repeat>1 || nfilters>1) printf ("\n");
}

    count=0;
    for (pass=0; pass < repeat; pass++ )
    {
if (!silent && (repeat > 1)) printf ("PASS %d%s", pass+1, nfilters>1?"\n":" ...");
	for (n = 0; n < nfilters; n++, count++)
	{
if (!silent)
{
    if(nfilters > 1) printf ("%sFILTER %d ...", repeat>1?" ":"",n+1);
    else if(repeat==1) printf (" ...");
    fflush(stdout);
}

	    if (count==0)
	    {
		in = G_open_cell_old (in_name, in_mapset);
#ifdef DEBUG
printf ("open cell file %s in %s = %d\n", in_name, in_mapset, in);
#endif
		if (in < 0)
		{
		    char msg[100];
		    sprintf (msg, "unable to open cell file [%s] in [%s]",
			in_name, in_mapset);
		    G_fatal_error (msg);
		}
		close(creat(tmp1=G_tempfile(),0666));
		out = open (tmp1, 2);
		if (out < 0)
		    G_fatal_error ("unable to create a temporary file");
	    }
	    else if (count==1)
	    {
#ifdef DEBUG
printf ("close cell file\n");
#endif
		G_close_cell(in);
		in = out;
		close(creat(tmp2=G_tempfile(),0666));
		out = open (tmp2, 2);
		if (out < 0)
		    G_fatal_error ("unable to create a temporary file");
	    }
	    else
	    {
		int fd;

#ifdef DEBUG
printf ("swap temp files\n");
#endif
		fd = in;
		in = out;
		out = fd;
	    }

	    rowio_setup (&r, in, filter[n].size, buflen, count?getrow:getmaprow, NULL);

	    execute_filter (&r, out, &filter[n], cell);

	    rowio_release (&r);
if (!silent) printf ("\n");
	}
    }

    if (count==1)
	G_close_cell(in);
    else if (count > 1)
	close(in);

/* copy final result to output cell file */
    in = out;
    out = G_open_cell_new (out_name);
    if (out < 0)
    {
	char msg[100];
	sprintf (msg, "unable to create cell file [%s] in [%s]",
	    out_name, G_mapset());
	G_fatal_error (msg);
    }

if (!silent) printf ("WRITING [%s]\n", out_name);
    for (row=0; row<nrows; row++)
    {
	getrow (in, cell, row, buflen);
	G_put_map_row (out, cell);
    }

/* remove the temporary files before closing so that the G_close_cell()
   has more disk to work with
*/
if (!silent) printf ("CREATING SUPPORT FILES\n");
    if (count > 0)
	unlink (tmp1);
    if (count > 1)
	unlink (tmp2);
    G_close_cell (out);
}
