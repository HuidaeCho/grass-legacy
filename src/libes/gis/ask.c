/****************************************************************
* These routines prompt the user for names of GIS data files
*
* G_ask_new           (prompt, name, element, desc)
* G_ask_old           (prompt, name, element, desc)
* G_ask_any           (prompt, name, element, desc, warn)
* G_ask_in_mapset     (prompt, name, element, desc)
*
* G_ask_new_ext       (prompt, name, element, desc, option, lister)
* G_ask_old_ext       (prompt, name, element, desc, option, lister)
* G_ask_any_ext       (prompt, name, element, desc, warn, option, lister)
* G_ask_in_mapset_ext (prompt, name, element, desc, option, lister)
*
*   char *prompt      prompt to be printed. can be "" in which
*                     case an appropriate prompt will be printed.
*   char *name        buffer to hold the name input by the user
*   char *element     GIS data element - "cell", "vect", etc.
*   char *desc        a description of element.  Used for prompting
*                     and listing. Will be set to element if given as ""
*                     (eg, if element is "vect", set desc = "vector")
*   char *option      list option. a description of the option.
*                     (eg, "with utms" will prompt as follows:
*                     list -f for a list with utms)
*   int (*lister)()   subroutine to return text for -f option.
*
*
* G_ask_new() requires the user to enter the name of a file
*             which does not exist in the current mapset
*             (but which may exist in other mapsets).
*
* G_ask_old() requires the user to enter the name of a file
*             which already exists. If the user enters a simple
*             name, the mapset search list is searched. If the
*             users enter the name followed by "in xxx" mapset
*             xxx is searched.
*
* G_ask_in_mapset() requires the user to enter the name of a file
*                   which exists in the current mapset
*
* G_ask_any() accepts any legal filename. Optionally warns user
*             if the file exists in the current mapset.
*
*
* returns:
*   char *    mapset where file was found, or
*             mapset where file is to be created
*   NULL      user hit RETURN to cancel the request
*
* note:
*  These routines have a 'list' function built in.  If a list -f
*  option is also desired, create a lister() routine, and
*  use G_ask_xxx_ext(). The lister() routine will be called as
*  follows:
*
*       lister (name, mapset, buf)
*
*       char *name        name of file 
*       char *mapset      mapset to where file lives
*       char *buf         buffer to hold description.
*                         lister() should copy into buf.
*                         buf will be large (about 400 bytes)
*                         but only first 60 chars will be displayed
*
*
*  for each mapset, lister() will be called once with
*  name set to the empty string "" in order to get an title for the
*  list.  Set buf to null to suppress title, otherwise copy title
*  into buf.  The title will start above the text for the files.
*
*  then for each file in each mapset, lister() will be called
*  to obtain infomation about the file.
*
* also:
*   G_set_ask_return_msg (msg) char *msg;
*   can be used to change the hit RETURN to cancel request message
*   displayed during the ask prompting.
*
*   G_get_ask_return_msg() will return the msg.
******************************************************************/

#include "gis.h"

/*
 * OLD references any mapset
 * NEW, ANY, PRJ are for the current mapset only
 *
 * OLD means must exist in some mapset
 * NEW means must not exist in current mapset
 * ANY means just get a name. If file exists, (optionally) warn user.
 * PRJ means must exist in current mapset
 */

#define OLD 0
#define NEW 1
#define PRJ 2
#define ANY 3
#define ANY_NW 4

static char *ask_return_msg = 0 ;
static char clear_return_msg = 0 ;
static int (*no_lister)() = 0 ;
char *ask();

char *
G_ask_new (prompt, name, element, desc)
    char *prompt;
    char *name;
    char *element;
    char *desc;
{
    return ask (prompt, name, element, desc, (char *) NULL, no_lister, NEW);
}

char *
G_ask_new_ext (prompt, name, element, desc, option, lister)
    char *prompt;
    char *name;
    char *element;
    char *desc;
    char *option;
    int (*lister)();
{
    return ask (prompt, name, element, desc, option, lister, NEW);
}

