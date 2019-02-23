/*  %W%  %G%  */
/*
**  Last modified by Dave Gerdes  5/1988
**	added find_line, other changes for 3.0
**  US Army Construction Engineering Research Lab
*/

#define  FIND_WIDTH 2

#include "digit.h"

static int screen_x = 1, screen_y = 1;

find_node_with_mouse ( x, y, thresh, header)
    double    *x, *y;
    double    thresh;
    char    *header;
{
    int button;
    int node_num;
    double    ux1, uy1;
    double    ux2, uy2;
    char    buffer[64];

    _Clear_base ();
    _Write_base(10, header);
    _Write_base(12, "    Buttons:");
    _Write_base(13, "       Left:   Choose node");
    _Write_base(14, "       Middle: Abort");
     Write_base(15, "       Right:  Accept chosen node");

    node_num = 0;
    while (1)
    {
	R_get_location_with_pointer ( &screen_x, &screen_y, &button);
	flush_keyboard ();

	switch (button)
	 {
	    case 1:			/*  choose this  */
		if (node_num)
		{
		    R_standard_color (dcolors[dig_node_color (CM->Node[node_num].n_lines)]);
		    Blot (&(CM->Node[node_num].x), &(CM->Node[node_num].y));
		}
		screen_to_utm(screen_x, screen_y, &ux1, &uy1);
		*x = ux1;
		*y = uy1;

		if ( (node_num = dig_which_node (CM, &ux1, &uy1, thresh))  < 0)
		    sprintf (buffer, " no node found ");
		else
		{
		    sprintf (buffer, " node#: %d", node_num);
		    R_standard_color (dcolors[CLR_HIGHLIGHT]);
		    Blot (&(CM->Node[node_num].x), &(CM->Node[node_num].y));
		}
		Write_info(1, buffer);
		break;

	    case 2:			/*  coordinates  */
		Clear_info ();
		_Clear_base ();
		return (-1);
		break;

	    case 3:			/*  quit this  */
		Clear_info ();
		_Clear_base ();
		return (node_num);
		break;
	 }

	screen_to_utm(screen_x, screen_y, &ux2, &uy2);

	sprintf(buffer," EAST:  %10.2f", ux1>ux2?ux1:ux2);
	    Write_info(3, buffer);
	sprintf(buffer," NORTH: %10.2f", uy1>uy2?uy1:uy2);
	    Write_info(4, buffer);
    }
}


find_point_with_mouse ( x, y, n_points, thresh)
    double    *x, *y;
    int    n_points;
    double    thresh;
{
    int  i;
    int  button;
    int  point_num;
    double    ux1, uy1;
    double    ux2, uy2;
    double    fabs();
    char    buffer[64];


    _Clear_base ();
    sprintf(buffer, " Number of points on line: %d", n_points);
    _Write_base(10, buffer);
    _Write_base(12, "    Buttons:");
    _Write_base(13, "       Left:   Choose point on line");
    _Write_base(14, "       Middle: Abort");
     Write_base(15, "       Right:  Accept chosen point");

    point_num = -1;

    while (1)
    {
	R_get_location_with_pointer ( &screen_x, &screen_y, &button);
	flush_keyboard ();

	switch (button)
	 {
	    case 1:			/*  choose this  */
		if (point_num > 0)
		{
		    R_standard_color (dcolors[CLR_ERASE]);
		    Blot (x+i, y+i);
		}
		screen_to_utm(screen_x, screen_y, &ux1, &uy1);

	    /*  find the point in the line  */
		for ( i=0; i < n_points; i++ )
		    if (  (fabs( *(x+i) - ux1)  <= thresh)  &&
			  (fabs( *(y+i) - uy1)  <= thresh) ) 
			    break;


		if ( i < n_points)
		{
		    sprintf (buffer, " point#: %d", i+1);
		    /*
		    ** assume a chosen line is already highlit
		    **  if Disp_lines, then make it contrast 
		    */
		    if (Disp_lines)
			R_standard_color (dcolors[CLR_1_NODE]);
		    else
			R_standard_color (dcolors[CLR_HIGHLIGHT]);
		    Blot (x+i, y+i);
		    point_num = i;
		}
		else
		{
		    sprintf (buffer, " no point found ");
		    point_num = -1;
		}

		Write_info(1, buffer);
		break;

	    case 2:			/*  coordinates  */
		Clear_info ();
		_Clear_base ();
		return (-1);
		break;

	    case 3:			/*  quit this  */
		Clear_info ();
		_Clear_base ();
		return (point_num);
		break;

	 }

	screen_to_utm(screen_x, screen_y, &ux2, &uy2);

	sprintf(buffer," EAST:  %10.2f", ux1>ux2?ux1:ux2);
	    Write_info(3, buffer);
	sprintf(buffer," NORTH: %10.2f", uy1>uy2?uy1:uy2);
	    Write_info(4, buffer);

    }
}


