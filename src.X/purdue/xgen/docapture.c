#include "xgen.h"
#include <Xm/SelectioB.h>
#include <Xm/ScrolledWP.h>
void saveCB();
void SaveOKCB();

static Widget saveDialog = NULL;

void
DoCaptureText(com)
	Command *com;
{
	Widget captureWidget;
	Widget main_window;
	Widget menu_bar;
	Widget dismiss;
	Widget save;
	Widget frame;
	Widget text;
	FILE *fp;
	char *string;
	struct stat statbuf;
	int length;
	int n;


	
	if ((fp = fopen(com->tmpfile, "r+")) == NULL)
		if ((fp = fopen(com->tmpfile, "r")) != NULL) {
			fprintf(stderr, "Warning: file opened read only.\n");
		} else {
			fprintf(stderr, "Sorry: couldn't get tmpfile\n"); 
			return;
		}

	/* get the legth of the string */
	if ( stat(com->tmpfile,&statbuf) == 0 )
		length = statbuf.st_size;
	else
		length = 1000000; /* arbitrary file length */

	/* read it... */
	if ( length > 0 ) {
		string = (char *)XtMalloc(length);
		fread(string, sizeof(char), length, fp);
	}

	/* close up the file */
   	if (fclose(fp) != NULL) 
		fprintf(stderr, "Warning: unable to close file.\n");	

	if ( length ) {

		n = 0;
   		SetGlobalArgs(&n,NOFONTS);
   		SetObjectColorArgs(NULL,&n);

		XtSetArg(args[n],XmNallowShellResize,True); n++;
   		captureWidget = XtCreatePopupShell(com->path,topLevelShellWidgetClass,
                                  	xgenGD.applShell,args,n);

	/***************************************************************
   	* create the MainWindow : this is used since it allows for a
   	* MenuBar, a Frame, and a ScrolledWindow (if necessary).
   	* The application designer might choose to place labels, lists,
   	* or pushbuttons in a menu.
   	**************************************************************/
  		n = 0;
  		SetGlobalArgs(&n,NOFONTS);
  		SetObjectColorArgs(NULL,&n);
		main_window = XmCreateMainWindow(captureWidget,com->path,args,n);
   		XtManageChild(main_window);
 	/***************************************************************
   	* create the MenuBar 
   	**************************************************************/
   		menu_bar = NULL;
   		n = 0;
   		SetGlobalArgs(&n,NOFONTS);
   		SetObjectColorArgs(NULL,&n);
   		menu_bar = XmCreateMenuBar(main_window,"menu_bar",args,n);
   		XtManageChild(menu_bar);
 	/***************************************************************
   	* create menu buttons 
   	**************************************************************/
		n = 0;
   		SetGlobalArgs(&n,FONTS);
   		SetObjectColorArgs(NULL,&n);
   		dismiss = XmCreateCascadeButton(menu_bar, "Dismiss", args,n);
   		XtManageChild(dismiss);
   		save = XmCreateCascadeButton(menu_bar, "Save to file", args,n);
   		XtManageChild(save);
   	/***************************************************************
   	* create a frame for the objects
   	**************************************************************/
   		n = 0;
   		SetGlobalArgs(&n,NOFONTS);
   		SetObjectColorArgs(NULL,&n);
   		frame = XmCreateFrame(main_window,"frame",args,n);
   		XtManageChild(frame);

		n = 0;
   		SetGlobalArgs(&n,NOFONTS);
   		SetObjectColorArgs(NULL,&n);
		XtSetArg (args[n], XmNrows, 24);  n++;
		XtSetArg (args[n], XmNcolumns, 80);  n++;
		XtSetArg (args[n], XmNresizeWidth, False);  n++;
		XtSetArg (args[n], XmNresizeHeight, False);  n++;
		XtSetArg (args[n], XmNscrollVertical, True);  n++;
		XtSetArg (args[n], XmNscrollHorizontal, True);  n++;
		XtSetArg (args[n], XmNeditMode, XmMULTI_LINE_EDIT);  n++;
		text = XmCreateScrolledText(frame,"text",args,n);
		XtManageChild(text);

   	    /***************************************************************
   	     * add callbacks 
   	     **************************************************************/
   		XtAddCallback(dismiss,XmNactivateCallback,dismissCB,
		  	(caddr_t)captureWidget);
   		XtAddCallback(save,XmNactivateCallback,saveCB,
		  	(caddr_t)text);

        /***************************************************************
         *  don't ask......
         **************************************************************/
		{
		    XmScrolledWindowWidget sw = (XmScrolledWindowWidget)text;
    
		    n = 0;
		    SetObjectColorArgs(NULL,&n);
		    XtSetValues(sw->core.parent,args,n);
		}

		XtPopup(captureWidget,XtGrabNone);
		XmTextSetString(text, string);
	}
}

void 
dismissCB(w,cld,cad)
    Widget w;
    caddr_t cld;
    caddr_t cad;
{
    Widget popup = (Widget)cld;
    XtDestroyWidget(popup);
}

void 
saveCB(w,cld,cad)
    Widget w;
    caddr_t cld;
    caddr_t cad;
{
	int n;
	XmString xmstring;
	Widget text = (Widget) cld;

	if ( !saveDialog ) {
	    n = 0;
	    xmstring = XmStringCreateLtoR("Save To File...",SDC);
	    XtSetArg(args[n], XmNselectionLabelString,xmstring); n++;
	    saveDialog = XmCreatePromptDialog(w,"save dialog",args,n);
	    XtManageChild(saveDialog);
	    XtAddCallback (saveDialog, XmNokCallback, SaveOKCB, (caddr_t)text);
	} else
	    XtManageChild(saveDialog);
}

void
SaveOKCB(w,cld,cad)
    Widget w;
    caddr_t cld;
    caddr_t cad;
{
	char *fileString = NULL;
	char *fileName = NULL;
	FILE *fp = NULL;
	XmSelectionBoxCallbackStruct *scb =
		 (XmSelectionBoxCallbackStruct *) cad;
	Widget text = (Widget) cld;

	/* get the file name from the dialog */
	XmStringGetLtoR(scb->value, SDC, &fileName);

	/* is NULL or empty just return so maybe the user will enter one */
	if ( !fileName || !strcmp(fileName,"") ) return;

	/* open the file, if possible */
	if ((fp = fopen(fileName, "a+")) == NULL) {
		sprintf(errorbuf,"unable to open [%s], text not saved.",fileName);
		XgenWarning("save captured text",errorbuf);
	}

	/* get the text string */
	fileString = XmTextGetString(text);

	/* write it to the file */
	fwrite(fileString, sizeof(char), strlen(fileString) + 1, fp);

	fflush(fp);
	fclose(fp);

	/* unmanage the dialog */
	XtUnmanageChild (saveDialog);
}
