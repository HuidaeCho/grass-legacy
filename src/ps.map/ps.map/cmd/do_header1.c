/* Functions: do_map_header, read_header_file
**
** Author: Paul W. Carlson	April 1992
*/

#include "ps_info.h"
#include "header.h"

int 
do_map_header (void)
{
    char buf[100];
    char temp[100];
    double x, y, dy, fontsize;
    static char *loc = "LOCATION:     ";

    /* set font */
    fontsize = (double)hdr.fontsize;
    fprintf(PS.fp, "(%s) FN %.1lf SF\n", hdr.font, fontsize);

    /* set start of first line */
    dy = 1.5 * fontsize;
    x = 72.0 * PS.left_marg + 1.5;
    y = 72.0 * (PS.page_height - PS.top_marg) - fontsize - 1.5;

    if (hdr.fp == NULL)
    {

    	if (PS.celltitle[0])
    	{
	    fprintf(PS.fp, "(%s) SW pop %.1lf add /xo XD\n", loc, x);
	    fprintf(PS.fp, "(TITLE:) %.1lf %.1lf MS\n", x, y);
	    fprintf(PS.fp, "(%s) xo %.1lf MS\n", PS.celltitle, y);
	    y -= dy;
    	    strcpy(temp, G_myname());
    	    G_strip(temp);
    	    if (*temp == 0) strcpy(temp, G_location());
	    fprintf(PS.fp, "(%s) %.1lf %.1lf MS\n", loc, x, y);
	    fprintf(PS.fp, "(%s) show\n", temp);
	    y -= 0.25 * dy;
	}
    	if (PS.min_y > y) PS.min_y = y;
	return;
    }
	    
    read_header_file();
    y -= 0.25 * dy;
    if (PS.min_y > y) PS.min_y = y;
}

    
int 
read_header_file (void){}
