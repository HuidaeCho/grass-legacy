#include <stdio.h>
#include "xgrass_lib.h"
#include "Interact.h"
#include "gis.h"
#include "dig_head.h"
#include "digit.h"

static Widget inter, fields[12], infopop;

Widget
make_info_pop (parent, namelabel)

    Widget parent, namelabel;
{
    Widget labels[12], explain, form, rc1, rc2;
    int i, n, x, y;
    Arg wargs[10];
    Pixel fg, bg;
    int q = 0;
    XmString str;

    static char *labelstring[] = {
	"Your organization",
	"Today's date (mon, yr)",
	"Your name",
	"Map's name",
	"Map's date",
	"Map's scale        1 :",
	"Other information",
	"Zone",
	"West edge of area",
	"South edge of area",
	"East edge of area",
	"North edge of area"
	};
    static int len[] = { 
	29, 19, 19, 40, 10, 9, 
	58, 5, 14, 14, 14, 14};

    n = 0;
    XtSetArg (wargs[n], XtNx, Winx); n++;
    XtSetArg (wargs[n], XtNy, Winy); n++;
    XtSetArg (wargs[n], XmNsaveUnder, True); n++;
    infopop = XtCreatePopupShell ("new file", transientShellWidgetClass,
						    parent, wargs, n);
    n = 0;
    XtSetArg (wargs[n], XmNautoUnmanage, TRUE); n++;
    inter = XgCreateInteractor (infopop, "inter", wargs, n);

    form = XtCreateManagedWidget ("form", xmFormWidgetClass,
                                                     inter, NULL, 0);
    str = XmStringCreateSimple ("Enter map information. Scale must be non-zero"); 
    explain = XtVaCreateManagedWidget ("explain", xmLabelWidgetClass, form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
    	    XmNlabelString, str, 
	    XmNrightAttachment, XmATTACH_FORM,
	    NULL);
    XtFree (str);
    rc1 = XtVaCreateManagedWidget ("rc1", xmRowColumnWidgetClass, form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, explain,
	    XmNbottomAttachment, XmATTACH_FORM,
	    NULL);
    rc2 = XtVaCreateManagedWidget ("rc2", xmRowColumnWidgetClass, form,
	    XmNleftAttachment, XmATTACH_WIDGET,
	    XmNleftWidget, rc1,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, explain,
	    XmNbottomAttachment, XmATTACH_FORM,
	    NULL);

    for ( i = 0; i < 12; i++)
    {
	n = 0;
        XtSetArg (wargs[n], XmNmaxLength, len[i]); n++;
        fields[i] = XtCreateManagedWidget ("", xmTextFieldWidgetClass,
                                                     rc2, wargs, n);
	if (i != 11)
            XtAddCallback(fields[i], XmNactivateCallback, XmProcessTraversal, 
					      XmTRAVERSE_NEXT_TAB_GROUP); 
        else
	    XtAddCallback(fields[i], XmNactivateCallback, gototop, NULL); 
	
	XtAddCallback(fields[i], XmNvalueChangedCallback, changes, NULL); 
        labels[i] = XtVaCreateManagedWidget (labelstring[i],
				xmLabelGadgetClass, rc1, 
				XmNmarginTop, 4,
				XmNmarginBottom, 4,
				XmNmarginHeight, 5,
				NULL);
    }
    XtAddCallback(inter, XmNokCallback, downcb, infopop); 
    XtAddCallback(inter, XmNokCallback, make_menus_active, True); 
    XtAddCallback(inter, XmNokCallback, set_map_name, namelabel); 
    XtAddCallback(inter, XmNokCallback, init_head, NULL); 

    XtManageChild (inter);
    XtUnmanageChild(XgInteractorGetChild(inter, XmINTERACT_HELP_BUTTON));
    XtUnmanageChild(XgInteractorGetChild(inter, XmINTERACT_CANCEL_BUTTON));
    
    return infopop;
}

void
changes ()
{
   /*write_out(0);*/
   Changes_Made = 1;
}
void
gototop (w, client_data, call_data)
    Widget w;
    caddr_t *client_data;
    caddr_t *call_data;
{
    int i;
    for (i=0; i < 11; i++)
    XmProcessTraversal(w, XmTRAVERSE_PREV_TAB_GROUP); 
}

