#include "xgrass_dlib.h"
#include "gis.h"
#include "win_dim.h"
#include "image.h"


/*
 ***************************************************************************
 * XgdBWImage - Create a black & white image from the given map and mapset.
 ***************************************************************************
 */
void
#ifdef _NO_PROTO
XgdBWImage(obj, scrn, name, mapset)
     XgdObject      *obj;
     int             scrn;
     char           *name, *mapset;
#else
XgdBWImage(XgdObject *obj, int scrn, char *name, char *mapset)
#endif
{
  char            buf[120];
  struct Cell_head cellhead;
  int             fd, row, col;
  register CELL  *cell;
  int             nrows, ncols;
  int             newbwwidth, newbwheight;
  int             crow, ccol;
  double          delta_x, delta_y, row_inc, col_inc;
  float           gap;
  
  fd = G_open_cell_old(name, mapset);
  if (fd < 0) {
    sprintf(buf,
	    "%s in %s -can't open cell file",
	    name, mapset);
    XgdError(buf);
  }
  cell = G_allocate_cell_buf();
  
  cellhead = obj->Obj.GeoFrame.region;
  
  nrows = G_window_rows();
  ncols = G_window_cols();
  
  if (obj->width == ncols && obj->height == nrows) {
    col_inc = 1.0;
    row_inc = 1.0;
    crow = obj->width;
    ccol = obj->height;
    gap = 0.0;
  } else {
    gap = 0.5;
    /*XgSetwh(obj->Obj.GeoFrame.region,
	    &col_inc, &row_inc, obj->width, obj->height);
*/
    if (row_inc < 0.0 && col_inc < 0.0) {
      newbwwidth = (int) (double) (ncols - 1) / col_inc + 1;
      newbwheight = (int) (double) (nrows - 1) / row_inc + 1;
    } else {
      newbwwidth = (int)
	((double) (ncols - 0.5) / col_inc) + 2;
      newbwheight = (int)
	((double) (nrows - 0.5) / row_inc) + 2;
    }
  }
  
  
  if (!obj->Obj.GeoFrame.colorsExist)
    if (G_read_colors(name, mapset, &obj->Obj.GeoFrame.colors) < 0) {
      XgdError("Color file not available");
    }

  obj->Obj.GeoFrame.colorsExist = True;
  
  obj->Obj.GeoFrame.image = XCreateImage(obj->display,
			 DefaultVisual(obj->display, scrn),
			 DefaultDepth(obj->display, scrn),
			 ZPixmap, 0, None,
			 newbwwidth, newbwheight, 8, 0);
  
  if (!obj->Obj.GeoFrame.image)
    fprintf(stderr, "Fatal Error - Couldn't create image.\n");
  
  obj->Obj.GeoFrame.image->data = (char *) XtMalloc(newbwwidth * newbwheight);
  
  if (col_inc >= 1.0 && row_inc >= 1.0) {
    crow = 0;
    /* loop over rows */
    for (row = 0; row < nrows;
	 row = (int) (crow * row_inc + gap)) {
      if (G_get_map_row(fd, cell, row) < 0)
	exit(-1);
      
      ccol = 1;
      col = 0;
      
      /* loop over cols */
      if (row % 2 == 0) {
	while (col < ncols) {
	  switch
	    ((int) obj->Obj.GeoFrame.lookup_tbl[cell[col] - (int) obj->Obj.GeoFrame.colors.fixed.min + 1]) {
	    case 0:
	      XPutPixel(obj->Obj.GeoFrame.image, (ccol - 1), crow,
			BlackPixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, (ccol - 1), crow + 1,
			BlackPixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, ccol, crow,
			BlackPixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, ccol, crow + 1,
			BlackPixel(obj->display, scrn));
	      break;
	      
	    case 1:
	      XPutPixel(obj->Obj.GeoFrame.image, (ccol - 1), crow,
			BlackPixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, (ccol - 1), crow + 1,
			WhitePixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, ccol, crow,
			BlackPixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, ccol, crow + 1,
			WhitePixel(obj->display, scrn));
	      break;
	      
	    case 2:
	      XPutPixel(obj->Obj.GeoFrame.image, (ccol - 1), crow,
			BlackPixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, (ccol - 1), crow + 1,
			WhitePixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, ccol, crow,
			WhitePixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, ccol, crow + 1,
			BlackPixel(obj->display, scrn));
	      break;
	      
	    case 3:
	      XPutPixel(obj->Obj.GeoFrame.image, (ccol - 1), crow,
			BlackPixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, (ccol - 1), crow + 1,
			WhitePixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, ccol, crow,
			WhitePixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, ccol, crow + 1,
			WhitePixel(obj->display, scrn));
	      break;
	      
	    case 4:
	      XPutPixel(obj->Obj.GeoFrame.image, (ccol - 1), crow,
			WhitePixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, (ccol - 1), crow + 1,
			WhitePixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, ccol, crow,
			WhitePixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, ccol, crow + 1,
			WhitePixel(obj->display, scrn));
	      break;
	    }
	  col = (int) (ccol * col_inc + gap) + 2;
	  ccol++;
	}
	XPutImage(obj->display, obj->Obj.GeoFrame.pixmap, obj->objgc, obj->Obj.GeoFrame.image,
		  0, crow, obj->x, obj->y + crow, ccol, 1);
	crow = crow + 2;
      }
    }
  } else {
    int             rrow, rcol;
    crow = 0;
    
    /* loop over rows */
    for (row = 0; row < newbwheight - 1; row++) {
      rrow = (int) (row_inc * row);
      if (rrow >= nrows)
	rrow = nrows - 1;
      if (G_get_map_row(fd, cell, rrow) < 0)
	exit(-1);
      
      ccol = 1;
      col = 0;
      
      /* loop over cols */
      if (row % 2 == 0) {
	while (col < newbwwidth - 1) {
	  rcol = (int) (double) col *col_inc;
	  if (rcol >= ncols)
	    rcol = ncols - 1;
	  switch
	    ((int) obj->Obj.GeoFrame.lookup_tbl[cell[rcol] - (int) obj->Obj.GeoFrame.colors.fixed.min + 1]) {
	    case 0:
	      XPutPixel(obj->Obj.GeoFrame.image, (ccol - 1), crow,
			BlackPixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, (ccol - 1), crow + 1,
			BlackPixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, ccol, crow,
			BlackPixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, ccol, crow + 1,
			BlackPixel(obj->display, scrn));
	      break;
	      
	    case 1:
	      XPutPixel(obj->Obj.GeoFrame.image, (ccol - 1), crow,
			BlackPixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, (ccol - 1), crow + 1,
			WhitePixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, ccol, crow,
			BlackPixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, ccol, crow + 1,
			WhitePixel(obj->display, scrn));
	      break;
	      
	    case 2:
	      XPutPixel(obj->Obj.GeoFrame.image, (ccol - 1), crow,
			BlackPixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, (ccol - 1), crow + 1,
			WhitePixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, ccol, crow,
			WhitePixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, ccol, crow + 1,
			BlackPixel(obj->display, scrn));
	      break;
	      
	    case 3:
	      XPutPixel(obj->Obj.GeoFrame.image, (ccol - 1), crow,
			BlackPixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, (ccol - 1), crow + 1,
			WhitePixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, ccol, crow,
			WhitePixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, ccol, crow + 1,
			WhitePixel(obj->display, scrn));
	      break;
	      
	    case 4:
	      XPutPixel(obj->Obj.GeoFrame.image, (ccol - 1), crow,
			WhitePixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, (ccol - 1), crow + 1,
			WhitePixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, ccol, crow,
			WhitePixel(obj->display, scrn));
	      XPutPixel(obj->Obj.GeoFrame.image, ccol, crow + 1,
			WhitePixel(obj->display, scrn));
	      break;
	    }
	  col = col + 2;
	  ccol = ccol + 2;
	}
	XPutImage(obj->display, obj->Obj.GeoFrame.pixmap, obj->objgc, obj->Obj.GeoFrame.image,
		  0, crow, obj->x, obj->y + crow, ccol, 2);
	crow = crow + 2;
      }
    }
  }
  
  
  newwidth = newbwwidth;
  newheight = newbwheight;
  G_close_cell(fd);
}
