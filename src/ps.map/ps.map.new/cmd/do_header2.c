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

    /* set font */
    fontsize = (double)hdr.fontsize;
    fprintf(PS.fp, "(%s) FN %.1lf SF\n", hdr.font, fontsize);

    /* set start of first line */
    dy = 1.5 * fontsize;
    y = 72.0 * (PS.page_height - PS.top_marg) - fontsize - 1.5;

    if (hdr.fp == NULL)
    {

    	if (PS.celltitle[0])
    	{
	    fprintf(PS.fp, "/t (TITLE:  %s) def\n", PS.celltitle);
    	    strcpy(temp, G_myname());
    	    G_strip(temp);
    	    if (*temp == 0) strcpy(temp, G_location());
	    fprintf(PS.fp, "/l (LOCATION:  %s) def\n", temp);
	    fprintf(PS.fp, "t SW pop /tw XD l SW pop /lw XD\n");
	    fprintf(PS.fp, "tw lw lt {/xw lw def} {/xw tw def} ifelse\n");
	    fprintf(PS.fp, "%.1lf xw sub D2 /xo XD\n", 72.0 * PS.page_width);
	    fprintf(PS.fp, "t xo %.1lf MS\n", y);
	    y -= dy;
	    fprintf(PS.fp, "l xo %.1lf MS\n", y);
	    y -= 0.25 * dy;
	}
    	if (PS.min_y > y) PS.min_y = y;
	return;
    }
	    
    x = 72.0 * PS.left_marg + 1.5;
    read_header_file();
    y -= 0.25 * dy;
    if (PS.min_y > y) PS.min_y = y;
}

    
int 
read_header_file (void){}