/* x = y = 0.0  on abort.  */
new_point_with_mouse (x, y, header)
    double    *x, *y;
    char *header;
{
    int  button;
    double    ux1, uy1;
    double    ux2, uy2;
    char    buffer[64];
    int prev;

    *x = 0.0 ;  *y = 0.0;

    _Clear_base ();
    _Write_base(10, header);
    _Write_base(12, "    Buttons:");
    _Write_base(13, "       Left:   Choose this position");
    _Write_base(14, "       Middle: Abort");
    Write_base (15, "       Right:  Accept chosen point position");


    prev = 0;
    while (1)
    {
	R_get_location_with_pointer ( &screen_x, &screen_y, &button);
	flush_keyboard ();

	switch (button)
	 {
	    case 1:			/*  choose this  */
		if (prev)
		{
		    R_standard_color (dcolors[CLR_ERASE]);
		    _Blot (x, y);
		}
		prev = 1;
		screen_to_utm(screen_x, screen_y, &ux1, &uy1);
		*x = ux1;
		*y = uy1;
		R_standard_color (dcolors[CLR_HIGHLIGHT]);
		Blot (x, y);
		Write_info (1, " Point location:");
		break;

	    case 2:			/*  abort  */
		if (prev)
		{
		    R_standard_color (dcolors[CLR_ERASE]);
		    Blot (x, y);
		}
		*x = 0.0;
		*y = 0.0;
		Clear_info ();
		_Clear_base ();
		return (0);
		break;

	    case 3:			/*  quit this  */
		Clear_info ();
		_Clear_base ();
		return (0);
		break;

	 }

	screen_to_utm(screen_x, screen_y, &ux2, &uy2);

	sprintf(buffer," EAST:  %10.2f", ux1>ux2?ux1:ux2);
	    Write_info(3, buffer);
	sprintf(buffer," NORTH: %10.2f", uy1>uy2?uy1:uy2);
	    Write_info(4, buffer);
    }
}

/* ask user to choose a line in the window with the mouse
**   if click finds a line, highlight it.
**   allow option to abort or select that line
**   on abort line is redisplayed with normal line drawing colors
**   on select, line number is returned and line is left highlighted
**
**   type_mask is line type mask to limit searches to   or -1 if all lines 
**      are to be searched
**   header  is title to give options menu
**
**   calls   Read_line ()
**     fills   extern   Gpoints  with  Xarray Yarray info.  if successful
**
**   call is a pointer to subroutine that gets called on each successful
**	choice of a line.   If call == NULL  no special action is taken
**	if call() returns (< 0)  -> find_line_ returns 0
**
**   always returns a positive line  or 0 on not found or aborted
**
*/
find_line_with_mouse (type_mask, header, call)
    char    *header;
    char type_mask;
    int (*call)();
{
    return (_find_line_with_mouse (type_mask, header, call, USE_PTR));
}

/*
**  if type_ptr != USE_PTR, then must have x, and y values
**   otherwise, they are not necessary
*/
_find_line_with_mouse (type_mask, header, call, type_ptr, x, y)
    char    *header;
    char type_mask;
    int (*call)();
    int type_ptr;
    double x, y;
{
    int button;
    int line;
    int ret;
    double    ux1, uy1;
    double    ux2, uy2;
    char    buffer[64];
    char type;
    int screen_x1, screen_y1;

    _Clear_base ();
    _Write_base(10, header);
    if (type_mask == DOT)
    {
	_Write_base(12, "    Buttons:");
	_Write_base(13, "       Left:   Choose site");
	_Write_base(14, "       Middle: Abort");
	 Write_base(15, "       Right:  Accept chosen site");
    }
    else
    {
	_Write_base(12, "    Buttons:");
	_Write_base(13, "       Left:   Choose line");
	_Write_base(14, "       Middle: Abort");
	 Write_base(15, "       Right:  Accept chosen line");
    }

    if (type_ptr != USE_PTR)
	utm_to_screen (x, y, &screen_x1, &screen_y1);

    ret = line = 0;
    while (1)
    {
	switch (type_ptr) {
	    case USE_PTR:
		R_get_location_with_pointer (&screen_x, &screen_y, &button);
		break;
	    case USE_LINE:
		R_get_location_with_line (screen_x1, screen_y1, &screen_x, &screen_y, &button);
		break;
	    case USE_BOX:
		R_get_location_with_box (screen_x1, screen_y1, &screen_x, &screen_y, &button);
		break;
	}
	flush_keyboard ();
	/* 
	** instead of just taking a point, lets take several pixels around 
	** the mouse also 
	**  determined by FIND_WIDTH.  if == 1, then use first square of
	**  pixels (3x3) around point.  if == 2, then uses a 5x5 pixel square
	**  to determine which lines to check.
	*/
	screen_to_utm (screen_x-FIND_WIDTH, screen_y-FIND_WIDTH, &ux1, &uy1); /* Top left */
	screen_to_utm (screen_x+FIND_WIDTH, screen_y+FIND_WIDTH, &ux2, &uy2); /* bottom right */


	switch (button)
	 {
	    case 1:			/*  choose this  */

		/* Hack   dpg  12/6/89 */
		screen_to_utm (screen_x, screen_y, &Point_X, &Point_Y);

		if (line)	/* replace last chosen line */
		{
		    display_line (type, &Gpoints, line, CM);
		}

		/*  changed 6/21/89 dpg
		line = dig_point_to_line (CM, ux1, uy1, type_mask);
		*/
		line = dig_point_by_line (CM, ux1, uy1, ux2, uy2, type_mask);
		if (! line)
		{
		    sprintf (buffer, " no line found ");
		    Write_info(1, buffer);
		}
		else
		{
		    type = CM->Line[line].type;
		    if(0 > dig__Read_line(&Gpoints, CM->digit, CM->Line[line].offset))
			return (0);
		    highlight_line (type, &Gpoints, line, CM);
		    sprintf (buffer, " line#: %d", line);
		    Write_info(1, buffer);
		    ret = line;
		    if (call != NULL)
			if (0 > ((*call) (CM, line)))
			    ret = 0;
		}
		break;

	    case 2:				/*  Abort this  */
		if (line)	/* replace last chosen line */
		{
		    display_line (type, &Gpoints, line, CM);
		}
		return (0);
		break;

	    case 3:				/* return line */
		Clear_info ();
		_Clear_base ();
		return (ret);
		break;
	 }
    }
}
