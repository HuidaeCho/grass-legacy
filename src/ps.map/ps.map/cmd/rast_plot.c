/* Functions: PS_raster_plot, ps_get_map_row
**
** Author: Paul W. Carlson	3/92
** 
** ps_get_map_row is substituted by G_get_map_row_nomask
** writing mask file is done separately by function ps_write_mask_row
** which used code previously in ps_get_map_row. This is done because
** sometimes the raster file is not drawn, but we still need a mask
** so temporary mask file is created by new function PS_make_mask()
** which calls ps_write_mask_row()
** These changes are made by Olga Waupotitsch 4/94
*/

#include "ps_info.h"
#include "group.h"

static FILE *ps_mask_fp;
extern int verbose;
extern char *ps_mask_file;

PS_make_mask()
{
    int row;
    CELL *maskbuf;
    int maskfd, r, g, b;

    maskfd = G_maskfd();
    if(maskfd < 0 )
    /* there is no mask */
    {
       if(PS.mask_needed)
	  PS.mask_needed = 0;
       return;
    }
    if (maskfd >= 0) maskbuf = G_allocate_cell_buf();

    /* if masked, open a file to hold the PostScript mask data */
    if (maskfd >= 0 && PS.mask_needed)
    {
	if ((ps_mask_fp = fopen(ps_mask_file, "w")) == NULL)
	{
	    printf("\nCan't create temporary PostScript mask file.\n");
	    exit(-1);
	}

	/* get no data rgb values for mask */
    	G_get_color((CELL)0, &r, &g, &b, &PS.colors);
    	PS.r0 = (double)r / 255.0;
    	PS.g0 = (double)g / 255.0;
    	PS.b0 = (double)b / 255.0;

        for (row = 0; row < PS.w.rows; row++)
        {
	    G_get_map_row_nomask(maskfd, maskbuf, row);
	    ps_write_mask_row(maskbuf);
        }
        fclose(ps_mask_fp);
        free (maskbuf);

    }

}

PS_raster_plot()
{
    int i, n, r, g, b, rr, gg, bb, row, col, doing_color;
    CELL *cellbuf, *cbuf[3];

    if (!PS.do_raster && !grp.do_group) return;

    /* are we doing color? */
    doing_color = (PS.grey == 0 && PS.level == 2);

    /* save graphics state */
    fprintf(PS.fp, "gsave\n");

    /* make variables for cells_wide and cells_high */
    fprintf(PS.fp, "/cw %d def /ch %d def\n", PS.cells_wide, PS.cells_high);

    /* set lower left corner of map */
    fprintf(PS.fp, "%.2lf %.2lf TR\n", PS.map_left, PS.map_bot);

    /* mapping of image to map_pix_wide x map_pix_high unit rectangle */
    fprintf(PS.fp, "%d %d scale\n", 
	(int)(PS.map_pix_wide + 0.5), (int)(PS.map_pix_high + 0.5));


    /* make strings to hold image RGB values */
    if (doing_color) fprintf(PS.fp, "/imgstrg cw 3 mul string def\n");
    else	     fprintf(PS.fp, "/imgstrg cw string def\n");
    fprintf(PS.fp, "cw ch 8\n");
    fprintf(PS.fp, "[cw 0 0 ch neg 0 ch]\n");
    fprintf(PS.fp, "{currentfile imgstrg readhexstring pop}\n");
    if (doing_color) fprintf(PS.fp, "false 3 colorimage\n");
    else	     fprintf(PS.fp, "image\n");

    /* let user know what's happenning */
    if (verbose > 1)
    {
        if (PS.do_raster) printf("PS-PAINT: reading raster file <%s in %s> ...",
	    PS.cell_name, PS.cell_mapset);
        else printf("PS-PAINT: reading raster files in group <%s> ...",
	    grp.group_name);
        fflush(stdout);
    }

    /* build the image RGB string */
    if (PS.do_raster) 
    {
	cellbuf = G_allocate_cell_buf();
        n = 0;
        for (row = 0; row < PS.w.rows; row++)
        {
	    G_get_map_row(PS.cell_fd, cellbuf, row);
	    if ((row % PS.row_delta) == 0)
	    {   for (col = 0; col < PS.w.cols; col += PS.col_delta) 
	        {   
	            G_get_color(cellbuf[col], &r, &g, &b, &PS.colors);
    
		    /* if color raster */
		    if (doing_color)
		    {
		        fprintf(PS.fp, "%02X%02X%02X", r, g, b);
	                if (++n == 13)
	                {	
		            n = 0;
		            fprintf(PS.fp, "\n");
		        }
		    }
	    	
		    /* if grey raster */
		    else
		    {
		        fprintf(PS.fp, "%02X",
		            (int)(.3 * (double)r + .59 * (double)g + 
				  .11 * (double)b));
	                if (++n == 39)
	                {	
		            n = 0;
		            fprintf(PS.fp, "\n");
		        }
		    }
	        }
	    }
        }
    }
    else 
    {
	for (i = 0; i < 3; i++) cbuf[i] = G_allocate_cell_buf();
        n = 0;
        for (row = 0; row < PS.w.rows; row++)
        {
	    for (i = 0; i < 3; i++)
	        G_get_map_row(grp.fd[i], cbuf[i], row);
	    if ((row % PS.row_delta) == 0)
	    {   for (col = 0; col < PS.w.cols; col += PS.col_delta) 
	        {   
	    	    for (i = 0; i < 3; i++)
		    {
	              	G_get_color(cbuf[i][col], &rr, &gg, &bb, 
				&(grp.colors[i]));
			if (i == 0) r = rr;
			if (i == 1) g = gg;
			if (i == 2) b = bb;
		    }
			
		    /* if color raster */
		    if (doing_color)
		    {
		        fprintf(PS.fp, "%02X%02X%02X", r, g, b);
	                if (++n == 13)
	                {	
		            n = 0;
		            fprintf(PS.fp, "\n");
		        }
		    }
		}
	    }
	}
    }
    fprintf(PS.fp, "\n");

    /* we're done with the cell stuff */
    if (PS.do_raster)
    {
        if (!PS.do_colortable) G_free_colors(&PS.colors);
        G_close_cell(PS.cell_fd);
        free(cellbuf);
    }
    else
    {
   	for (i = 0; i < 3; i++)
	{
	    G_free_colors(&(grp.colors[i]));
	    G_close_cell(grp.fd[i]);
	    free(cbuf[i]);
	}
	I_free_group_ref(&grp.ref);
    }
    /* restore graphics state */
    fprintf(PS.fp, "grestore\n");

    if (verbose > 1) printf("\n");
}


ps_write_mask_row(mask)
    register CELL *mask;
{
   register int n;
   int i, j, byte;
   static int bit[] = { 128, 64, 32, 16, 8, 4, 2, 1 };

   i = 0;
   j = 0;
   byte = 0;
   n = PS.w.cols;
   while (n-- > 0)
   {
       if (*mask++ == 0) 
  	      byte |= bit[i];
       i++;
       if (i == 8)
       {	
          i = 0;
          j++;
          fprintf(ps_mask_fp, "%02X", byte);
          if (j == 39)
          {
              fprintf(ps_mask_fp, "\n");
              j = 0;
          }
          byte = 0;
        }
    }
    if(i)
    {
       while (i < 8)
       {
           if (*(mask - 1) == 0) byte |= bit[i];
           i++;
       }
       fprintf(ps_mask_fp, "%02X", byte);
    }
    fprintf(ps_mask_fp, "\n");
}
