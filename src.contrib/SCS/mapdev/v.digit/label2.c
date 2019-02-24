/*
**  Written by Dave Gerdes  5/1988
**  US Army Construction Engineering Research Lab
*/

#include "gis.h"
#include "digit.h"
#include "dig_head.h"
#include <stdio.h>

int tell_area_label ();
int tell_area_unlabel ();
int make_area_label ();
int tell_line_label();	/* function called by find_line_with_mouse() */

static double local_x;	/* filled by label_area  */
static double local_y;	/* used by tell_area_label */
static int local_area;
static int local_prev;
static P_AREA local_struct;


int
Label()
{
    int command;		/* command user enters */
    int ret;			/* get return values from subrs */
    int Pass;			/* Holds value to return to caller */
    int chr;
    int cat;

#ifdef SCS_MODS
    int ans=0, ier;
    char message[72];
    struct Categories cats;
#endif /* SCS_MODS */

    Pass = 0;
    Set_G_Mask (MG_LABEL, OFF);

    while(1) 
    {
	_Clear_info ();
	update_global_menu ();
	Write_generic_win(&M_label);
	Label_settings ();

	if ((command = get_menu_command (&M_label, &chr)) > 0)
	{
	    switch(command)
	    {
	    case MLC_LAREA:
#ifdef SCS_MODS 
		Clear_base ();
                ans = ask_for_name(AREA, &cats);
#endif /* SCS_MODS */
		while (1)
		{
		    Clear_base ();
		    Clear_info ();
#ifndef SCS_MODS
                    cat = ask_cat();
#else
                    if (ans) cat = ask_name(&cats);
                    else cat = ask_cat();
#endif /* SCS_MODS */
		    if (!cat) break;
		    label_area (CM, cat);
		}
		break;
	    case MLC_LLINE:
#ifdef SCS_MODS
                Clear_base ();
                ans = ask_for_name(LINE, &cats);
#endif /* SCS_MODS */

		while (1)
		{
		    Clear_base ();
		    Clear_info ();
#ifndef SCS_MODS
                    cat = ask_cat();
#else
                    if (ans) cat = ask_name(&cats);
                    else cat = ask_cat();
#endif /* SCS_MODS */

		    if (!cat) break;
		    label_lines (CM, cat);
		}
		break;
	    case MLC_LSITE:
#ifdef SCS_MODS
                Clear_base ();
                ans = ask_for_name(DOT, &cats);
#endif /* SCS_MODS */

		while (1)
		{
		    Clear_base ();
		    Clear_info ();
#ifndef SCS_MODS
                    cat = ask_cat();
#else
                    if (ans) cat = ask_name(&cats);
                    else cat = ask_cat();
#endif /* SCS_MODS */

		    if (!cat) break;
		    label_sites (CM, cat);
		}
		break;
	    case MLC_LLINES:
		Clear_base ();
		Clear_info ();
		if (!curses_yes_no_default (2, "This function will label EVERY unlabeled line. Proceed?", 0))
		    break;
		cat = ask_cat ();
		if (!cat) break;
		label_all_lines (CM, cat);
		break;
	    case MLC_ULAREA:
		unlabel_area (CM);
		break;
	    case MLC_ULLINE:
		unlabel_lines (CM);
		break;
	    case MLC_ULSITE:
		unlabel_sites (CM);
		break;
	    case MLC_SLINES:
		display_llines (CM);
		break;
	    case MLC_SAREAS:
		display_lareas (CM);
		break;
	    case MLC_MLINES:	/* label multiple lines */
#ifdef SCS_MODS
                Clear_base ();
                ans = ask_for_name(LINE, &cats);
#endif /* SCS_MODS */

		while (1)
		{
		    Clear_base ();
		    Clear_info ();
#ifndef SCS_MODS
                    cat = ask_cat();
#else
                    if (ans) cat = ask_name(&cats);
                    else cat = ask_cat();
#endif /* SCS_MODS */
		    if (!cat) break;
		    label_mlines (CM, cat);
		}
		break;
	    case MLC_UMLINES:	/* unlabel multiple lines */
		Clear_base ();
		Clear_info ();
		label_mlines (CM, 0);
		break;
	    case MLC_CONTOUR:	/* label contour lines w/ others */
		while (1)
		    if (0 == label_contour (CM, Contour_Interval))
			break;
		break;
	    case MLC_INTERV:  /* contour interval */
		{
		    char buf[100];
		    int val;

		    _Clear_info ();
		    Write_info (4, "   Enter New Contour Interval: ") ;
		    Get_curses_text (buf) ;
		    val = atoi (buf);
		    if (!val)
			break;
		    Contour_Interval = val;
		}
		break;
	    case MLC_QUIT:
		Pass = 0;
		goto LABEL_END;
		break;
	    default:
		break;	 /* should not get here */
	    }
	}
	else
	{
	    if ((ret = global_menu (chr, &M_label)) > 0)
	    {
		Pass = ret;
		break;  /* return and execute new command */
	    }
	    if (ret < 0)
		BEEP;
	}
    }
LABEL_END:
    
    Set_G_Mask (MG_LABEL, ON);
    return (Pass);
}

