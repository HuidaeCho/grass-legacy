#include "xgen.h"

Widget
CreateSlider(object,widget)
	InterfaceObject *object;
	Widget widget;
{
	static int numsliders = 0;
	Resource *resource;
	XmString xmstring;
	char slidername[80];
	Widget sliderW;
	int n;

	numsliders++;
	sprintf(slidername,"slider%03d",numsliders);

	n = 0;
	SetGlobalArgs(&n,FONTS);
	SetObjectGeometryArgs(object,&n);
	SetObjectColorArgs(object,&n);
    SetObjectFont(object,&n);
	XtSetArg(args[n],XmNshowValue,True); n++;
	XtSetArg(args[n],XmNhighlightOnEnter,True); n++;
	XtSetArg(args[n],XmNhighlightThickness,2); n++;
	if ( NULL != ( resource = IndexResource(object,OBJECT,"maximum"))) {
		if ( resource->variable ) ExpandVariable(resource);
		XtSetArg(args[n],XmNmaximum,resource->val.ival); n++;
	}
	if ( NULL != ( resource = IndexResource(object,OBJECT,"minimum"))) {
		if ( resource->variable ) ExpandVariable(resource);
		XtSetArg(args[n],XmNminimum,resource->val.ival); n++;
	}
	if ( NULL != ( resource = IndexResource(object,OBJECT,"startvalue"))) {
		if ( resource->variable ) ExpandVariable(resource);
		XtSetArg(args[n],XmNvalue,resource->val.ival); n++;
	}
	if ( NULL != ( resource = IndexResource(object,OBJECT,"sliderwidth"))) {
		if ( resource->variable ) ExpandVariable(resource);
		XtSetArg(args[n],XmNscaleWidth,resource->val.ival); n++;
	}
	if ( NULL != ( resource = IndexResource(object,OBJECT,"sliderheight"))) {
		if ( resource->variable ) ExpandVariable(resource);
		XtSetArg(args[n],XmNscaleHeight,resource->val.ival); n++;
	}
	if ( NULL != ( resource = IndexResource(object,OBJECT,"orientation"))) {
		if ( resource->variable ) ExpandVariable(resource);
		if ( !strcmp(resource->val.cval,"vertical") ) {
			XtSetArg(args[n],XmNorientation,XmVERTICAL); n++;
			XtSetArg(args[n],XmNprocessingDirection,XmMAX_ON_TOP); n++;
		} else if ( !strcmp(resource->val.cval,"horizontal") ) {
			XtSetArg(args[n],XmNorientation,XmHORIZONTAL); n++;
			XtSetArg(args[n],XmNprocessingDirection,XmMAX_ON_RIGHT); n++;
		} else {

			sprintf(errorbuf,"invalid orientation in slider \"%s\"\n",
					object->name);
			XgenWarning("create slider",errorbuf);
		}
	} else {
		XtSetArg(args[n],XmNorientation,XmHORIZONTAL); n++;
		XtSetArg(args[n],XmNprocessingDirection,XmMAX_ON_RIGHT); n++;
	}
	if ( NULL != ( resource = IndexResource(object,OBJECT,"titlestring"))) {
		if ( resource->variable ) ExpandVariable(resource);
	    xmstring = XmStringCreateLtoR(resource->val.ival,SDC);
	} else
	    xmstring = XmStringCreateLtoR(object->name,SDC);
	XtSetArg(args[n],XmNtitleString,xmstring); n++;
	if ( NULL != ( resource = IndexResource(object,OBJECT,"decimalpoints"))) {
		if ( resource->variable ) ExpandVariable(resource);
		if ( resource->val.ival > 0 ) {
		    XtSetArg(args[n],XmNdecimalPoints,resource->val.ival); n++;
		}
	}
	sliderW = XmCreateScale(widget,slidername,args,n);
	XtManageChild(sliderW);
	return sliderW;
}
