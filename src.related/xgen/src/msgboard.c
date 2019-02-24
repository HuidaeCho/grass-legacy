/**********************************************************************
   msgboard.c   - create a paned commandboard shell (msgboard is an artifact)
 *********************************************************************/
/*******************************************************************************
Xgen was developed by Kurt Buehler, while at the Center for Advanced Decision
Support for Water and Environmental Systems (CADSWES), University of Colorado
at Boulder and at the Indiana Water Resources Research Center (IWRRC),
Purdue University for the U.S. Army Construction Engineering Research
Laboratory in support of the Geographical Resources Analysis Support
System (GRASS) software. The example scripts were developed by Ms. Christine
Poulsen of USA-CERL, much thanks goes to her for her work.

Permission to use, copy, modify and distribute without charge this software,
documentation, etc. is granted, provided that this comment is retained,
and that the names of Kurt Buehler, Christine Poulsen, CADSWES, IWRRC,
the University of Colorado at Boulder, Purdue University, or USA-CERL are not
used in advertising or publicity pertaining to distribution of the software
without specific, written prior permission.

The author disclaims all warranties with regard to this software, including
all implied warranties of merchantability and fitness, in no event shall
the author be liable for any special, indirect or consequential damages or
any damages whatsoever resulting from loss of use, data or profits,
whether in an action of contract, negligence or other tortious action,
arising out of or in connection with the use or performance of this
software.
*******************************************************************************/

/***************************************************************
 * This routine takes a messageboard description and creates it.
 **************************************************************/
#include "xgen.h"
#include <X11/StringDefs.h>