Label_settings ()
{
    char tmpstr[30];
    sprintf (tmpstr, "<%5d>", Contour_Interval);
    Base_string (8, 68, tmpstr);
}

ask_cat ()
{
    char buf[100];

    Write_info ( 4, "   Enter Category Number (0 to END):[0] ") ;
    Get_curses_text (buf) ;
    return (atoi (buf));
}


/* ask user to select area to label and create  new area and label */
/* returns 0 OK  or -1 no area created */
label_area  (map, cat)
    struct Map_info *map;
    int cat;
{
    int line, area, att;
    double x, y;
    int ret;

    while (1)
    {
	Clear_info ();
	
	if (Digtiz_Device == MOUSE)
	   {
 	      /* find_line_with_mouse  fills Gpoints */
	   new_point_with_mouse (&x, &y, "Select point within area:");
	   }
        else
	   if (D_cursor_buttons())
	   {
	      /* find_line_with_dig   fills Gpoints */
	   new_point_with_dig (&x, &y, "Select point within area:");
           }

	if (x == 0.0 && y == 0.0)
	    return (-1);

	/* change color so they know something happend */
	R_standard_color (dcolors[CLR_AMARK]); 
	Blot (&x, &y);
	local_x = x; local_y = y;	/* store these for tell_area_label() */

	local_prev = 0;	/* reset static flag */
	/* find_line loads global struct: Garea */
        if (Digtiz_Device == MOUSE)
              {
	      if (0>=(line = find_line_with_mouse (AREA, "Select a Boundary line:", tell_area_label)))
	          {
	          unset_dot (x, y);
	          continue;
	          }
              }     
        else    
              if (D_cursor_buttons())
              {                       
              if (0>=(line = find_line_with_dig (AREA, "Select a Boundary line:", tell_area_label)))
                  {  
                  unset_dot (x, y);
                  return (-1);      
                  }
              }

	if (make_area_label (map, line) >= 0)	/* completed an area? */
	{
           if (Digtiz_Device == MOUSE)
              {                        
              if (2 == mouse_yes_no ("Accept this area ? "))
                    return (0);                              
              ret = 1;          
              }        
           else 
              if (D_cursor_buttons())
              {                       
              if ( ! ask_driver_yes_no("Accept this area ? ") )
                        return(0) ;                             
              ret = 1;              
              }        

	  if (ret)
	  {
	    /* if this far, then an area is selected, either old or new */
	    /*  if local_area, then is old, else  Garea holds area info */
	    if (local_area)
	    {
		P_ATT *AP;
		char buf[100];

		if (!map->Area[local_area].att)
		{
/*DEBUG*/ debugf ("Label area: creating new attribute\n");
		    map->Area[local_area].att = 
			dig_new_att (map, local_x, local_y, AREA, local_area, cat);
		}
		else
		{
/*DEBUG*/ debugf ("Label area: attribute exists changing it\n");
		    AP = &(map->Att[map->Area[local_area].att]);
		    sprintf (buf, "%d", AP->cat);
		    R_standard_color (dcolors[CLR_ERASE]);
		    _Blot (&(AP->x), &(AP->y));
		    Adot (&(AP->x), &(AP->y), buf);
		    AP->cat = cat;
		    AP->x = x;
		    AP->y = y;
		    dig_update_att (map, map->Area[local_area].att);
		}

		display_area (local_area, map);
		area = local_area;
	    }
	    else
	    {
/*DEBUG*/ debugf ("Label area: new area:  ");
		area = dig_new_area (map, &Garea, 0);	/* give dummy att info*/
		if (area < 0) return (-1);		/* out of memory? */
/*DEBUG*/ debugf (" creating new attribute\n");
		att = dig_new_att (map, x, y, AREA, area, cat);	/* create new att  */
		if (att < 0)
		    return (-1);
		if (att < 0) return (-1);		/* out of memory? */
		map->Area[area].att = att;		/* stick in att info */
		display_area (area, map);
	    }
	    display_area_label (area, map);
	    Changes_Made = 1;
	  }
	  else	/* cleanup and leave */
	  {
	    display_line (map->Line[line].type, &Gpoints, line, map);
	    R_standard_color (dcolors[CLR_ERASE]);
	    Blot (&local_x, &local_y);
	    if (Disp_outline && local_area && AREA_LABELED (&(map->Area[local_area])))
		display_area (local_area, map);
	    else
		if (local_area)
		    reset_area (local_area, map);
		else
		    _reset_area (&Garea, map);
	  }
	}
	else  /* area not made */
	{
	    R_standard_color (dcolors[CLR_ERASE]);
	    Blot (&local_x, &local_y);
	}
    }
}

