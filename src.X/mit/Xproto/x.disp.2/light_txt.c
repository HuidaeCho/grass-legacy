#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#define ASCII_STRING
#include <X11/Xaw/AsciiText.h>

Arg args[2];
extern Display *dpy;

void light_text(widget, event, params, nparams)
Widget widget;
XEvent *event;
char **params;
int *nparams;
{
    XtSetArg(args[0], XtNbackground, pixel("yellow"));
    XtSetArg(args[1], XtNforeground, pixel("blue"));
    XtSetValues(widget, args, 2);
    printf("\n light");
}

void unlight_text(widget, event, params, nparams)
Widget widget;
XEvent *event;
char **params;
int *nparams;
{
    XtSetArg(args[0], XtNbackground, pixel("blue"));
    XtSetArg(args[1], XtNforeground, pixel("yellow"));
    XtSetValues(widget, args, 2);
    printf("\n unlight");
}
