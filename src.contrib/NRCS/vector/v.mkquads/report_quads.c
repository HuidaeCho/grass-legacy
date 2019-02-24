

/*  This takes all the information we have accumulated so far and creates
*   the vector file.  Creates the quads line by line.
*   lat is north/south,  lon is east/west.
*   A quad is not a square box.  
*   Written by Grass Team, Fall of 88, -mh.
*/

#include	<stdio.h>
#include	"quad_structs.h"
#include	"dig_defines.h"

report_quads (fp_report, Q, flags)
	FILE  *fp_report ;
	struct  quads_description  *Q ;
	struct  command_flags  *flags ;
{

	int  i, k ;
	int  num_v_rows, num_v_cols ;
	double  x,  y ;
	double  lon, lat ;
	double  lon_shift,  lat_shift ;

	num_v_rows = Q->num_vect_rows ;
	num_v_cols = Q->num_vect_cols ;


	lat_shift =  Q->lat_shift ;
	lon_shift =  Q->lon_shift ;


	fprintf( fp_report, "\n%30s\n", "QUAD POINTS") ;
/*	fprintf( fp_report, "\n         Spheroid Type Used: '%s'\n\n",
		Q->spheroid_name) ;
*/
	fprintf( fp_report, "         Lon             Lat               Easting       Northing\n") ;
	fprintf( fp_report, "         ---             ---               -------       --------\n") ;

/*  write out all the vector lengths (x vectors) of the entire grid  */


	lat = Q->origin_lat ;
	for ( i = 0; i < num_v_rows; ++i)
	{
		lon = Q->origin_lon ;
		for (k = 0; k < num_v_cols; ++k)
		{
			convert_ll_to_utm( lon, lat, &x, &y, Q) ;
			write_report_line( fp_report, k+1, lon, lat, x, y) ;
			lon += lon_shift ;
		}
		fprintf( fp_report, "\n") ;

		lat += lat_shift ;
	}

	if(flags->encompass)
	{
	fprintf( fp_report, "\n These quad points encompass the area.\n") ;
	fprintf( fp_report, "\n Widen the region to see the results.\n") ;
	}

	return (0) ;

}


static  write_report_line( fp_report, cnt, lon, lat, x, y )
	FILE  *fp_report ;
	int  cnt ;
	double  lon, lat, x, y ;
{
	char  lon_buf[80] ;
	char  lat_buf[80] ;

/*	CC_lon_format ( lon, lon_buf) ;*/
RG_lon_format ( lon, lon_buf) ;
	CC_lat_format ( lat, lat_buf) ;

	fprintf( fp_report, " %d)   %-15s  %-15s    %10.2lf    %10.2lf\n",
		cnt, lon_buf, lat_buf, x, y) ;
}

 
RG_lon_format (lon, buf)
    double lon;
    char *buf;
{
    int d,m;
    char h;
    double s;
		      
    RG_lon_parts (lon, &d, &m, &s, &h);
    rg_format (buf, d,m,s,h);
}
 
RG_lon_parts (lon, d, m, s, h)
    double lon;     /* lon to be split into parts */
    int *d, *m;     /* degrees, minutes */
    double *s;      /* seconds */
    char *h;        /* hemisphere */
{
    if (lon < 0)
    {
    *h = 'W' ;
    lon = -lon;
    }
    else
    *h = 'E' ;
    rg_ll_parts (lon, d, m, s);
}
 
static
rg_ll_parts (ll, d, m, s)
    double ll;  /* ll to be split into parts */
    int *d, *m; /* degrees, minutes */
    double *s;  /* seconds */
{
    long x;
		  
    x = ll;
		       
    *d = x / 3600 ;
    *m = (x % 3600) / 60;
    *s = ll - *d * 3600 - *m * 60 ;
}
static
rg_format (buf, d,m,s,h)
    char *buf;
    int d,m;
    double s;
    char h;
{
    char temp[50];
    double ss;
    int x;
			     
    sprintf (temp, "%lf", s);
    sscanf (temp, "%lf", &ss);
    if (ss >= 60)
    {
        ss = 0; /* force it to zero */
        if (++m >= 60)
        {
        m = 0;
        d++;
        }
    }
	   
    sprintf (temp, "%lf", ss);
    x = strlen (temp);
    while (x-- > 0)
        if (temp[x] == '0' || temp[x] == '.')
            temp[x] = 0;
    else
        break;
    if (*temp)
    {
        if (temp[0] == '.')
            sprintf (buf, "%d.%02d.00%s%c", d, m, temp, h);
        else if (temp[1] == '.')
            sprintf (buf, "%d.%02d.0%s%c", d, m, temp, h);
        else
            sprintf (buf, "%d.%02d.%s%c", d, m, temp, h);
    }
    else if (m > 0)
        sprintf (buf, "%d.%02d%c", d, m, h);
    else
        sprintf (buf, "%d%c", d, h);
}