label_lines  (map, cat)
    struct Map_info *map;
    int cat;
{
    int line;

    while (1)
    {
	Clear_info ();
	/* find_line_with_mouse  fills Gpoints */
        if (Digtiz_Device == MOUSE)
	      {
	      if (0 >= (line = find_line_with_mouse (LINE | AREA, "Choose line:", tell_line_label)))
	          {
	              return (-1);
	          }
	      }
        else
              if (D_cursor_buttons())
              {                       
              if (0>=(line = find_line_with_dig (LINE | AREA, "Choose line:", tell_line_label)))
                  {
                  return (-1);
                  }            
              }     

	if (0 > label_line (map, line, cat, &Gpoints))
	    return (-1);
    }
}

label_sites  (map, cat)
    struct Map_info *map;
    int cat;
{
    int line;

    while (1)
    {
	Clear_info ();
	/* find_line_with_mouse  fills Gpoints */
        if (Digtiz_Device == MOUSE)               
              {
	      if (0 >= (line = find_line_with_mouse (DOT, "Choose site:", tell_line_label)))
	          {
	          return (-1);
	          }
              }
        else    
              if (D_cursor_buttons())
              {                       
              if (0>=(line = find_line_with_dig (DOT, "Choose site:", tell_line_label)))       
                  {
                  return (-1);
                  }            
              }

if (0 > label_line (map, line, cat, &Gpoints))
	    return (-1);
    }
}
    
label_line (map, line, cat, Points)
    struct Map_info *map;
    int line;
    int cat;
    struct line_pnts *Points;
{
    int att;
    double x, y;
    int line_type;
    P_ATT *Att;


    line = abs (line); /* dpg 11/89 */

    line_type = map->Line[line].type;

    /* area and line lines all get labelled as LINE */
    if (line_type == AREA)
	line_type = LINE;

    /* remove old label from screen */
    erase_line (map->Line[line].type, Points, line, map);

    get_line_center (&x, &y, Points);

    if (map->Line[line].att) /* if already exists, change it */
    {
	att = map->Line[line].att;
	Att = &(map->Att[att]);
	Att->cat = cat;
	Att->x = x;
	Att->y = y;
	Changes_Made = 1;
	dig_update_att (map, att);
    }
    else
    {
	att = dig_new_att (map, x, y, line_type, line, cat);
	if (att < 0)
	    return (-1);
	map->Line[line].att = att;
	Changes_Made = 1;
    }
    display_line (map->Line[line].type, Points, line, map);
    return (0);
}

unlabel_area  (map)
    struct Map_info *map;
{
    int line, area, att;
    double x, y;

    while (1)
    {
	Clear_info ();

        if (Digtiz_Device == MOUSE)               
              {                     
                /* find_line_with_mouse  fills Gpoints */
	      new_point_with_mouse (&x, &y, "Select point within area:");
	      }
        else    
              if (D_cursor_buttons())
              {                       
                /* find_line_with_dig  fills Gpoints */
              new_point_with_dig (&x, &y, "Select point within area:");
              }

	if (x == 0.0 && y == 0.0)
	{
	    unset_dot (x, y);
	    return (-1);
	}
	R_standard_color (dcolors[CLR_AMARK]);
	Blot (&x, &y);
	local_x = x; local_y = y;	/* store these for tell_area_label() */

        if (Digtiz_Device == MOUSE)
              {
	      if (0>=(line = find_line_with_mouse (AREA, "Select a Boundary line:", tell_area_unlabel)))
	          {
	    /* is a bug here.  if accept a line that does not make an area */
	    /* this line will stay highlit */
	    /* if they aborted w/out choosing a line, there will be no line */
	    /* lit.  the previous is the one that needs to be taken care of */
	    /* should probly be fixed in find_w_mouse */
	          unset_dot (x, y);
	          continue;
	          }
              }     
        else    
              if (D_cursor_buttons())
              {                       
              if (0>=(line = find_line_with_dig (AREA, "Select a Boundary line:", tell_area_unlabel)))
                  {    
                  unset_dot (x, y);
                  continue;         
                  }         
              }

	unset_dot (x, y);

	if (local_area && AREA_LABELED (&(map->Area[local_area])))
	{
	    P_AREA *Area;
	    P_ATT *Att;
	    char buf[50];

	    Area = &(map->Area[local_area]);
	    Changes_Made = 1;

	    Att = &(map->Att[Area->att]);	

	    /* remove cat on screen */
	    R_standard_color (dcolors[CLR_ERASE]);
	    _Blot (&(Att->x), &(Att->y));
	    sprintf (buf, "%d", Att->cat);
	    Adot (&(Att->x), &(Att->y), buf);
	    _reset_area (Area, map);

	    dig_del_att (map, Area->att);	/* delete its attribute */
	    Area->att = 0;
	    /*  this is too drastic, lets leave the area alone
	    Del_area (map, local_area);
	    */
	}
	else
	{
/*DEBUG*/ debugf ("No attribute.  no action taken\n");
	}
    }
}