void 
put_head_info (have_old)
    int have_old;
{
    char buf[100];
    struct dig_head *dhead;
    struct Cell_head window;

    dhead = &(CM->head);

    if (!have_old)
    {
        G_get_default_window (&window);
        /* it exits on error ... */

        dhead->W = window.west;
        dhead->S = window.south;
        dhead->N = window.north;
        dhead->E = window.east;
#ifdef NO_PORTABLE      /* added Aug 22, 1991  -dpg */
        dhead->portable = 0;
#else
        dhead->portable = 1;
#endif
    }


    XmTextSetString (fields[0], dhead->organization);
    XmTextSetString (fields[1], dhead->date);
    XmTextSetString (fields[2], dhead->your_name);
    XmTextSetString (fields[3], dhead->map_name);
    XmTextSetString (fields[4], dhead->source_date);
    if (dhead->orig_scale)
    {
        sprintf (buf, "%d", dhead->orig_scale);
        XmTextSetString (fields[5], buf);
    }
    XmTextSetString (fields[6], dhead->line_3);
    
    if (dhead->plani_zone)
    {
        sprintf (buf, "%d", dhead->plani_zone);
        XmTextSetString (fields[7], buf);
    } 
    sprintf (buf, "%10.2lf", dhead->W);
    XmTextSetString (fields[8], buf);
    sprintf (buf, "%10.2lf", dhead->S);
    XmTextSetString (fields[9], buf);
    sprintf (buf, "%10.2lf", dhead->E);
    XmTextSetString (fields[10],buf); 
    sprintf (buf, "%10.2lf", dhead->N);
    XmTextSetString (fields[11], buf);

 }

void
get_head_info ()
{
    char *buf;
    struct dig_head *dhead;

    dhead = &(CM->head);


    buf = XmTextGetString (fields[0]);
    strcpy(dhead->organization, buf);
    XtFree (buf);

    buf = XmTextGetString (fields[1]);
    strcpy(dhead->date, buf);
    XtFree (buf);
    
    buf = XmTextGetString (fields[2]); 
    strcpy(dhead->your_name, buf);
    XtFree (buf);
    
    buf = XmTextGetString (fields[3]);
    strcpy(dhead->map_name, buf);
    XtFree (buf);
    
    buf = XmTextGetString (fields[4]);
    strcpy(dhead->source_date, buf);
    XtFree (buf);

    buf = XmTextGetString (fields[5]);
    sscanf (buf, "%d", &(dhead->orig_scale));
    XtFree (buf);

    buf = XmTextGetString (fields[6]);
    strcpy(dhead->line_3, buf);
    XtFree (buf);
    
    buf = XmTextGetString (fields[7]);
    sscanf (buf, "%d", &(dhead->plani_zone));
    XtFree (buf);
    
    buf = XmTextGetString (fields[8]);
    sscanf (buf, "%lf", &(dhead->W));
    XtFree (buf);
    
    buf = XmTextGetString (fields[9]);
    sscanf (buf, "%lf", &(dhead->S));
    XtFree (buf);
    
    buf = XmTextGetString (fields[10]); 
    sscanf (buf, "%lf", &(dhead->E));
    XtFree (buf);
    
    buf = XmTextGetString (fields[11]);
    sscanf (buf, "%lf", &(dhead->N));
    XtFree (buf);

}

void 
close_infopop()
{
     /* called after first time when new map is opened */
    /* make scale field uneditable so they can't change it again */
    XtVaSetValues (fields[5], XmNeditable, False, 
			       XmNcursorPosition, 0,NULL); 
    XtAddCallback(fields[5], XmNmotionVerifyCallback, message_callback, 
	    "Scale cannot be changed during editing session.");  
   
   /* remove callback to fill info fields from head structure
      and add one that saves changes made to head structure */

    XtRemoveCallback(inter, XmNokCallback, init_head, NULL); 
    XtAddCallback(inter, XmNokCallback, get_head_info, NULL);  
}
   
void
showintro ()
{
   if (!XtIsManaged (infopop))
      XtManageChild ( infopop);
}

void
set_map_name (w, namelabel)
    Widget w, namelabel;
{
    char buf[100];
    XmString str;
    strcpy(buf, "Map name:   ");
    strcat(buf, N_name); 
    str = XmStringCreateSimple (buf); 
    XtVaSetValues (namelabel, XmNlabelString, str, NULL);
    XtFree (str);
}