char *
G_ask_old (prompt, name, element, desc)
    char *prompt;
    char *name;
    char *element;
    char *desc;
{
    return ask (prompt, name, element, desc, (char *) NULL, no_lister, OLD);
}


char *
G_ask_old_ext (prompt, name, element, desc, option, lister)
    char *prompt;
    char *name;
    char *element;
    char *desc;
    char *option;
    int (*lister)();
{
    return ask (prompt, name, element, desc, option, lister, OLD);
}

char *
G_ask_any (prompt, name, element, desc, warn)
    char *prompt;
    char *name;
    char *element;
    char *desc;
{
    return ask (prompt, name, element, desc, (char *) NULL, no_lister, warn?ANY:ANY_NW);
}

char *
G_ask_any_ext (prompt, name, element, desc, warn, option, lister)
    char *prompt;
    char *name;
    char *element;
    char *desc;
    char *option;
    int (*lister)();
{
    return ask (prompt, name, element, desc, option, lister, warn?ANY:ANY_NW);
}

char *
G_ask_in_mapset (prompt, name, element, desc)
    char *prompt;
    char *name;
    char *element;
    char *desc;
{
    return ask (prompt, name, element, desc, (char *) NULL, no_lister, PRJ);
}

char *
G_ask_in_mapset_ext (prompt, name, element, desc, option ,lister)
    char *prompt;
    char *name;
    char *element;
    char *desc;
    char *option;
    int (*lister)();
{
    return ask (prompt, name, element, desc, option, lister, PRJ);
}

G_set_ask_return_msg (msg)
    char *msg;
{
    if (ask_return_msg) free (ask_return_msg);
    ask_return_msg = G_store (msg);
    clear_return_msg = 0;
}
char *
G_get_ask_return_msg()
{
    static char *none = "to cancel request" ;

    return (ask_return_msg == NULL ? none : ask_return_msg);
}

