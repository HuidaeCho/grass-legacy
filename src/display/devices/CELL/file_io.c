

/*
**  code to write data to tmp file 
*/
#include "cell.h"

extern int SCREEN_RIGHT  ;

int store_xy (int x, int y)
{
/*DEBUG fprintf (stderr, "STORE_XY  (%d,%d)  %d\n", x, y, Cur_color); */
    fseek (Temp_fp, (long) y*SCREEN_RIGHT+x-1, 0);
    fwrite (&Cur_color, 1, 1,  Temp_fp);

    return 0;
}

int horiz_line (int y, int x1, int x2)
{
    register int i, len;

    len = x2-x1+1;
    for (i = 0 ; i < len ; i++)
	Row_buf[i] = Cur_color;

    fseek (Temp_fp, (long) y*SCREEN_RIGHT+x1-1, 0);
    fwrite (Row_buf, 1, len,  Temp_fp);
    return (0);
}
