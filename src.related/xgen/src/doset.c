/**********************************************************************
   doset.c      - perform a set command
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

DoSet(ptr,resource,type,text)
    caddr_t ptr;
    Resource *resource;
    int type;
	char *text;
{
	char buf[1021024];
	char *savedResourceString;
	Boolean objectExpanded = False;
	char *envPtr;

    if ( !strcmp(resource->name,"set")) {

    /***************************************************************
     * value will be in x=y form, check for x=buttonname for dynamic
     * assignment of the object->name to the variable.
     * Also, expand the string if neccessary.
     **************************************************************/
        
		if ( resource->variable ) 
			ExpandVariable(resource,(char *)ptr,type);
		if ( resource->val.cval == NULL ) {
			XgenWarning("do set command","value for set is empty");
			ExpandError(resource,ptr,type);
			fprintf(stderr,"\tsetting variable to nothing\n\n");
			resource->val.cval = (char *)XtMalloc(strlen(resource->varValue)+2);
			strcpy(resource->val.cval,resource->varValue);
			strcat(resource->val.cval," ");
		}
		if ( rindex(resource->val.cval,'[')) {
		    savedResourceString = XtMalloc(strlen(resource->val.cval) + 1);
		    strcpy(savedResourceString,resource->val.cval);
		    ExpandObjectValues(resource,ptr,type);
		    objectExpanded = True;
		}
        if ( !strncmp(rindex(resource->val.cval,'=')+1,"!",1)) {
            FILE *fp;

            if ( NULL == (fp = popen(rindex(resource->val.cval,'=')+2,"r"))) {

                sprintf(errorbuf,
                    "<%s> popen(command) failed", resource->val.cval);
                XgenWarning("set procedure",errorbuf);
            } else {
            /**************************************************
             * get the first 1024 chars, truncate if longer.
             *************************************************/
                fgets(buf,1024,fp);
                fclose(fp);
                pclose(fp);
                ReplaceButtonName(resource->val.cval,buf,False);
                envPtr = XtMalloc(strlen(resource->val.cval)+1);
                strcpy(envPtr,resource->val.cval);
                putenv(envPtr);
            }
        } else {
			Boolean replaced = False;
            if (!strcmp(rindex(resource->val.cval,'=')+1,"buttonname")) {
				replaced = True;
                ReplaceButtonName(resource->val.cval,text,True);
			}
            envPtr = XtMalloc(strlen(resource->val.cval)+1);
            strcpy(envPtr,resource->val.cval);
            putenv(envPtr);
			if ( replaced ) 
                ReplaceButtonName(resource->val.cval,"buttonname",True);
        }
		if ( objectExpanded ) {
			resource->val.cval = (char *)XtMalloc(strlen(savedResourceString) + 1);
			strcpy(resource->val.cval,savedResourceString);
			XtFree(savedResourceString);
			objectExpanded = False;
		}
    } else {
		fprintf(stderr, "What kinda programmer would do this ? \n");
		exit(0);
	}
}
