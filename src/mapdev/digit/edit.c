/*  %W%  %G%  */
/*
**  Written by Mike Higgins 
**    Last modified by Dave Gerdes  4/1988
**  US Army Construction Engineering Research Lab
*/


#include "digit.h"
#include "head.h"
#include <stdio.h>

/*
**  Edit menu 
**
**  driver routine for Edit menu
**
*/
int
Edit()
{
    int command;		/* command user enters */
    int ret;			/* get return values from subrs */
    int Pass;			/* Holds value to return to caller */
    int chr;

    Pass = 0;
    Set_G_Mask (MG_EDIT, OFF);

    while(1) 
    {
	_Clear_info ();
	update_global_menu ();
	Write_generic_win(&M_edit);

	if ((command = get_menu_command (&M_edit, &chr)) > 0)
	{
	    command &= 0177;
	    switch(command)
	    {
	    case MEC_REMOVE:
		Clear_info ();
		remove_line (CM, LINE | AREA);
		break;
	    case MEC_BLOCK:
		remove_block (CM);
		break;
	    case MEC_RMVSIT:
		Clear_info ();
		remove_line (CM, DOT);
		break;
	    case MEC_SNAP:
		Clear_info ();
		snap_nodes(CM);
		break;
	    case MEC_DISP:
		Clear_info ();
		within_a_thresh (CM);
		break;
#ifdef FOO
	    case MEC_MARK:
		Clear_info ();
		if (Dig_Enabled)
		{
		    mark_point (CM);
		    break;
		}

		Write_info (2,"Point marker only valid with Digitizer Enabled");
		sleep (2);

		break;
#endif
	    case MEC_MOVE:
		Clear_info ();
		move_point (CM);
		break;
	    case MEC_BREAK:
		Clear_info ();
		break_line (CM);
		break;
	    case MEC_TYPE:
		Clear_info ();
		retype_line (CM);
		break;
	    case MEC_QUIT:
		goto EDIT_END;
		break;
	    default:
		break;	 /* should not get here */
	    }
	}
	else
	{
	    if ((ret = global_menu (chr, &M_edit)) > 0)
	    {
		Pass = ret;
		break;  /* return and execute new command */
	    }
	    if (ret < 0)
		BEEP;
	}
    }
EDIT_END:
    
    Set_G_Mask (MG_EDIT, ON);
    return (Pass);
}