unlabel_lines  (map)
    struct Map_info *map;
{
    int line;

    while (1)
    {
	Clear_info ();
	/* find_line_with_mouse  fills Gpoints */
        if (Digtiz_Device == MOUSE)               
              {
	      if (0 >= (line = find_line_with_mouse (LINE | AREA, "Choose labeled line:", tell_line_label)))
	          {
	          return (-1);
	          }
              }     
        else    
              if (D_cursor_buttons())
              {                       
              if (0>=(line = find_line_with_dig (LINE | AREA, "Choose labeled line:", tell_line_label)))
                  {      
                  return (-1);
                  }            
              }

	if (map->Line[line].att)
	{
	    erase_line (map->Line[line].type, &Gpoints, line, map);
	    dig_del_att (map, map->Line[line].att);
	    map->Line[line].att = 0;
	    Changes_Made = 1;
	    display_line (map->Line[line].type, &Gpoints, line, map);
	}
    }
}

unlabel_sites (map)
    struct Map_info *map;
{
    int line;

    while (1)
    {
	Clear_info ();
	/* find_line_with_mouse  fills Gpoints */
        if (Digtiz_Device == MOUSE)               
              {
	      if (0 >= (line = find_line_with_mouse (DOT, "Choose labeled Site:", tell_line_label)))
		  {
	          return (-1);
		  }
              }     
        else    
              if (D_cursor_buttons())
              {                       
              if (0>=(line = find_line_with_dig (DOT, "Choose labeled Site:", tell_line_label)))
                  {
                  return (-1);
                  }            
              }

	if (map->Line[line].att)
	{
	    dig_del_att (map, map->Line[line].att);
	    map->Line[line].att = 0;
	    Changes_Made = 1;
	    display_line (map->Line[line].type, &Gpoints, line, map);
	}
    }
}

tell_line_label (map, line)
    struct Map_info *map;
    int line;
{
    char buf[200];

    if (map->Line[line].att)
    {
        if (map->Line[line].type == DOT)                                     
             sprintf (buf, "Site is Category %d", 
				 map->Att[map->Line[line].att].cat);
	else
             sprintf (buf, "Line is Category %d", 
				 map->Att[map->Line[line].att].cat);
	Write_info (2, buf);
    }
    else
    {    
        if (map->Line[line].type == DOT)                                    
              Write_info(2, "Site is Not labeled");
        else
	      Write_info(2, "Line is Not labeled");
    }
    return (0);
}


/* Document this !!   return values?? */
/* called by find_line_w_mouse */
make_area_label (map, line)
    struct Map_info *map;
    int line;
{
    int area;
    char buf[200];

    if (local_prev)
	if (local_area)
	    display_area (local_area, map);
	else
	    _display_area (&Garea, map);
	
    if ((area = check_area (map, line, local_x, local_y)) > 0)
    {
	local_prev = 1;
	local_area = area;

	if (Auto_Window && area_outside_window (&(map->Area[local_area])))
	{
	    P_AREA *Area;

	    Area = &(map->Area[local_area]);
	    expand_window (Area->N, Area->S, Area->E, Area->W);
	}

	highlight_area (area, map);
	if (map->Area[area].att)
	    sprintf (buf, "Area is Category %d", map->Att[map->Area[area].att].cat);
	else
	    sprintf (buf, "Area is not labeled");
	Write_info(2, buf);
	return (1);
    }
    else
    {
	local_area = 0;
	if (0 >= build_area (map, local_x, local_y, line, &Garea))	/* create new area */
	{
	    BEEP;
	    Write_info (2, "Could not create area.");
	    sleep (2);
	    local_prev = 0;
	    display_line (AREA, &Gpoints, line, map);	/* undo highlight */
	    return (-1);	/* NO Current Area */
	}
	else
	{
	    if (Auto_Window && area_outside_window (&Garea))
		expand_window (Garea.N, Garea.S, Garea.E, Garea.W);
	    _highlight_area (&Garea, map);
	}
    }

    return (0);
}

tell_area_unlabel (map, line)
    struct Map_info *map;
    int line;
{
    int area;
    char buf[1024];

    Clear_info ();
    local_area = 0;
    if ((area = check_area (map, line, local_x, local_y)) > 0 &&
	map->Area[area].att)
    {
	sprintf (buf, "Area is labeled as category %d", 
	    map->Att[map->Area[area].att].cat);
	Write_info (1, buf);
	local_area = area;
	return (0);
    }
    else
    {
	Write_info (1, "Area is not labeled");
	local_area = 0;
	return (-1);
    }
}


tell_area_label (map, line)
    struct Map_info *map;
    int line;
{
    int area;
    char buf[1024];

    Clear_info ();
    local_area = 0;
    if ((area = check_area (map, line, local_x, local_y)) > 0 &&
	map->Area[area].att)
    {
	sprintf (buf, "Area is labeled as category %d", 
	    map->Att[map->Area[area].att].cat);
	Write_info (1, buf);
	local_area = area;
    }
    else
    {
	Write_info (1, "Area is not labeled");
	local_area = 0;
    }
    return (0);
}

