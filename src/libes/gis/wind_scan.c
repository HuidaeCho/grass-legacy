#include "gis.h"

G_scan_northing (buf, northing, projection)
    char *buf;
    double *northing;
{
    if (projection == PROJECTION_LL)
    {
	if(G_lat_scan (buf, northing))
	    return 1;
	if (!scan_double (buf, northing))
	    return 0;
	return (*northing <= 90.0 && *northing >= -90.0);
    }
    return scan_double (buf, northing);
}

G_scan_easting (buf, easting, projection)
    char *buf;
    double *easting;
{
    if (projection == PROJECTION_LL)
    {
	if (G_lon_scan (buf, easting))
	    return 1;
	if (!scan_double (buf, easting))
	    return 0;
	while (*easting > 180.0)
	    *easting -= 360.0;
	while (*easting < -180.0)
	    *easting += 360.0;
	return 1;
    }
    return scan_double (buf, easting);
}

G_scan_resolution (buf, res, projection)
    char *buf;
    double *res;
{
    if (projection == PROJECTION_LL)
    {
	if(G_llres_scan (buf, res))
		return 1;
    }
    return (scan_double (buf, res) && *res > 0.0);
}

static
scan_double (buf, value)
    char *buf;
    double *value;
{
    char junk[2];

/* use sscanf to convert buf to double
 * make sure value doesn't have other characters after it
 */
    *junk = 0;
    *value = 0.0;
    return (sscanf (buf, "%lf%1s", value, junk) == 1 && *junk == 0);
}