void
CreateMessageBoard(s) 
    Shell *s;
{
    Resource *resource;        /* pointer to the current resource                 */
    Widget main_window;        /* the MainWindow widget                         */
    Widget menu_bar;        /* the Menubar widget                             */
    Widget help;            /* the Help widget                                 */
    Widget frame;            /* the Frame widget                             */
    Widget row_col;            /* the RowColumn widget                        */
    int n;                    /* the Arg counter                                 */
    WidgetClass widgetClass;/* override or transient                         */
    int popupWidth,
        popupHeight;
    int dpyWidth = DisplayWidth(xgenGD.display,xgenGD.screen),
        dpyHeight = DisplayHeight(xgenGD.display,xgenGD.screen);

    n = 0;
    SetGlobalArgs(&n,NOFONTS);
    SetShellColorArgs(s,&n);
    if ( (resource = IndexResource((char *)s,SHELL,"dx")) != NULL ) {
        int x;

        if ( resource->variable ) 
            ExpandVariable(resource,(char *)s,SHELL);
        if ( !IsPercent(resource->val.dval)) {
            sprintf(errorbuf, "dx value in shell [%s] out of range, ",s->name);
            sprintf(errorbuf,"%sit must be in the range [0.0,100.0]",
                errorbuf);
            XgenWarning("create paned commandboard",errorbuf);
            resource->val.dval = 0.0;
        }
        x = (int)((resource->val.dval/100.0) * 
            (double)dpyWidth);
        XtSetArg(args[n],XmNx,x); n++;
    }
    if ( (resource = IndexResource((char *)s,SHELL,"dy")) != NULL ) {
        int y;

        if ( resource->variable ) 
            ExpandVariable(resource,(char *)s,SHELL);
        if ( !IsPercent(resource->val.dval)) {
            sprintf(errorbuf, "dy value in shell [%s] out of range, ",s->name);
            sprintf(errorbuf,"%sit must be in the range [0.0,100.0]",
                errorbuf);
            XgenWarning("create paned commandboard",errorbuf);
            resource->val.ival = 0.0;
        }
        y = (int)((resource->val.dval/100.0) *
            (double)dpyHeight);
        XtSetArg(args[n],XmNy,y); n++;
    }
    if ( (resource = IndexResource((char *)s,SHELL,"width")) != NULL ) {
        if ( resource->variable ) 
            ExpandVariable(resource,(char *)s,SHELL);
        XtSetArg(args[n],XmNwidth,resource->val.ival); n++;
    }
    if ( (resource = IndexResource((char *)s,SHELL,"height")) != NULL ) {
        if ( resource->variable ) 
            ExpandVariable(resource,(char *)s,SHELL);
        XtSetArg(args[n],XmNheight,resource->val.ival); n++;
    }
    widgetClass = transientShellWidgetClass;
    if ( (resource = IndexResource((char *)s,SHELL,"override")) != NULL ) {
         widgetClass = overrideShellWidgetClass;
    }
    XtSetArg(args[n],XmNallowShellResize,True); n++;
    if ( (resource = IndexResource((char *)s,SHELL,"titlestring")) != NULL ) {
        if ( resource->variable ) 
            ExpandVariable(resource,(char *)s,SHELL);
        s->widget = XtCreatePopupShell(resource->val.cval,
            widgetClass, xgenGD.applShell,args,n);
    } else
        s->widget = XtCreatePopupShell(s->name,
            widgetClass, xgenGD.applShell,args,n);


    /***************************************************************
     * create the MainWindow : this is used since it allows for a
     * MenuBar, a Frame, and a ScrolledWindow (not used for messageboards).
     * The application designer might choose to place labels, messages,
     * or pushbuttons in a messageboard.
     **************************************************************/
    n = 0;
    SetGlobalArgs(&n,NOFONTS);
    SetShellColorArgs(s,&n);
    main_window = XmCreateMainWindow(s->widget,s->name,args,n);
    XtManageChild(main_window);

    /***************************************************************
     * create the MenuBar with help button (if help is supplied)
     **************************************************************/
    menu_bar = NULL;
    if ( (resource = IndexResource((char *)s,SHELL,"help")) != NULL ) {
        n = 0;
        SetGlobalArgs(&n,NOFONTS);
        SetShellColorArgs(s,&n);
        menu_bar = XmCreateMenuBar(main_window,"menu_bar",args,n);
        XtManageChild(menu_bar);
        n = 0;
        SetGlobalArgs(&n,FONTS);
        SetShellColorArgs(s,&n);
        help = XmCreateCascadeButton(menu_bar, "Help", args,n);
        XtManageChild(help);
        /***************************************************************
          * add the help callback, passing it the help string
          **************************************************************/
        if ( resource->variable ) 
            ExpandVariable(resource,(char *)s,SHELL);
        XtAddCallback(help,XmNactivateCallback,helpCB,
                        (caddr_t)resource->val.cval);

        n = 0;
        XtSetArg(args[n],XmNmenuHelpWidget,help); n++;
        XtSetValues(menu_bar,args,n);
    }

    /***************************************************************
     * if the shell has pulldowns, then create all of them.
     **************************************************************/
    if ( ShellHasPulldown(s) ) {
        InterfaceObject *o = s->objects;

        if ( !menu_bar ) {
            n = 0;
            SetGlobalArgs(&n,NOFONTS);
            SetShellColorArgs(s,&n);
            menu_bar = XmCreateMenuBar(main_window,"menu_bar",args,n);
            XtManageChild(menu_bar);
        }

        while ( o ) {
            if ( o->type == PULLDOWN )
                CreatePulldown(o,menu_bar);
            o = o->next;
        }
    }

    /***************************************************************
     * create a frame for the objects 
     **************************************************************/
    n = 0;
    SetGlobalArgs(&n,NOFONTS);
    SetShellColorArgs(s,&n);
    frame = XmCreateFrame(main_window,"frame",args,n);
    XtManageChild(frame);

    /***************************************************************
     * create a rowcolumn widget to manage the objects 
     **************************************************************/
    n = 0;
    SetGlobalArgs(&n,NOFONTS);
    SetShellColorArgs(s,&n);
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 1); n++;
    XtSetArg(args[n], XmNisAligned, False); n++;
    row_col = XmCreateRowColumn(frame,"rc",args,n);
    XtManageChild(row_col);

    /***************************************************************
     * set main window areas 
     **************************************************************/
    XmMainWindowSetAreas(main_window,menu_bar,NULL,NULL,NULL,frame);
    XtRealizeWidget(s->widget);

    /***************************************************************
     * add row_col to tab group 
     * (enable <TAB> and arrow keys focus mechanism) 
     **************************************************************/
    XmAddTabGroup(row_col);

    /***************************************************************
     * create the objects....
     **************************************************************/
    CreateObject(s,row_col,False,NULL,True);

    n = 0;
    XtSetArg(args[n],XtNwidth,&popupWidth); n++;
    XtSetArg(args[n],XtNheight,&popupHeight); n++;
    XtGetValues(s->widget,args,n);

    n = 0;
    if ( (resource = IndexResource((char *)s,SHELL,"x")) != NULL ) {
        if ( resource->variable ) 
            ExpandVariable(resource,(char *)s,SHELL);
        if ( abs(resource->val.ival) > dpyWidth ) {
            sprintf(errorbuf, "x value in shell [%s] out of range, ",s->name);
            sprintf(errorbuf,"%sthe screen is only %d pixels wide",
                errorbuf,dpyWidth);
            XgenWarning("create paned commandboard",errorbuf);
            resource->val.ival = 0;
        }
        if ( resource->val.ival < 0 ) {
            XtSetArg(args[n],XmNx,dpyWidth-popupWidth+resource->val.ival); n++;
        } else {
            XtSetArg(args[n],XmNx,resource->val.ival); n++;
        }
    }
    if ( (resource = IndexResource((char *)s,SHELL,"y")) != NULL ) {
        if ( resource->variable ) 
            ExpandVariable(resource,(char *)s,SHELL);
        if ( abs(resource->val.ival) > dpyHeight ) {
            sprintf(errorbuf, "y value in shell [%s] out of range, ",s->name);
            sprintf(errorbuf,"%sthe screen is only %d pixels tall",
                errorbuf,dpyHeight);
            XgenWarning("create paned commandboard",errorbuf);
            resource->val.ival = 0;
        }
        if ( resource->val.ival < 0 ) {
            XtSetArg(args[n],XmNx,dpyHeight-popupHeight+resource->val.ival);n++;
        } else {
            XtSetArg(args[n],XmNy,resource->val.ival); n++;
        }
    }
    XtSetValues(s->widget,args,n);
}

