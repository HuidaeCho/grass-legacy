#include <stdio.h>
#include <X11/IntrinsicP.h> 
#include <X11/Intrinsic.h> 
#include <X11/CompositeP.h>
#include <X11/CommandP.h> 
#include <X11/LabelP.h> 
#include <X11/Box.h> 
#include <X11/StringDefs.h> 
#include <X11/Shell.h>
#include <X11/Viewport.h>
#include <X11/Form.h>
#include <X11/Core.h>
#include "abstractions.h"


#define DELETE 1
#define NODELETE 0
#define MAX_CELLS 100
Widget toplevel;
Widget grass_shell,
              grass_box,
	      grass_label,
              grass_cmd1,
	      grass_cmd2,
              grass_cmd3,
              grass_cmd4,
	      grass_cmd5,
              grass_form,
	      cell_shell,
              cell_form1,
              cell_label,
	      cell_viewport,
	      cell_form2,
              cell_cmd[MAX_CELLS];	

extern Widget corridor_shell;
extern Widget corridor_form1;
static XFontStruct *font;
extern void Set();
extern void process_map();
extern void input_val();
extern void copy();
extern void dialog();

XtTranslations cmd_translations;
XtTranslations cmd4_translations;
XtTranslations cmd5_translations;
XtTranslations cell_translations;
XtTranslations form_translations;
XtTranslations map_translations;
Arg box_args[8] = {
    {XtNhSpace, (XtArgVal)0},
    {XtNvSpace, (XtArgVal)0},
};

Arg cmd_args[8] = {
    {XtNborderWidth, (XtArgVal)0},
};

Arg label_args[12] = {
    {XtNborderWidth, (XtArgVal)0},
    {XtNforeground, (XtArgVal)0},
    {XtNbackground, (XtArgVal)1},
};

Arg form_args[8] = {
    {XtNdefaultDistance, (XtArgVal)0},
    {XtNborderWidth, (XtArgVal)0},
};

static void signal()
{
	printf("\n pointer moved");
	}

static void window()
{
  XtSetSensitive(grass_cmd2, False);
  XtSetSensitive(grass_cmd3, False);
  XtSetSensitive(grass_cmd4, False);
  XtSetSensitive(grass_cmd5, False);
}

extern void disp_trans();

void display(w)
Widget w;
{
void dispatche();


  XtSetSensitive(grass_cmd1, False);
  XtSetSensitive(grass_cmd3, False);
  dispatche(w);
}

static void delete()
{
    XtSetSensitive(corridor_form1, False);

  XtSetSensitive(grass_cmd1, False);
  XtSetSensitive(grass_cmd2, False);
}


static void next(w)
Widget w;
{
GC gc;
XGCValues values;
WidgetList children;
static Boolean foreground = 0;
CompositeWidget cw;

  foreground = !foreground;
  values.foreground = foreground;
  gc = XtGetGC(w,GCForeground,&values);
  cw = (CompositeWidget)w->core.parent;
  XFillRectangle(XtDisplay(toplevel), XtWindow(cw->composite.children[1]), gc, 
                 0,0,w->core.width,w->core.height);
}

static void nothing()
{
return;
}


static void popdown(w, client_data, call_data) 
  Widget w;
  caddr_t client_data,
  call_data;
{
  XtPopdown(w);
  XtSetSensitive(grass_cmd1, True);
  XtSetSensitive(grass_cmd2, True);
  XtSetSensitive(grass_cmd3, True);
}

static void popup(w)
Widget w;
{
  static i=0;
  XtArgVal x, y, width;
  static Arg args[6];
  CompositeWidget cw;
  Widget forefather;
  WidgetList popup_list;
/*  XtSetSensitive(grass_cmd1, False);
  XtSetSensitive(grass_cmd2, False);
  XtSetSensitive(grass_cmd3, False);*/

  forefather = w->core.parent->core.parent->core.parent;
/*  cw = (CompositeWidget)w->core.parent;*/
  XtSetArg(args[0], XtNx, &x);
  XtSetArg(args[1], XtNy, &y);
  XtSetArg(args[2], XtNwidth, &width);
  XtGetValues(forefather, args,3);
  /*XtSetArg(args[0], XtNx, (forefather->core.x+forefather->core.width-6));
  XtSetArg(args[1], XtNy, forefather->core.y + w->core.parent->core.y
                                        -w->core.popup_list[0]->core.height);*/
/*  XtSetArg(args[0], XtNx, x+width-6);
  XtSetArg(args[1], XtNy, y+w->core.parent->core.y 
                          -w->core.popup_list[0]->core.height/3);
  XtSetValues(w->core.popup_list[0], args, 2);*/
  w->core.popup_list[0]->core.x = x+width-6;
  w->core.popup_list[0]->core.y =  y+w->core.parent->core.y
                          -w->core.popup_list[0]->core.height/4;
  XtPopup(w->core.popup_list[0], XtGrabExclusive);
}


void create_viewport_commands(w, flag, calldata)
  Widget w;
  caddr_t flag,
          calldata;
{
  FILE *fp, *popen();
  char cell_name[100];
  int count = 0;
  char buf[200];
  static XtCallbackRec callback[3];
  XtArgVal width;
  static Arg args[2];


  cell_form2 = XtCreateManagedWidget("", formWidgetClass,
                                     cell_viewport ,form_args, 2);

  sprintf(buf, "ls /usr/iesl/gis/data/spearfish/demo/cell");

  fp = popen(buf, "r");
  
  XtSetArg(args[0], XtNwidth, &width);
/*  XtGetValues(cell_label, args, 1);*/

  XtSetArg(cmd_args[1], XtNwidth, 1000);
  callback[0].callback = dispatche;
  XtSetArg(cmd_args[3], XtNcallback, callback);
  XtSetArg(cmd_args[4], XtNvertDistance, 0);

  while(fgets(buf,40,fp))
    {

      if( count == 0)
        {
          XtSetArg( cmd_args[5], XtNfromVert, NULL);
        }
       else
        {
          XtSetArg( cmd_args[5], XtNfromVert,cell_cmd[count-1]);
        }

        sscanf(buf,"%[^\n]", cell_name);
        cell_cmd[count] =  XtCreateManagedWidget(cell_name,  
                                                   commandWidgetClass,
                                                   cell_form2, cmd_args, 6);
        XtOverrideTranslations( cell_cmd[count++], cmd_translations);

      }
  
  pclose(fp);

}

static void Set(w)
Widget w;
{
static GC gc;
static XGCValues values;
static CommandWidget cmd1, cmd2;

if(w->core.widget_class == overrideShellWidgetClass)
  w = w->core.parent->core.parent;
cmd1 =(CommandWidget)((CompositeWidget)w)->composite.children[0];
cmd2 =(CommandWidget)((CompositeWidget)w)->composite.children[1];
values.foreground = cmd1->label.foreground;
gc = XtGetGC(cmd1, GCForeground,   &values);
XFillRectangle(XtDisplay(w), XtWindow(cmd1),
               gc, 0, 0, cmd1->core.width,
                         cmd1->core.height);
XFillRectangle(XtDisplay(w), XtWindow(cmd2),
               gc, 0, 0, cmd2->core.width,
                         cmd2->core.height);

values.foreground =cmd1->core.background_pixel;
values.font = cmd1->label.font->fid;
gc = XtGetGC(cmd1, GCForeground|GCFont, &values);

XDrawString(XtDisplay(w), XtWindow(cmd1),
               gc,cmd1->label.label_x,cmd1->label.label_y,
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       