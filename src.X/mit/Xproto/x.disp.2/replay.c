#include "gis.h"
#include <X11/Intrinsic.h>
extern char cell_displayed[25], *vector_files[5];
extern Window display_win;
extern int n_vects;
extern unsigned disp_width, disp_height;
extern double xl, xr, yt, yb;
extern struct Cell_head window;
extern Display *dpy;

void replay(widget, event, params, nparams)
Widget widget;
XEvent *event;
char **params;
int *nparams;
{
    char buf[100];
    register k = 0;
    int x, y;
	unsigned border_width, drawable_depth;
    Drawable drawable_root;

    printf("\n REPLAY ");
    XClearWindow(XtDisplay(widget), display_win);
    XFlush(XtDisplay(widget));
    if (cell_displayed[0] != '\0') {
        sprintf(buf, "Xcell %s", cell_displayed);
        system(buf);
    }
    while (k < n_vects) {
        sprintf(buf, "Xvect %s", vector_files[k]);
        system(buf);
        k++;
    }
    printf("REPLAY 1");
    /* calc window bounds inside the display win    */
    XGetGeometry(dpy, display_win, &drawable_root, &x, &y, &disp_width,
        &disp_height, &border_width, &drawable_depth);

    XD_get_screen_bounds(&xl, &xr, &yt, &yb, window.north, window.south,
        window.west, window.east, disp_width, disp_height);
}