/* given x, y  and line number,  check if x, y is within a predefined 
**  area bounded by  line
*/
check_area (map, line, x, y)
    struct Map_info *map;
    int line;
    double x, y;
{
    line = ABS (line);
/*DEBUG*/ debugf ("Check_area: line %d R %d L %d (%lf, %lf)\n", line, map->Line[line].right, map->Line[line].left, x, y);
    if (map->Line[line].right > 0)   /* ISLE */
	if (dig_point_in_area (map, x, y, &(map->Area[map->Line[line].right])) > 0.)
	{
/*DEBUG*/ debugf ("Check_area:  POINT IN AREA(right)  returned TRUE\n");
	    return (map->Line[line].right);
	}
    if (map->Line[line].left > 0)   /* ISLE */
	if (dig_point_in_area (map, x, y, &(map->Area[map->Line[line].left])) > 0.)
	{
/*DEBUG*/ debugf ("Check_area:  POINT IN AREA(left) returned TRUE\n");
	    return (map->Line[line].left);
	}
/*DEBUG*/ debugf ("Check_area:  POINT IN AREA returned FALSE\n");
    return (0);
}

/* need a point to place label on line.
**  for a line w/ > 2 points just pick the middle point
**  for a line w/ only 2 points we dont want to label the node (> 1 line)
**  so we calculate a mid-point
*/
#ifdef OLD_LINE_CENTER
get_line_center (x, y, Points)
    double *x, *y;
    struct line_pnts *Points;
{
    int which_coor;

    if (Points->n_points < 1)
	return (-1);

    if (Points->n_points < 2)
    {
	*x = Points->x[0];
	*y = Points->y[0];
	return (0);
    }
    if (Points->n_points > 2)
    {
	which_coor = Points->n_points >> 1;
	*x = Points->x[which_coor];
	*y = Points->y[which_coor];
	return (0);
    }

    /* calculate the middle of a two points line */
    *x = (Points->x[0] + Points->x[1]) / 2.0;
    *y = (Points->y[0] + Points->y[1]) / 2.0;
    return (0);
}
#else  /* new improved line_center */

/* 
** find a fast approximate point on a chain to place a label
**  uses a city block distance approximation to choose the point
**  In other words use distance x+y to approximate len of hypot
*/

/*
**  return found point in *x and *y
** return 0 on success ,-1 on error
*/

double fabs ();

get_line_center (x, y, Points)
    double *x, *y;
    struct line_pnts *Points;
{
    register int i;
    register int n_points;
    register double *ux, *uy;
    double dist;		/* running total of line length */
    double half_dist;		/* half total line length */
    double len;			/* tmp length of current line seg */
    double frac;		/* overshoot / line length */

    n_points = Points->n_points;
    ux = Points->x;
    uy = Points->y;

    if (n_points <= 0)
	return -1;
    if (n_points == 1)
    {
	*x = Points->x[0];
	*y = Points->y[0];
	return (0);
    }
	
    dist = 0.0;
    /* get total dist */
    for (i = 1 ; i < n_points ; i++)
	dist += (fabs(ux[i]-ux[i-1]) + fabs(uy[i]-uy[i-1]));
    if (dist == 0.0)
    {
	*x = Points->x[0];
	*y = Points->y[0];
	return (0);
    }

    half_dist = dist / 2.0;

    dist = 0.0;
    for (i = 1 ; i < n_points ; i++)
    {
	len = (fabs(ux[i]-ux[i-1]) + fabs(uy[i]-uy[i-1]));
	dist += len;
	if (dist >= half_dist)  /* we're there */
	{
	    frac = 1 - (dist - half_dist) / len;
	    *x = frac * (ux[i]-ux[i-1]) + ux[i-1];
	    *y = frac * (uy[i]-uy[i-1]) + uy[i-1];
	    return (0);
	}
    }

    fprintf (stderr, "Get_line_center failed.\n");
    *x = Points->x[0];
    *y = Points->y[0];
    return (-1);
}
#endif

display_labeled_areas (map)
    struct Map_info *map;
{
    display_all_areas (map);
}

display_all_areas (map)
    struct Map_info *map;
{
    register int i;
    int ret = 0 ;
    char buf[100];

    set_keyboard ();
    for (i = 1 ; i <= map->n_areas ; i++)
    {
	if (key_hit (buf))
	{
	    if (*buf == ESC)
	    {
		ret = -1;
		break;
	    }
	}
	if (AREA_LABELED (&(map->Area[i])))
	    display_area (i, map);
    }
    unset_keyboard ();
    R_flush ();
    return (ret);
}

