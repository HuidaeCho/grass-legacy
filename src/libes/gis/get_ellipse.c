#include "gis.h"

static struct table
{
    char *name;
    double a;
    double e2;
} *table = NULL;
static int count = -1;

/*
 * This routine returns the ellipsoid parameters from the database.
 * If the file ELLIPSOID exist in the PERMANENT mapset, read info from
 * that file, otherwise return WGS 84 values.
 *
 * Returns: 1 ok, 0 default values used.
 * Dies with diagnostic if there is an error
 */
G_get_ellipsoid_parameters (a, e2)
    double *a, *e2;
{
    FILE *fd;
    char buf1[100], buf2[100];
    int stat;
    char err[1024];

    static char *ELLIPSOID = "ELLIPSOID";
    static char *PERMANENT = "PERMANENT";
    if (G_find_file ("", ELLIPSOID, PERMANENT) == NULL)
    {
	*a = 6378137.0 ;
	*e2 = .006694385 ;
	return 0;
    }
    fd = G_fopen_old ("", ELLIPSOID, PERMANENT);
    if (fd == NULL)
    {
	sprintf (err, "Unable to open file %s in %s", ELLIPSOID, PERMANENT);
	G_fatal_error (err);
    }
    stat = fscanf (fd, "%s %s", buf1, buf2);
    fclose (fd);
    switch (stat)
    {
    case 1:
	read_ellipsoid_table(1);
	if (G_get_ellipsoid_by_name (buf1, a, e2))
	    return 1;
	sprintf (err, "%s in %s: %s - unknown ellipsoid",
		    ELLIPSOID, PERMANENT, buf1);
	G_fatal_error (err);
	exit(-1); /*NOTREACHED*/

    case 2:
	if (get_a_e2 (buf1, buf2, a, e2) || get_a_e2 (buf2, buf1, a, e2))
	    return 1;
	sprintf (err, "%s in %s: %s %s - illegal ellipsoid specified",
		    ELLIPSOID, PERMANENT, buf1, buf2);
	G_fatal_error (err);
	exit(-1); /*NOTREACHED*/
    
    default:
	sprintf (err, "%s in %s - illegal file format", ELLIPSOID, PERMANENT);
	G_fatal_error (err);
	exit(-1); /*NOTREACHED*/
    }
/*NOTREACHED*/
}

/*
 * looks up ellipsoid in ellipsoid table and returns the
 * a, e2 parameters for the ellipsoid
 *
 * returns 1 if ok,
 *         0 if not found in table
 */

G_get_ellipsoid_by_name (name, a, e2)
    char *name;
    double *a, *e2;
{
    int i;

    read_ellipsoid_table(0);
    for (i = 0; i < count; i++)
    {
	if (same(name, table[i].name))
	{
	    *a = table[i].a;
	    *e2 = table[i].e2;
	    return 1;
	}
    }
    return 0;
}

/*
 * returns name(s) of ellipsoid
 *
 * for (i = 0; name = G_ellipsoid_name(i); i++)
 *         ....
 */
char *
G_ellipsoid_name (n)
{
    read_ellipsoid_table(0);
    return n>=0 && n < count ? table[n].name : NULL;
}

static
same (a, b)
    char *a, *b;
{
    while (*a && *b)
	if (lower(*a++) != lower (*b++))
	    return 0;
    
    return (*a == 0 && *b == 0);
}

static
lower (c)
{
    if (c >= 'A' && c < 'Z')
	c += 'a' - 'A' ;
    return c;
}

static
get_a_e2 (s1, s2, a, e2)
    char *s1;
    char *s2;
    double *a, *e2;
{
    double b,f;

    if (sscanf (s1, "a=%lf", a) != 1)
	return 0;
    if (*a <= 0.0) return 0;

    if (sscanf (s2, "e=%lf", e2) == 1)
	return (*e2 > 0.0);

    if (sscanf (s2, "f=1/%lf", &f) == 1)
    {
	if (f <= 0.0) return 0;
	f = 1/f;
	*e2 = f+f - f*f;
	return (*e2 > 0.0);
    }

    if (sscanf (s2, "b=%lf", &b) == 1)
    {
	if (b <= 0.0) return 0;
	f = b/(*a);
	*e2 = 1.0 - f*f;
	return (*e2 > 0.0);
    }
    return 0;
}

static
char *
ellipsoid_table_file(file)
    char *file;
{
    sprintf (file, "%s/etc/ellipse.table", G_gisbase());
    return file;
}

static
read_ellipsoid_table(fatal)
{
    FILE *fd;
    char file[1024];
    char buf[1024];
    char name[100], buf1[100], buf2[100];
    char badlines[256];
    int line;
    int err;

    if (count >= 0)
	return 1;
    count = 0;
    table = NULL;

    ellipsoid_table_file (file);
    fd = fopen (file, "r");
    if (fd == NULL)
    {
	perror (file);
	sprintf (buf, "unable to open ellipsoid table file: %s", file);
	fatal ? G_fatal_error(buf) : G_warning (buf);
	return 0;
    }

    err = 0;
    *badlines = 0;
    for (line = 1; G_getl (buf, sizeof buf, fd); line++)
    {
	G_strip (buf);
	if (*buf == 0 || *buf == '#')
	    continue;

	if (sscanf (buf, "%s %s %s", name, buf1, buf2) != 3)
	{
	    err++;
	    sprintf (buf, " %d", line);
	    if (*badlines) strcat (badlines, ",");
	    strcat (badlines, buf);
	    continue;
	}

	table = (struct table *) G_realloc (table, (count+1) * sizeof(*table));
	table[count].name = G_store (name);

	if(get_a_e2 (buf1, buf2, &table[count].a, &table[count].e2)
	|| get_a_e2 (buf2, buf1, &table[count].a, &table[count].e2))
	    count++;
	else
	{
	    err++;
	    sprintf (buf, " %d", line);
	    if (*badlines) strcat (badlines, ",");
	    strcat (badlines, buf);
	    continue;
	}
    }
    if (!err) return 1;
    sprintf (buf, "Line%s%s of ellipsoid table file <%s> %s invalid",
	err==1?"":"s", badlines, file, err==1?"is":"are");
    fatal ? G_fatal_error(buf) : G_warning (buf);
    return 0;
}
