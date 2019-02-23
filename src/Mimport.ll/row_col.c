/* %W% %G% */

#include "geo.h"
row_col (geo, lat, lon, row, col)
    struct GEO *geo;
    double lat, lon;
    int *row, *col;
{
    *row = (geo->lat - lat) / geo->lat_res + .5;
    *col = (geo->lon - lon) / geo->lon_res + .5;
}