display_labeled_lines (map)
    struct Map_info *map;
{
    register int i;
    for (i = 1 ; i <= map->n_lines ; i++)
	if (LINE_ALIVE (&(map->Line[i])) && map->Line[i].att && 
		line_in_window (&(map->Line[i]))) 
	{
	    V1_read_line (map, &Gpoints, map->Line[i].offset);
	    _display_line (map->Line[i].type, &Gpoints, i, map);
	}
    R_flush ();
}

/* this is (no longer) a hidden feature for whatever use */
label_all_lines (map, cat)
    struct Map_info *map;
    int cat;
{
    int line, att;
    double x, y;

    Write_info (2, "Processing ...");
    for (line = 1 ; line <= map->n_lines ; line++)
    {
	Clear_info ();

	/* only do this for LINE lines */
	/* if already labeled, leave it alone */
	if (LINE_ALIVE (&(map->Line[line])) && map->Line[line].type == LINE && !map->Line[line].att)
	{

	    if(0 > V1_read_line(map, &Gpoints, map->Line[line].offset))
		return (-1);
	    /*
	    erase_line (map->Line[line].type, &Gpoints, line, map);
	    */
	    get_line_center (&x, &y, &Gpoints);
	    att = dig_new_att (map, x, y, LINE, line, cat);
	    if (att < 0)
		return (-1);
	    map->Line[line].att = att;
	    Changes_Made = 1;
	    display_line (map->Line[line].type, &Gpoints, line, map);
	}
    }
    Write_info (2, "Processing ...   DONE");
}


#ifdef SCS_MODS
ask_for_name(Type,pcats)
    int Type;
    struct Categories *pcats ;
{
    int ans, ans2, ier;
    char buffr[128];
    char *title;

    ans = -1;
    while (ans < 0)
    {
	Clear_info ();
	if (Type == 1)         /* LINE */
	    ans = curses_yes_no_default(2,
	    " Do you wish to enter line names? ", 1);
	else if (Type == 2)    /* AREA */
	    ans = curses_yes_no_default(2,
	    " Do you wish to enter area names? ", 1);
	else if (Type == 4)     /* DOT */
	    ans = curses_yes_no_default(2,
	    " Do you wish to enter site names? ", 1);
	else                   /* PSU  always enter names */
	    ans = 1;

	if (ans)
	{
	    /* Make Master Category dir, if not existing */
	    G__make_mapset_element("SUBJ") ;
	    while(1)
	    {
		ans = -1;
		while(ans == -1)
		{
		    Clear_info ();
		    Write_info (2, " Enter the SUBJECT matter ");
		    Write_info (3, " Enter 'list' for available Subject files");
		    Write_info (4, " <CR> to Abort/Quit): ");
		    Get_curses_text(buffr,20);
		    if (strlen(buffr) == 0) 
			ans = 0;
		    else
		    if (strcmp(buffr,"list") == 0) 
			list("SUBJ");
		    else
			ans = 1;
		}

		if (ans == 0) break;
		N_subj_file = G_store(buffr);

		/* read category file , if it exists*/
		G_suppress_warnings (1);
		ier = G__read_cats ("SUBJ", N_subj_file, G_mapset(), pcats, 1);
		G_suppress_warnings (0); 
		if (ier < 0)
		{ 
		    if (Type == PSU)
		    {
			_Clear_info ();
			Write_info (2, "PSU SUBJ file must already exist.");
			sleep (3);
			return -1;
		    }
		    Clear_info ();
		    sprintf(buffr," Do you want to create SUBJ/ file <%s>? ",N_subj_file);
		    ans2 = curses_yes_no_default (2, buffr, 1);
		    if (ans2)
		    {
			G_init_cats ((CELL)0, N_subj_file, pcats);
			G_set_cat ((CELL)0, "no data", pcats); 
			return(1);
		    }
		    else break;
		}  /* for ier < 0 */
		else
		    return(1);
	    }   /* end of while */
	}    /* for if ans */
	if (ans == 0) return(0);
    }  /* end while */
    return (0);
}

ask_name(pcats)
    struct Categories *pcats ;
{
    int i, icode, recd, ier;
    char buffr[128], area_name[40], cat_name[40];
    char *nptr, *cptr ;

    while (1)
      {
      Clear_info();
      Write_info( 4, "   Enter a label (<CR> to END): ");
      Get_curses_text (buffr,40) ;
      if (!strlen(buffr)) 
	 {
         Clear_info();
         return(0);
         }

      strcpy(area_name,buffr);
      nptr = area_name;

	/* find input string in category struct, assign category value to the
		    area_name based on category file record number*/
      recd = pcats->count;             /* set the number of categories */
      for (i=0;i<recd;i++)                /* category search */
	{		 
	                                    /* get a category label */
        sscanf (pcats->list[i].label, "%s", cat_name);
	cptr = cat_name;                /* first part only */

	if (strcmp(nptr,cptr) == 0)     /* compare for match */
	   {                           /* match, assigned already */
	   icode = pcats->list[i].num; /* set icode to category code */
	   return(icode);
	   }
	} 
	/* end of category search, NO category names match */

      Clear_info ();
      sprintf(buffr," Add new category <%d>, named <%s> ? ",recd,nptr);
      if (curses_yes_no_default (2, buffr, 1)) 
	 {                                      /* user said YES */
	 G_set_cat ((CELL)recd, nptr, pcats);  /* create entry */
         ier = G__write_cats ("SUBJ", N_subj_file, pcats);
         if (ier < 0)
            { 
            sprintf(buffr," Error in writting SUBJ file <%s>",
					     N_subj_file);
            Write_info(2,buffr); sleep(2);
            }
	 return(recd);
	 }
      }   /* end of while */
}

