/**********************************************************************
   clear.c      - perform a clear command
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

#include "xgen.h"

void
ClearTable(o)
	InterfaceObject *o;
{
	Resource *resource;
	int rows, columns, r, c;

	if ( NULL != (resource = IndexResource((char *)o,OBJECT,"rows"))) {
		if ( resource->variable ) 
			ExpandVariable(resource,(char *)o,OBJECT);
		rows = resource->val.ival;
	} else
		XgenFatalError("clear table","no row count");
	if ( NULL != (resource = IndexResource((char *)o,OBJECT,"columns"))) {
		if ( resource->variable ) 
			ExpandVariable(resource,(char *)o,OBJECT);
		columns = resource->val.ival;
	} else
		XgenFatalError("clear table","no column count");

	for ( r = 1; r <= rows; r ++ )
		for ( c = 1; c <= columns; c++ )
			XmTableSetValue((XmTableWidget)o->widget,r,c,"");
}

void
ClearTablePart(o,spec)
	InterfaceObject *o;
	char *spec;
{
	/* index past '(' */
	spec++;
	if ( NULL != rindex(spec,',')) {
		int r = atoi((char *)strtok(spec,",")),
			c = atoi((char *)strtok(NULL,")"));
		
		XmTableSetValue((XmTableWidget)o->widget,r,c,"");
	} else {
		int r = atoi((char *)strtok(spec,")"));
	    Resource *resource;
	    int columns, i;
    
	    if ( NULL != (resource = IndexResource((char *)o,OBJECT,"columns"))) {
		    if ( resource->variable ) 
				ExpandVariable(resource,(char *)o,OBJECT);
		    columns = resource->val.ival;
	    } else
		    XgenFatalError("clear table","no column count");

		for ( i = 0; i < columns; i++ ) {
			XmTableSetValue((XmTableWidget)o->widget,r,i+1,"");
		}
	}

}

void
ClearToggle(o)
	InterfaceObject *o;
{
	ToggleData *tdp;

	if ( NULL == (tdp = IndexToggleData(o->name))) {
		XgenWarning("clear toggle","couldn't find toggle object");
		return;
	}
	while ( tdp ) {
		if ( tdp->set ) {
			XtSetArg(args[0],XmNset,False);
			XtSetValues(tdp->widget,args,1);
			tdp->set = False;
		}
		tdp = tdp->next;
	}
}

void
ClearListObject(name)
	char *name;
{
	ListData *ldp;

	if ( NULL == (ldp = IndexListData(name))) {
		XgenWarning("clear list","couldn't find list object");
		return;
	}
	while ( ldp ) {
		if ( ldp->selected ) 
			ldp->selected = False;
		ldp = ldp->next;
	}
}

