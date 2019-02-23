#define X(x)	fprintf(stderr,"At x\n")
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "options.h"
#include "gis.h"
#include "driver.h"
#include <stdio.h>

#define CHUNK   128

static int coors_allocated = 0;
static int *xarray;
static int *yarray;
char *falloc();
char *frealloc();
extern struct Cell_head window;
Display *dpy;
Window win;
Colormap colormap;
int scrn;
GC  gc;

prepare()
{
    double XD_u_to_d_col(), XD_u_to_d_row();
    char *XD_get_font_name();
    char fontname[64];
    XRectangle clip_rect[1];

    Window root_return;
    int x, y;
	unsigned window_width, window_height, border_width, depth;
    XWindowAttributes win_atts;
	unsigned long colr;


    /* Set the display to be the default display */
    if (!(dpy = XOpenDisplay(NULL))) {
        fprintf(stderr, " can't open display\n");
        return (-1);
    }
    scrn = DefaultScreen(dpy);
    win = XD_get_cur_window(dpy, scrn);
    gc = XCreateGC(dpy, win, 0, None);

    XGetGeometry(dpy, win, &root_return, &x, &y, &window_width,
        &window_height, &border_width, &depth);

    if (XD_do_conversions(&window,
            window_width, window_height))
        G_fatal_error("Error in calculating conversions");

    clip_rect[0].x = (short)XD_u_to_d_col(window.west);
    clip_rect[0].y = (short)XD_u_to_d_row(window.north);
    clip_rect[0].width = (unsigned short)
        (XD_u_to_d_col(window.east) -
        XD_u_to_d_col(window.west));
    clip_rect[0].height = (unsigned short)
        (XD_u_to_d_row(window.south) -
        XD_u_to_d_row(window.north));

    XSetClipRectangles(dpy, gc, 0, 0,
        clip_rect, 1, Unsorted);

    XGetWindowAttributes(dpy, win, &win_atts);
    colormap = win_atts.colormap;

	do_color("gray") ;
    Move_abs( (int) XD_u_to_d_col((window.east + window.west) / 2.),
        (int) XD_u_to_d_row((window.north + window.south) / 2.));

	set_Font("romand") ;

}                      /* end of function prepare */



set_text_size()
{
    double x, y;
    double XD_u_to_d_col(), XD_u_to_d_row();

    x = XD_u_to_d_col((double) 0) - XD_u_to_d_col(hsize);
    y = XD_u_to_d_row((double) 0) - XD_u_to_d_row(vsize);

    Text_size(abs((int) x), abs((int) y));
    return (0);
}

do_draw(buff)
char *buff;
{
    double x, y;
    double XD_u_to_d_col(), XD_u_to_d_row();

    if (2 != sscanf(buff, "%*s %lf %lf", &x, &y))
        return (-1);

    x = XD_u_to_d_col(x);
    y = XD_u_to_d_row(y);

    do_line((double) cur_x, (double) cur_y, x, y);
    cur_x = (int) x;
    cur_y = (int) y;

    return (0);
}


do_move(buff)
char *buff;
{
    double x, y;
    double XD_u_to_d_col(), XD_u_to_d_row();

    if (2 != sscanf(buff, "%*s %lf %lf", &x, &y))
        return (-1);

    cur_x = (int) XD_u_to_d_col(x);
    cur_y = (int) XD_u_to_d_row(y);
    return (0);
}

do_icon(buff)
char *buff;
{
    double x, y;
    int ix, iy;
    char type;
    int size;
    double XD_u_to_d_col(), XD_u_to_d_row();

    if (4 != sscanf(buff, "%*s %c %d %lf %lf", &type, &size, &x, &y))
        return (-1);

    ix = (int) XD_u_to_d_col(x);
    iy = (int) XD_u_to_d_row(y);

    switch (type & 0177) {
    case 'o':

        XDrawLine(dpy, win, gc,
            ix - size, iy - size, ix - size, iy + size);

        XDrawLine(dpy, win, gc,
            ix - size, iy + size, ix + size, iy + size);

        XDrawLine(dpy, win, gc,
            ix + size, iy + size, ix + size, iy - size);

        XDrawLine(dpy, win, gc,
            ix + size, iy - size, ix - size, iy - size);

        break;

    case 'x':

        XDrawLine(dpy, win, gc,
            ix - size, iy - size, ix + size, iy + size);
        XDrawLine(dpy, win, gc,
            ix - size, iy + size, ix + size, iy - size);
        break;

    case '+':
    default:

        XDrawLine(dpy, win, gc,
            ix, iy - size, ix, iy + size);
        XDrawLine(dpy, win, gc,
            ix - size, iy, ix + size, iy);

        break;
    }


    return (0);
}