label_psu  (map, cat)
    struct Map_info *map;
    int cat;
{
    int line, area, att;
    double x, y;
    int ret;

	Clear_info ();
	if (Digtiz_Device == MOUSE)
	      {
	         /* find_line_with_mouse  fills Gpoints */
	      new_point_with_mouse (&x, &y, "Select point within area:");
	      }
	else
	      if (D_cursor_buttons())
	      {
	         /* find_line_with_dig  fills Gpoints */
	      new_point_with_dig (&x, &y, "Select point within area:");
	      }

	if (x == 0.0 && y == 0.0)
	    return (-1);

	/* change color so they know something happend */
	R_standard_color (dcolors[CLR_AMARK]); 
	Blot (&x, &y);
	local_x = x; local_y = y;	/* store these for tell_area_label() */

	local_prev = 0;	/* reset static flag */
	/* find_line loads global struct: Garea */
	if (Digtiz_Device == MOUSE)
	      {
	      if (0>=(line = find_line_with_mouse (AREA, "Select a Boundary line:", tell_area_label)))
	          {
	          unset_dot (x, y);
	          return (-1);
	          }
	      }
	else
	      if (D_cursor_buttons())
	      {
	      if (0>=(line = find_line_with_dig (AREA, "Select a Boundary line:", tell_area_label)))
	          {
	          unset_dot (x, y);
	          return (-1);
	          }
	      }

	if (make_area_label (map, line) >= 0)	/* completed an area? */
	{

	   if (Digtiz_Device == MOUSE)
	      {
	      if (2 == mouse_yes_no ("Accept this area ? "))
		    return (0);
              ret = 1;
	      }
	   else
	      if (D_cursor_buttons())
	      {
	      if ( ! ask_driver_yes_no("Accept this area ? ") )
			return(0) ;
              ret = 1;
	      }

          if (ret)
	  {
	    /* if this far, then an area is selected, either old or new */
	    /*  if local_area, then is old, else  Garea holds area info */
	    if (local_area)
	    {
		P_ATT *AP;
		char buf[100];

		if (!map->Area[local_area].att)
		{
/*DEBUG*/ debugf ("Label area: creating new attribute\n");
		    map->Area[local_area].att = 
			dig_new_att (map, local_x, local_y, AREA, local_area, cat);
		}
		else
		{
/*DEBUG*/ debugf ("Label area: attribute exists changing it\n");
		    AP = &(map->Att[map->Area[local_area].att]);
		    sprintf (buf, "%d", AP->cat);
		    R_standard_color (dcolors[CLR_ERASE]);
		    _Blot (&(AP->x), &(AP->y));
		    Adot (&(AP->x), &(AP->y), buf);
		    AP->cat = cat;
		    AP->x = x;
		    AP->y = y;
		    dig_update_att (map, map->Area[local_area].att);
		}

		display_area (local_area, map);
		area = local_area;
	    }
	    else
	    {
/*DEBUG*/ debugf ("Label area: new area:  ");
		area = dig_new_area (map, &Garea, 0);	/* give dummy att info*/
		if (area < 0) return (-1);		/* out of memory? */
/*DEBUG*/ debugf (" creating new attribute\n");
		att = dig_new_att (map, x, y, AREA, area, cat);	/* create new att  */
		if (att < 0)
		    return (-1);
		if (att < 0) return (-1);		/* out of memory? */
		map->Area[area].att = att;		/* stick in att info */
		display_area (area, map);
	    }
	    display_area_label (area, map);
	    Changes_Made = 1;
	  }
	  else	/* cleanup and leave */
	  {
	    display_line (map->Line[line].type, &Gpoints, line, map);
	    R_standard_color (dcolors[CLR_ERASE]);
	    Blot (&local_x, &local_y);
	    if (Disp_outline && local_area && AREA_LABELED (&(map->Area[local_area])))
		display_area (local_area, map);
	    else
		if (local_area)
		    reset_area (local_area, map);
		else
		    _reset_area (&Garea, map);
	  }
	}
	else  /* area not made */
	{
	    R_standard_color (dcolors[CLR_ERASE]);
	    Blot (&local_x, &local_y);
	}
}

T__read_cats (element, name, mapset, pcats, full)
    char *element;
    char *name ;
    char *mapset ;
    struct Categories *pcats ;
{
    FILE *fd ;
    char buff[258], *calloc() ;
    CELL cat;
    int i, old;
    long num;


