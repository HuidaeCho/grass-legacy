/*  %W%  %G%  */

/*  Routines to manage the graphics window contents list
 *
 * D_claim_offset_is(num)
 *     Remembers color offset as num
 *
 * D_offset_is(num)
 *     Returns saved color offset for window
 *
 * D_clear_window()
 *     Removes all information about current window
 *
 * D_add_to_list(string)
 *     Adds string to growing list of screen contents.
 *     "string" is, by convention, a command string.
 *
 * D_set_cell_name(name)
 *     sets the name of the cell file currently displayed
 *
 * D_get_cell_name(name)
 *     returns the name of the cell file currently displayed
 *
 */

#include <stdio.h>
D_set_cell_name(name)
	char *name ;
{
	int stat ;

	R_pad_delete_item("cell") ;

	return(R_pad_set_item ("cell", name)) ;
}

D_get_cell_name(name)
	char *name ;
{
	int stat ;
	char **list ;
	int count ;

	if(stat = R_pad_get_item ("cell", &list, &count))
		return(-1) ;

	strcpy(name, list[0]) ;

	R_pad_freelist (list,count) ;
	return(0) ;
}

D_add_to_list(string)
{
	return(R_pad_append_item("list", string)) ;
}

D_clear_window()
{
	R_pad_delete_item("list") ;
	R_pad_delete_item("cell") ;
	R_pad_delete_item("off") ;
	R_pad_delete_item("m_win") ;
	return 0;
}

D_claim_offset_is(num)
	int num ;
{
	int stat ;
	char buf[32] ;

	sprintf(buf,"%d",num) ;
	return(R_pad_set_item ("off", buf)) ;
}

D_offset_is(num)
	int *num ;
{
	char **list ;
	int count ;
	int stat ;

	if(stat = R_pad_get_item ("off", &list, &count))
	{
		*num = 0 ;
		return(-1) ;
	}

	if (sscanf(list[0],"%d",num) != 1)
		*num = 0 ;

	R_pad_freelist (list,count) ;
	
	return(0) ;
}