do_color(buff)
char *buff;
{
    char color[64];
    unsigned long colr;

    if (sscanf(buff, "%*s %s", color) != 1)
        return(-1);
    if ((int)(colr = XD_make_colr(dpy, win, scrn, colormap, color)) < 0)
        return(-1);
    XSetForeground(dpy, gc, colr);
    return (0);
}


char *
    do_poly(buff, infile)
char *buff;
FILE *infile;
{
    int i;
    int num;
    char origcmd[64];
    double x, y;
    char *fgets();
    char *to_return;
    XPoint *points;

    double XD_u_to_d_col(), XD_u_to_d_row();

    sscanf(buff, "%s", origcmd);

    num = 0;

    for (;;) {
        if ((to_return = fgets(buff, 128, infile)) == NULL)
            break;

        if (!sscanf(buff, "%lf %lf", &x, &y))
            break;

        check_alloc(num + 1);
        xarray[num] = (int) XD_u_to_d_col(x);
        yarray[num] = (int) XD_u_to_d_row(y);

        num++;
    }

    points = (XPoint *) malloc(num * sizeof(XPoint));

    for (i = 0; i < num; i++) {
        points[i].x = (short) xarray[i];
        points[i].y = (short) yarray[i];
    }




    if (num) {
        if (!strcmp(origcmd, "polygon"))
            XFillPolygon(dpy, win,
                gc, points, num,
                Complex, CoordModeOrigin);
        else
            XDrawLines(dpy, win,
                gc, points, num,
                CoordModeOrigin);
    }
    return (to_return);
}

/*---------------------------------------------------------*/

do_size(buff)
char *buff;
{
    if (sscanf(buff, "%*s %lf %lf", &hsize, &vsize) != 2)
        return (-1);
    set_text_size();
}

do_text(buff)
char *buff;
{
    char *ptr;

	/* remove new line */
    for (ptr = buff; *ptr != 012; ptr++);
    *ptr = 0;

    ptr = buff;
    for (; *ptr != ' '; ptr++);
    for (; *ptr == ' '; ptr++);
    Text(ptr);
}

check_alloc(num)
int num;
{
    int to_alloc;

    if (num < coors_allocated)
        return;

    to_alloc = coors_allocated;
    while (num >= to_alloc)
        to_alloc += CHUNK;

    if (coors_allocated == 0) {
        xarray = (int *) falloc(to_alloc, sizeof(int));
        yarray = (int *) falloc(to_alloc, sizeof(int));
    } else {
        xarray = (int *) frealloc(
            (char *) xarray,
            to_alloc,
            sizeof(int),
            coors_allocated);
        yarray = (int *) frealloc(
            (char *) yarray,
            to_alloc,
            sizeof(int),
            coors_allocated);
    }

    coors_allocated = to_alloc;
}

do_line(x1, y1, x2, y2)
double x1, y1, x2, y2;
{
    static int first = 1;
    static double cx, cy;

    do_clip(window.south, window.north, window.west, window.east,
        &x1, &y1, &x2, &y2);
    if (x1 == x2 && (x1 == window.west || x1 == window.east))
        if (y1 == y2 && (y1 == window.north || y1 == window.south))
            return; /* Original line was completely outside
                 * window */

    if (cx != x1 || cy != y1)
        Move_abs((int) (cx = x1), (int) (cy = y1));

    Cont_abs((int) (cx = x2), (int) (cy = y2));
}

/*--------------------------------------------------*/
do_clip(s, n, w, e, x1, y1, x2, y2)
double s, n, w, e;
double *x1, *y1, *x2, *y2;
{
    double XD_u_to_d_col(), XD_u_to_d_row();
    static int first = 1;
    static double ss, sn, sw, se;

    if (first) {
        sw = XD_u_to_d_col(w);
        se = XD_u_to_d_col(e);
        sn = XD_u_to_d_row(n);
        ss = XD_u_to_d_row(s);
        first = 0;
    }
    D_clip(sn, ss, sw, se, x1, y1, x2, y2);
}

/*----------------------------------------------------*/