    if (!(fd = G_fopen_old (element, name, mapset)))
	return -2 ;

/* Read the number of categories */
    if (G_getl(buff,sizeof buff,fd) == NULL)
	goto error;

    if (sscanf ( buff, "# %ld"   , &num) == 1)
	old = 0;
    else if (sscanf ( buff, "%ld"   , &num) == 1)
	old = 1;
    else
	goto error;

    if (num < 0)
	goto error;
    if (!full)
    {
	fclose (fd);
	return (CELL) num;
    }

/* Read the title for the file */
    if (G_getl(buff,sizeof buff,fd) == NULL)
	goto error;
    G_strip (buff);
    G_ascii_check(buff) ;
    G_init_cats ((CELL)num, buff, pcats);

#ifdef SYSV
        {
	    long len;
	    long nalloc;

	   /* pre-allocate memory for possiblily LARGE cats file */

            pcats->count = num + 1;
	    nalloc = num + 1;
	    len = (long) nalloc * sizeof(struct Cat_List) ;
	    if ((len % 16) != 0) len = len + (len % 16);
sprintf(buff,"len= %d",len);
Write_info(1,buff); sleep(3);
	    if (len != (int) len) /* make sure len doesn't overflow int */
		       return -1;
    	    pcats->list = (struct Cat_List *) G_calloc(nalloc, sizeof(struct Cat_List));
	    pcats->nalloc = nalloc;
	}
#endif

    if (!old)
    {
	char fmt[256];
	float m1,a1,m2,a2;
	if (G_getl(fmt,sizeof fmt,fd) == NULL)
		goto error;
/* next line contains equation coefficients */
	if (G_getl(buff,sizeof buff,fd) == NULL)
		goto error;
	if(sscanf(buff, "%f %f %f %f", &m1, &a1, &m2, &a2) != 4)
		goto error;
	G_set_cats_fmt (fmt, m1, a1, m2, a2, pcats);
    }

/* Read all category names */
    for (cat=0;;cat++) 
    {
	char label[256];
	if (G_getl(buff, sizeof buff, fd) == 0)
	    break;
	if (old)
	    G_set_cat (cat, buff, pcats) ;
	else
	{
	    *label = 0;
	    if (sscanf (buff, "%ld:%[^\n]", &num, label) < 1)
		goto error;

#ifdef SYSV
            G_ascii_check (label);
            G_strip (label) ;      
            pcats->list[num].num = num;
            pcats->list[num].label = G_store(label) ;
            if (num > pcats->num)
               pcats->num = num;   
#else
  	    G_set_cat ((CELL)num, label, pcats);
#endif
	}
    }
    G_sort_cats (pcats);

    fclose (fd);
    return 0 ;
error:
    fclose (fd);
    return -1 ;
}

T__set_cat (num, label, pcats)
    CELL num ;
    char *label ;
    struct Categories *pcats ;
{
    int n;
char buff[50];

sprintf(buff,"set: got %d",num);
Write_info(1, buff); 
    if (find_cat (num, pcats, &n))
    {
	if (pcats->list[n].label != NULL)
	    free (pcats->list[n].label) ;
    }
    else
    {
sprintf(buff,"set: not existing, ADD: cnt= %d, alloc= %d",pcats->count,pcats->nalloc);
Write_info(3, buff); sleep(3);
	n = pcats->count++;
	if (pcats->count > pcats->nalloc)
	{
	    long len;
	    long nalloc;

	    nalloc = pcats->nalloc + 256;
/*#ifdef SYSV
	    nalloc = pcats->nalloc + 2;
#else
	    nalloc = pcats->nalloc + 256;
#endif*/
	    len = (long) nalloc * sizeof(struct Cat_List) ;
	    if (len != (int) len) /* make sure len doesn't overflow int */
	    {
		pcats->count--;
		return -1;
	    }
	    pcats->list = (struct Cat_List *) G_realloc(pcats->list, (int)len);
	    pcats->nalloc = nalloc;
sprintf(buff,"set: alloc= %d",pcats->nalloc);
Write_info(4, buff);  sleep(3);
	}
    }
    G_ascii_check (label);
    G_strip (label);
    pcats->list[n].num = num;
    pcats->list[n].label = G_store(label) ;
    if (num > pcats->num)
	pcats->num = num;
    return 1;
}


static
find_cat (num, pcats, i)
    CELL num;
    struct Categories *pcats;
    int *i;
{
    int n;
char buff[50];
    for (n = 0; n < pcats->count; n++)
	if (pcats->list[n].num == num)
	{
		*i = n;
sprintf(buff,"find: got %d, i= %d",num,i);
Write_info(2, buff); 
if (num > 1996) sleep(2);
		return 1;
	}
sprintf(buff,"find: NO %d, n= %d",num, n);
Write_info(2, buff); 
sleep(2);
    return 0;
}
#endif /* SCS_MODS */
