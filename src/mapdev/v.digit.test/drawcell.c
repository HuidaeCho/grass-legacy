#include <stdlib.h>
#include "digit.h"
#include "gis.h"
#include "display.h"
#include "keyboard.h"
#include "raster.h"
#include "local_proto.h"

int drawcell(void)
{
    int fd;
    int left, top;
    int ncols, nrows;
    int row;
    CELL *cell;
    struct Colors colr;
    int repeat;
    char buf[100];
    struct Cell_head cellhd;
    int ret = 0;


    /*  Do I want this?
    D_clear_screen ();
    */

    G_get_set_window (&cellhd);  /* read WINDOW Information from window_rout () */

    if (Disp_flex)    G_put_window (&cellhd) ; /*OHLER*/
   
    if(G_read_colors (N_backdrop, N_backdrop_mapset, &colr) < 0)
	return 0;
    
    D_set_colors (&colr);

    if (Disp_flex) eq_grey_colors (N_backdrop,N_backdrop_mapset, &colr,0) ;  /*OHLER*/

    nrows = G_window_rows();
    ncols = G_window_cols();

    /*
    top =   0;
    left = 0;
    */
    top =   1;  /* make it sit inside the outline box in digit */
    left = 1;

    R_standard_color (WHITE);
    /*
    Outline_box (top, top+nrows-1, left, left+ncols-1);
    */

    fd = G_open_cell_old (N_backdrop, N_backdrop_mapset);
    if (fd < 0)
	return 0;
    cell = G_allocate_cell_buf();


    set_keyboard ();
    for (row = 0; row < nrows; row += repeat)
    {
	if (key_hit (buf))
        {
            if (*buf == ESC)
            {
                ret = -1;
                break;
            }
        }

	R_move_abs (left, top+row);
	if(G_get_map_row_nomask(fd, cell, row) < 0)
	    break;
	repeat = G_row_repeat_nomask (fd, row);
	D_raster (cell, ncols, repeat, &colr);
    }
    V_flush ();
    unset_keyboard ();
    G_close_cell (fd);
    G_free_colors (&colr);
    free (cell);

    return ret;
}
