/***********************************************************************
 * GRASS 5.0 coorcnv library
 * scan_ll.c, scanning of coordinates from strings
 *
 * Andreas Lange, andreas.lange@rhein-main.de
 * version 0.9
 * modified Jun 10 2000 
 *
 ***********************************************************************/

#include <stdio.h>
#include "CC.h"

static int scan_ll (char *, char *, double *);

int 
CC_lat_scan (char *buf, double *lat)
{
    return scan_ll (buf, "sn", lat);
}

int 
CC_lon_scan (char *buf, double *lon)
{
    return scan_ll (buf, "ew", lon);
}

static int 
scan_ll (char *buf, char *dir, double *result)
{
    char temp[3];
    int d, m, s;
    char ps[20], *pps;
    double p, f;

    if (sscanf (buf, "%d.%d.%d.%[0123456789]%1s", &d, &m, &s, ps, temp) == 5)
    {
	p = 0.0;
	f = .1;
	for (pps = ps; *pps; pps++)
	{
	    p += (*pps - '0') * f;
	    f /= 10.0;
	}
    }
    else if (sscanf (buf, "%d.%d.%d%1s", &d, &m, &s, temp) == 4)
    {
	p = 0.0;
    }
    else if (sscanf (buf, "%d.%d%1s", &d, &m, temp) == 3)
    {
	p = 0.0;
	s = 0 ;
    }
    else if (sscanf (buf, "%d%1s", &d, temp) == 2)
    {
	p = 0.0;
	s = m = 0;
    }
    else
	return 0;

    if (d < 0) return 0;
    if (m < 0 || m >= 60) return 0;
    if (s < 0 || s >= 60) return 0;

    if (*temp >= 'A' && *temp <= 'Z') *temp += 'a' -'A';
    if (*temp != dir[0] && *temp != dir[1]) return 0;

    *result = d*3600 + m*60 + s + p;
    if (*temp == dir[0]) *result = -(*result);
    return 1;
}