static
char *
ask (prompt, name, element, desc, option, lister, type)

    char *prompt;
    char *name;
    char *element;
    char *desc;
    char *option;
    int (*lister)();
{
    char tmap[256];
    char tname[256];
    int  name_in_mapset;
    int  ok;
    char tprompt[100];
    char input[100];
    char *mapset;
    char *cur_mapset;

    G__check_gisinit();

    fflush (stdout);
/* RETURN msg */
    if(clear_return_msg)
    {
	free (ask_return_msg);
	ask_return_msg = 0;
    }
    clear_return_msg = ask_return_msg ? 1 : 0;

/* make sure option is valid */
    if (lister && (option == 0 || *option == 0))
	lister = 0;

/* set name to NO NAME at outset */
    *name = 0;

/*
 * if element description not given, make it the same as the
 * element name
 */
    if (desc == 0 || *desc == 0)
	    desc = element;

/*
 * if no prompt is given, build an approriate prompt
 */
    if (prompt == 0 || *prompt == 0)
    {
	switch (type)
	{
	case NEW:
	    sprintf(prompt = tprompt,"Enter a new %s file name", desc);
	    break;
	case OLD:
	case PRJ:
	    sprintf(prompt = tprompt,"Enter the name of an existing %s file", desc);
	    break;
	default:
	    sprintf(prompt = tprompt,"Enter %s file name", desc);
	    break;
	}
    }

/*
 * get the current mapset name
 */
    cur_mapset = G_mapset();

    while (1)
    {
/*
 * print the prompt and input the request
 */
	do{
	    fprintf (stderr,"\n%s\n", prompt);
	    fprintf (stderr,"Enter 'list' for a list of existing %s files\n", desc);
	    if (lister)
	    {
		fprintf (stderr,"Enter 'list -f' for ");
		if (option && *option)
		    fprintf (stderr,"a list %s", option);
		else
		    fprintf (stderr,"an extended list");
		fprintf (stderr,"\n");
	    }

	    fprintf (stderr,"Hit RETURN %s\n", G_get_ask_return_msg());
	    fprintf (stderr,"> ");
	}
	while (!G_gets(input));

	G_strip (input);
	fprintf (stderr,"<%s>\n", input);

/*
 * if the user just hit return (or blanks only)
 * return NULL
 */
	if (*input == 0)
	    return 0;

/*
 * 'list' does a list without extension. if we are looking for a new
 * file only list the current mapset. Otherwise list all mapsets
 * in the mapset search list
 *
 * 0  not a list request
 * 1  list
 * 2  list -f
 * 3  list mapset
 * 4  list -f mapset
 */
  
	switch (parselist (input, lister?1:0, tmap))
	{
	case 0:
	    break;
	case 1:
	    G_list_element (element, desc, type==OLD?"":cur_mapset, no_lister);
	    continue;
	case 2:
	    G_list_element (element, desc, type==OLD?"":cur_mapset, lister);
	    continue;
	case 3:
	    G_list_element (element, desc, tmap, no_lister);
	    continue;
	case 4:
	    G_list_element (element, desc, tmap, lister);
	    continue;
	default:
	    fprintf (stderr,"** illegal request **\n");
	    continue;
	}

	if(name_in_mapset = G__name_in_mapset (input, tname, tmap))
	    ok = G_legal_filename (tname) >= 0;
	else
	    ok = G_legal_filename (input) >= 0;
	if (!ok)
	{
	    fprintf (stderr,"\n**<%s> illegal name **\n", input);
	    continue;
	}
/*
 * now look for the file.
 *
 * new files must be simple name (ie, not name in mapset)
 * and must not exist in the current mapset
 */
	if (type != OLD)
	{
	    if (name_in_mapset)
	    {
		if(strcmp (cur_mapset, tmap) != 0)
		{
		    fprintf (stderr,"\n** %s - illegal request **\n", input);
		    continue;
		}
		strcpy (input, tname);
	    }
	    mapset = G_find_file (element, input, cur_mapset);
	    switch (type)
	    {
	    case NEW:

		if (!mapset)
		{
		    strcpy (name,input);
		    return cur_mapset;
		}
		fprintf (stderr,"\n** %s - exists, select another name **\n", input);
		break;

	    case ANY:
	    case ANY_NW:

		if (mapset && type == ANY)
		{
		    char question[200];
		    sprintf(question,
			"\n** %s exists. ok to overwrite? ", input);
		    if (!G_yes (question,0))
			break;
		}
		strcpy (name,input);
		return cur_mapset;

	    case PRJ:

		if (mapset)
		{
		    strcpy (name,input);
		    return cur_mapset;
		}
		fprintf (stderr,"\n** %s - not found **\n", input);
		break;

	    default:
		G_fatal_error ("ask: can't happen");
	    }
	}
/*
 * old names can be simple or name-in-mapset format and must
 * exist
 */
	else
	{
	    mapset = G_find_file (element, input, "");
	    if (mapset)
	    {
		if (name_in_mapset)
		    strcpy (name,tname);
		else
		    strcpy (name,input);
		return mapset;
	    }
	    fprintf (stderr,"\n** %s - not found **\n", input);
	}
    }
}

static
parselist (input, option, mapset)
    char *input;
    char *mapset;
{
    char list[30];
    char f1[30];
    char f2[30];
    char f3[30];
    int count;

    *list = *f1 = *f2 = 0;
    count = sscanf (input, "%s%s%s%s", list, f1, f2, f3);
    if (count < 1)
	return 0;
    if (strcmp (list, "list") != 0)
	return 0;

    if (count == 1) return 1;	/* list */
    if (count > 3)  return -1;	/* illegal */

    if (*f1 == '-')		/* list -f */
    {
	if (!option)
	    return -1;
	if (f1[1] == 0 || f1[1] != 'f' || f1[2] != 0)
	    return -1;
	if (count == 2)
	    return 2;
	strcpy (mapset, f2);
	    return 4;
    }
    if (count != 2)
	return -1;
    strcpy (mapset, f1);
    return 3;
}
