/*
 *************************************************************************
 * G_get_window (window)
 *     struct Cell_head *window
 *
 *      read the current mapset window
 *      dies if error
 *
 *************************************************************************
 * G_get_default_window (window)
 *     struct Cell_head *window
 *
 *      read the default window for the location
 *      dies if error
 *
 *************************************************************************
 * char *
 * G__get_window (window, element, name, mapset)
 *      read the window 'name' in 'element' in 'mapset'
 *      returns NULL if ok, error message if not
 ************************************************************************/

#include <stdlib.h>
#include "G.h"
#include "gis.h"
#include "glocale.h"


/*!
 * \brief read the database region
 *
 * Reads the database region as stored in the WIND file in the user's
 * current mapset <b>into region.</b>
 * An error message is printed and exit( ) is called if there is a problem reading
 * the region.
 * <b>Note.</b> GRASS applications that read or write raster files should not
 * use this routine since its use implies that the active module region will not
 * be used. Programs that read or write raster file data (or vector data) can
 * query the active module region <i>using G_window_rows and
 * G_window_cols..</i>
 *
 *  \param region
 *  \return int
 */

int G_get_window (struct Cell_head *window )
{
    static int first = 1;
    static struct Cell_head dbwindow ;

    if (first)
    {
	char *err;

	if(err = G__get_window (&dbwindow,"","WIND",G_mapset()))
	{
	    G_fatal_error (_("region for current mapset %s\nrun \"g.region\""), err);
	    G_free (err);
	}
    }

    first = 0;
    G_copy ((char *) window, (char *) &dbwindow, sizeof(dbwindow) ) ;

    if (!G__.window_set)
    {
	G__.window_set = 1;
	G_copy((char *) &G__.window, (char *) &dbwindow, sizeof(dbwindow) ) ;
    }

    return 1;
}


/*!
 * \brief read the default region
 *
 * Reads the default region for the location into
 * <b>region.</b>
 * An error message is printed and exit( ) is called if there is a problem
 * reading the default region.
 *
 *  \param region
 *  \return int
 */

int G_get_default_window ( struct Cell_head *window )
{
    char *err;

    if ((err = G__get_window (window,"","DEFAULT_WIND","PERMANENT")))
    {
	G_fatal_error (_("default region %s"), err);
	G_free (err);
    }
    return 1;
}

char *G__get_window ( struct Cell_head *window,
       char *element, char *name, char *mapset)
{
    FILE *fd ;
    char *err;
    char *G__read_Cell_head();

    G_zero ((char *) window, sizeof (struct Cell_head));
    if (!(fd = G_fopen_old (element, name, mapset) ))
    {
/*
char path[1024];
G__file_name (path,element,name,mapset);
fprintf (stderr, "G__get_window(%s)\n",path);
*/
	return G_store (_("is not set"));
    }

    err = G__read_Cell_head(fd, window, 0);
    fclose (fd);

    if (err)
    {
	char msg[1024];

	sprintf (msg, _("is invalid\n%s"), err);
	G_free (err);
	return G_store (msg);
    }

    return NULL;
}
