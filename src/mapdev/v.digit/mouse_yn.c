/*  @(#)mouse_yn.c	1.1  6/26/87  */
#include <stdio.h>
#include "digit.h"
#include "local_proto.h"
#include "raster.h"
#include "dig_curses.h"

int mouse_yes_no (char *header)
{
	int button ;
	int	screen_x, screen_y ;

	screen_x = screen_y = 1;

	_Clear_base () ;
	Write_base(10, header) ;
	Write_base(12, "    Buttons:") ;
	Write_base(13, "       Left:   yes") ;
	Write_base(14, "       Middle: no") ;
	Write_base(15, "       Right:  yes") ;


	R_get_location_with_pointer ( &screen_x, &screen_y, &button) ;

	return (!(button == 2)) ;
}

/* this is for node_lines () */
int mouse_next_prev (char *header)
{
	int button ;
	int	screen_x, screen_y ;

	_Clear_base () ;
	Write_base(10, header) ;
	Write_base(12, "    Buttons:") ;
	Write_base(13, "       Left:   Previous line") ;
	Write_base(14, "       Middle: Quit") ;
	Write_base(15, "       Right:  Next line") ;

	R_get_location_with_pointer ( &screen_x, &screen_y, &button) ;

	return(button) ;
}

int mouse_yes_no_zoom (char *header,
		unsigned char type, struct line_pnts *Xpoints)
{
	char buf[100];
	int button ;
	int	screen_x, screen_y ;

	screen_x = screen_y = 1;

	sprintf(buf, "       Middle: Zoom%s",
			(type == LINE || type == AREA ? "/Continue" : "")) ;
	_Clear_base () ;
	Write_base(10, header) ;
	Write_base(12, "    Buttons:") ;
	Write_base(13, "       Left:   yes") ;
	Write_base(14, buf) ;
	Write_base(15, "       Right:  no") ;


	R_get_location_with_pointer ( &screen_x, &screen_y, &button) ;

	if(button == 2)
		zoom_window (type, Xpoints);

	return (button) ;
}

