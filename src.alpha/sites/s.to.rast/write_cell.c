#include "gis.h"
write_cell (fd, window, north, east, quad_size, cat)
    int fd;
    struct Cell_head *window;
    double north, east;
    int quad_size;
    CELL cat;
{
    CELL cell;
    int row;
    int col;
    int startrow, endrow;
    int startcol, endcol;
    
    row = (int) G_northing_to_row (north, window);
    col = (int) G_easting_to_col (east, window);

    startcol = col - quad_size ;
    endcol   = col + quad_size ;
    if (startcol < 0)
        startcol = 0;
    if (endcol >= window->cols)
        endcol = window->cols - 1;

    if (endcol < startcol)
        return 0;

    startrow = row - quad_size ;
    endrow   = row + quad_size ;
    if (startrow < 0)
        startrow = 0;
    if (endrow >= window->rows)
        endrow = window->rows - 1;

    if (endrow < startrow)
        return 0;

    cell = cat;
    for (row = startrow; row <= endrow; row++)
        for (col = startcol; col <= endcol; col++)
            G_put_map_row_random (fd, &cell, row, col, 1);
}
