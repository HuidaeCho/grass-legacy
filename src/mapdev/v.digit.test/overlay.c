/*
**  Written by Dave Gerdes  5/1988
**  US Army Construction Engineering Research Lab
*/

#include <string.h>
#include <unistd.h>
#include "Vect.h"
#include "gis.h"
#include "wind.h"
#include "keyboard.h"
#include "line_pnts.h"
#include "dig_curses.h"
#include "Map_proto.h"
#include "local_proto.h"

/* 
** Overlays:    
**  ask_overlay()  ask for name of overlay map
**  display_overlay()  display chosen overlay map
*/  

static char over_name[100];
static char over_mapset[100];
static int have_overlay;

int ask_overlay (void)
{
    char buf[1024];
    char *mapset;

    mapset = G_ask_old ("", over_name, "dig", "vector");
    if (mapset == NULL)
    {
	N_overlay = "None";
	Overlay.digit_file = NULL;
	disable_overlay ();
	have_overlay = 0;
	return (0);
    }
    have_overlay = 1;
    strcpy (over_mapset, mapset);

    N_overlay = over_name;
    if (Overlay.digit_file != NULL)
	free_name_info (&Overlay);
    G__file_name (buf, "dig", over_name, mapset) ;
    Overlay.digit_file = G_store (buf);
    Overlay.name = over_name;
    Overlay.mapset = mapset;
    enable_overlay ();

    if (!Terse_On)
	Disp_overlay = G_yes (
	    "Do you want to automatically redraw overlay on re-window? ", 1);
    return (1);
}

/*
**  returns 0  or -1 if interupted by ESC 
*/
int display_overlay (void)
{
    int ret = 0;

    if (have_overlay)
    {
	Vect_set_open_level (1);
	if (1 != Vect_open_old (&Overlay, over_name, over_mapset))
	{
	    _Clear_info ();
	    BEEP;
	    Write_info (2, "Could Not open Overlay file for read\n");
	    sleep (3);
	    return (1);
	}

	ret = 0;
	flush_keyboard ();
	set_keyboard ();
	if (0 > plot_overlay (&Overlay))
	    ret = -1;	/* ESC pressed */
	unset_keyboard ();

	Vect_close (&Overlay);
    }
    else
	ret = 1;
    return ret;
}

/*
**  returns 0  or -1 if interupted by ESC 
*/
int plot_overlay (struct Map_info *overlay)
{
	int status ;
	int ret = 0;
	char buf[8];

	Vect_set_constraint_region (overlay, U_north, U_south, U_east, U_west);
	Vect_rewind (overlay);

	_Clear_info ();
	Write_info (2, "Plotting Overlay Map");
	Write_info (3, "                                  ...Press <ESC> key to stop");
	while (1)
	{
		if (key_hit (buf))
		{
		    if (*buf == ESC)
		    {
			ret = 2;
			goto end;
		    }
		}
   /* use Gpoints, since this should never happen in side another action */
		status = Vect_read_next_line (overlay, &Gpoints) ;
		if (status == -1)
		{
		    ret = status ;
		    goto end;
		}
		if (status == -2)
		{
		    ret = (0) ;
		    goto end;
		}

	        nplot_points(LINE, &Gpoints, CLR_OVERLAY, CLR_OVERLAY, 
			CLR_OVERLAY,0);
	 }
end:
	 V_flush ();
	 return (ret);
}

int disable_overlay (void)
{
    disenable_overlay (OFF);

    return 0;
}

int enable_overlay (void)
{
    disenable_overlay (ON);

    return 0;
}

int disenable_overlay (int onoff)
{
    register int i;

    for (i = 0 ; M_window.item[i].text != NULL ; i++)
	if (M_window.item[i].command == MWC_OVERLAY)
	{
	    M_window.item[i].enabled = onoff;
	    break;
	}

    return 0;
}

int free_name_info (struct Map_info *Map)
{
    if (Map->name)
	G_free (Map->name);
    if (Map->mapset)
	G_free (Map->mapset);
    if (Map->digit_file)
	G_free (Map->digit_file);

    return 0;
}
