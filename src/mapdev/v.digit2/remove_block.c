/*
**  Written by Dave Gerdes  6/1988
**  US Army Construction Engineering Research Lab
*/

#include "digit.h"
#include "dig_head.h"
#include "wind.h"
#include "gis.h"

remove_block (Map)
    struct Map_info *Map;
{
    char buffer[64] ;
    double x, y ;
    int screen_x, screen_y ;
    double ux1, uy1 ;
    double ux2, uy2 ;
    double sx1, sy1 ;
    double sx2, sy2 ;
    int button ;
    int cur_screen_x, cur_screen_y ;
    double N, S, E, W;
    double delta;
    int yn;
    int cnt;
    char buf[1024];
    register int i;

    Clear_info ();
    _Clear_base ();
    _Write_base (12, "Buttons:") ;
    _Write_base (13, "Left:   Establish a corner") ;
    _Write_base (14, "Middle: Abort") ;
    Write_base  (15, "Right:  Accept window") ;

    cur_screen_x = (int)D_west ;
    cur_screen_y = (int)D_south ;
/*
    screen_x = (int)D_east ;
    screen_y = (int)D_north ;
*/
    screen_x = cur_screen_x + 10 ;
    screen_y = cur_screen_y + 10 ;

    sx1 = ux1 = U_west ;
    sy1 = uy1 = U_south ;
    sx2 = ux2 = U_east ;
    sy2 = uy2 = U_north ;
top:

    while (1)
    {
	R_get_location_with_box(cur_screen_x, cur_screen_y, &screen_x, &screen_y, &button) ;
	Clear_info ();

	switch (button) {
	    case 1:
		if ( cur_screen_x == screen_x  &&  cur_screen_y == screen_y)
		{
		    Write_info(2, "Block is too small to use") ;
		    continue ;
		}
		cur_screen_x = screen_x ;
		cur_screen_y = screen_y ;
		screen_to_utm ( cur_screen_x, cur_screen_y, &ux1, &uy1) ;
		break;

	    case 2:
		return (0);
		break;
	    case 3:
		screen_to_utm ( screen_x, screen_y, &ux2, &uy2) ;
		    goto foo;
		break;
	}

    }

foo:
    _Clear_info ();
    Write_info (2, "You are about to remove a block of lines. OK? ");
    Get_curses_text (buf);
    G_squeeze (buf);
    if (*buf != 'y' && *buf != 'Y')
	return (0);
    cnt = 0;

    N = GREATER (uy1, uy2);
    S = LESSER  (uy1, uy2);
    E = GREATER (ux1, ux2);
    W = LESSER  (ux1, ux2);
    for (i = 1 ; i <= Map->n_lines ; i++)
    {
	if (!LINE_ALIVE (&(Map->Line[i])))
	    continue;
	if (_line_in_window (&(Map->Line[i]), N, S, E, W))
	{
	    if (_line_really_in_window (Map, &(Map->Line[i]), N, S, E, W))
	    {
		_remove_line (Map, i);
		cnt++;
	    }
	}
    }
    sprintf (buf, " %d lines removed.", cnt);
    Write_info (2, buf);
    sleep (3);
    return (cnt);
}
