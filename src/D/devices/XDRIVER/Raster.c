#include "sun.h"
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

extern Display *dpy;
extern Window  grwin;
extern Pixmap  bkupmap;
extern GC      gc;
XImage *grimage;
static short alloc = 0 ;
static short gotimage = 0;

/*
 * Write 'nrows' lines of 'num' pixels contained as ints in 'array' to the
 * screen starting at the current position. 'withzeros' indicates that
 * zero-valued pixels in array should either be written or left unchanged.
 * 'color_type' indicates that the pixel values should either be written
 * directly to the screen or used to reference a color look-up table.
 */

Raster(num, nrows, array, withzeros, color_type)
	int num ;
	int nrows ;
	int *array ;
	int withzeros ;
	int color_type ;
{
	int cur_x, cur_y ;
	int *arr ;
	char *calloc(), *realloc() ;
	int i,j ;

	XWindowAttributes xwa;
	char *imdata;

	int (*assign_color)() ;
	int _get_color_index() ;
	int do_nothing() ;
	
	/* set up the color look up table if needed */
	if(color_type)
		assign_color = _get_color_index ;
	else
		assign_color = do_nothing ;
	
/* Get current x,y position. Note that we lose current x,y */
	Get_current_xy(&cur_x, &cur_y) ;
	sun_x = 99999999, sun_y = 99999999 ; 

/* Unless this is 1st time thru or if raster line length has changed,
 * we don't need to reallocate space or re-create the Ximage.
 */
	if (alloc < num) {
	    if(gotimage) {
		XDestroyImage(grimage); /* destroy any previous images */
		gotimage = 0;
	    }
	    if (alloc == 0 )
		imdata = calloc(num, sizeof(*imdata)) ;
	    else
		imdata = realloc(imdata, sizeof(*imdata)*num) ;
	    if (imdata == NULL)
		return(-1) ;    /* not enough space left */
	    alloc = num ;
	    if (XGetWindowAttributes(dpy,grwin,&xwa) == 0)
		return (-1);
	    grimage = XCreateImage(dpy,xwa.visual,8,XYPixmap,
				   0,imdata,num,1,8,0);
	    gotimage = 1;
	}

/* If zeros are to be included, an entire raster row can be constructed */
	if (withzeros) {
	    char *pix ;
	    arr = array ;
	    pix=grimage->data ;
	    for(i=0;i<num;i++)
		*pix++ = (char)(*assign_color)(*arr++) ;
	    /*
		XPutPixel(grimage,i,0,(unsigned long)(*assign_color)(*arr++));
	    */
	    for(i=0; i<nrows; i++){
    		XPutImage(dpy,grwin,gc,grimage,0,0,cur_x,cur_y+i,num,1);
    		XPutImage(dpy,bkupmap,gc,grimage,0,0,cur_x,cur_y+i,num,1);
	    }
	}
/* If zeros are not included may need to draw many shorter rasters.
 * If the pixel value in array is zero we don't disturb the existing 
 * pixel of the drawable. If the pixel is non-zero we re-write it.
 */
	else {
		int start_col, width ;

		arr = array ;
		start_col = 0 ;
		width  = 0 ;
		for(j=0;j<num;j++)
		{
		    if (*arr == 0)
		    {
			if(width > 0)
			{
			    for(i=0; i<nrows; i++){
				XPutImage(dpy,grwin,gc,grimage,0,0,
					  cur_x+start_col,cur_y+i,width,1);
				XPutImage(dpy,bkupmap,gc,grimage,0,0,
					  cur_x+start_col,cur_y+i,width,1);
			    }
			    width  = 0 ;
			    start_col = j ;
			}
			else
			{
			    start_col++ ;
			}
		    }
		    else    /* non-zero pixel, put into the image */
		    {
			XPutPixel(grimage,width++,0,
				  (unsigned long)(*assign_color)(*arr));
		    }
		    arr++ ;
		}
		/* Flush out any remaining data */
		if(width > 0)
		{
		    for(i=0; i<nrows; i++)
			XPutImage(dpy,grwin,gc,grimage,0,0,
				  cur_x+start_col,cur_y+i,width,1);
			XPutImage(dpy,bkupmap,gc,grimage,0,0,
				  cur_x+start_col,cur_y+i,width,1);
		}
	}
}

static do_nothing(n)
{
	return(n) ;
}
