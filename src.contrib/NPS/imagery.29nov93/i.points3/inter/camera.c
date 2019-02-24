#include "globals.h"
#define  GLOBALCAM
#include "camera_ref.h"
#define NLINES 20
#define BACKGROUND COLOR_GREY

setup_camera_file()
{
Auxillary_Photo  *auxil;

    static int use = 1;
    int setup(), no(), yes();

    static Objects objects[]=
    {
	INFO("Do you wish to use the camera reference file?  ", &use),
	MENU("YES", yes, &use),
	MENU("NO",  no, &use),
	{0}
    };


/* make visiable */
   auxil = (Auxillary_Photo *) group.auxil;


    Input_pointer (objects);

    if (from_camera_fid) { 
      /** look for camera info  for this group **/
      if (I_get_group_camera (group.name, &auxil->camera) < 0)
	{ 
	  from_camera_fid = 0;
	  from_flag_fid = 0;
	  return 1;
	}
    }

    return(0);
}

static
no()
{
    from_camera_fid = 0;
    from_flag_fid = 0;
    return -1;
}

static
yes()
{
    from_camera_fid   = 1;
    from_flag_fid   = 1;
    return -1;
}


draw_camera_file()
{
Auxillary_Photo  *auxil;

  static int use = 1;

/* make visiable */
  auxil = (Auxillary_Photo *) group.auxil;

  /** look for camera info  for this group **/
  if (I_get_group_camera (group.name, &auxil->camera) < 0)
    { 
      from_camera_fid = 0;
      return 0;
    }
  
  drawcamnew();
  return(0);
}


static
drawcamnew ()
{
Auxillary_Photo  *auxil;

/* make visiable */
  auxil = (Auxillary_Photo *) group.auxil;

/* to give user a response of some sort */
    Menu_msg ("Preparing Camera Reference File...");

/*
 * build a popup window in bottom right of the screen.
 * 35% the height and wide enough to hold the report
 *
 */

/* height of 1 line, based on NLINES taking up 45% vertical space */
    height = (.35 * (SCREEN_BOTTOM - SCREEN_TOP))/NLINES  + 1;

/* size of text, 80% of line height */
    tsize = .8 * height;
    size = tsize - 2;  /* fudge for computing pixels width of text */

/* indent for the text */
    edge = .1 * height + 1;

/* determine the length, in chars, of printed line */
    FMT0 (buf,0.0);
    nums = strlen(buf) * size;
    FMT1 (buf," ", 0.0, 0.0);
    len = strlen(buf);
    middle = len * size;

/* width is for max chars plus sidecar for ess */
    width = len * size + nums + (2*height);
    if ((SCREEN_RIGHT - SCREEN_LEFT) < width)
	width = SCREEN_RIGHT - SCREEN_LEFT;


/* define the window */
    bottom = VIEW_MENU->top-1;
    top = bottom - height*NLINES;


    left  = SCREEN_RIGHT - width;
    right = SCREEN_RIGHT;
    middle += left + nums;
    nums += left;

/* save what is under this area, so it can be restored */
    R_panel_save (tempfile_camera, top, bottom, left, right);


/* fill it with white */
    R_standard_color (BACKGROUND);
    R_box_abs (left, top, right, bottom);

    right -= 2*height;	/* reduce it to exclude sidecar */

/* print messages in message area */
    R_text_size (tsize, tsize);


/* setup the more/less boxes in the sidecar */
    R_standard_color (COLOR_BLACK);
    less.top = top;
    less.bottom = top + 2*height;
    less.left = right;
    less.right = right + 2*height;
    Outline_box (less.top, less.bottom, less.left, less.right);

    more.top = bottom - 2*height;
    more.bottom = bottom;
    more.left = right;
    more.right = right + 2*height;
    Outline_box (more.top, more.bottom, more.left, more.right);

/*
 * top eight lines are for column labels
 * last two line is for overall rms error.
 */
    nlines = NLINES - 11;
    first_point = 0;

/* allocate predicted values */
    Xf = (double *) G_calloc (auxil->camera.num_fid, sizeof (double));
    Yf = (double *) G_calloc (auxil->camera.num_fid, sizeof (double));


/* put head on the report */
    cury = top;
    dotext (LHEAD1, cury, cury+height, left, right-1, 0, COLOR_BLACK);
    cury += height;
    dotext (LHEAD2, cury, cury+height, left, right-1, 0, COLOR_BLACK);
    cury += height;
    
    FMT2(buf, auxil->camera.cam_name);
    dotext (buf, cury, cury+height, left, right -1, 0, color);
    cury += height;
    FMT3(buf, auxil->camera.cam_id);
    dotext (buf, cury, cury+height, left, right -1, 0, color);
    cury += height;
    FMT4(buf, auxil->camera.CFL);
    dotext (buf, cury, cury+height, left, right -1, 0, color);
    cury += height;
    FMT5(buf, auxil->camera.Xp);
    dotext (buf, cury, cury+height, left, right -1, 0, color);
    cury += height;
    FMT6(buf, auxil->camera.Yp);
    dotext (buf, cury, cury+height, left, right -1, 0, color);
    cury += height;
    FMT7(buf, auxil->camera.num_fid);
    dotext (buf, cury, cury+height, left, right -1, 0, color);
    cury += height;
    
    dotext (LHEAD3, cury, cury+height, left, right -1, 0, COLOR_BLACK);
    cury += height;
    dotext (LHEAD4, cury, cury+height, left, right -1, 0, COLOR_BLACK);
    cury += height;
    
    R_move_abs (left, cury-1);
    R_cont_abs (right, cury-1);

/* isolate the sidecar */
    R_move_abs (right, top);
    R_cont_abs (right, bottom);

/* define report box */
/*  report.top = top + (10*height); */
    report.top = cury;
    report.left = left;
    report.right = right-1;

/* lets do it */
    pager = 1;


    R_text_size (tsize, tsize);
    line = 0;
    curp = first_point;
    cury = top + 10*height;
    while(1)
      {
	if (line >= nlines || curp >= auxil->camera.num_fid)
	  break;
	line++;
	color = COLOR_BLACK;
	if (pager)
	  {
	    FMT1(buf, auxil->camera.fiducials[curp].fid_id, auxil->camera.fiducials[curp].Xf, auxil->camera.fiducials[curp].Yf);
	    dotext (buf, cury, cury+height, left, right -1, 0, color);
	  }
	cury += height;
	curp++;
      }
    report.bottom = cury;
    downarrow (&more, curp < auxil->camera.num_fid ? COLOR_BLACK : BACKGROUND);
    uparrow   (&less, first_point > 0  ? COLOR_BLACK : BACKGROUND);
    R_standard_color (BACKGROUND);
    R_box_abs (left, cury, right-1, bottom);
    R_standard_color (COLOR_BLACK);
    R_move_abs (left, bottom-height);
    R_cont_abs (right-1, bottom-height);
    
    pager = 0;
    which = -1;

    return 1; /* return but don't QUIT */
}


static
uparrow (box, color)
    struct box *box;
{
    R_standard_color (color);
    Uparrow (box->top+edge, box->bottom-edge, box->left+edge, box->right-edge);
}

static
downarrow (box, color)
    struct box *box;
{
    R_standard_color (color);
    Downarrow (box->top+edge, box->bottom-edge, box->left+edge, box->right-edge);
}

static
pick(x,y)
{
Auxillary_Photo *auxil;
    int n;
    int cur;

/* make visible */
   auxil = (Auxillary_Photo *) group.auxil;

    cur = which;
    cancel_which();
    if (inbox(&more,x,y))
    {
	if (curp >= auxil->camera.num_fid)
	    return 0;
	first_point = curp;
	pager = 1;
	return 1;
    }
    if (inbox(&less,x,y))
    {
	if (first_point == 0)
	    return 0;
	first_point -= nlines;
	if (first_point < 0)
	    first_point = 0;
	pager = 1;
	return 1;
    }
    return -1; /* keep going */
    /* return 0;  ignore first click */
}

static
done()
{
    cancel_which();
    ok = 0;
    return -1;
}

static
cancel_which()
{
    if (which >= 0)
    {
	R_standard_color (BACKGROUND);
	Outline_box (report.top + which*height, report.top +(which+1)*height,
		         report.left, report.right-1);
	/* show_point (first_point+which, 1); */
    }
    which = -1;
}

static
inbox (box,x,y)
    struct box *box;
{
    return (x>box->left && x <box->right && y>box->top && y<box->bottom);
}

static
dotext (text, top, bottom, left, right, centered, color)
    char *text;
{
    R_standard_color (BACKGROUND);
    R_box_abs (left, top, right, bottom);
    R_standard_color (color);
    R_move_abs (left+1+edge, bottom-1-edge);
    if (centered)
	R_move_rel ((right-left-strlen(text)*size)/2,0);
    R_set_window (top, bottom, left, right);	/* for text clipping */
    R_text (text);
    R_set_window (SCREEN_TOP, SCREEN_BOTTOM, SCREEN_LEFT, SCREEN_RIGHT);
}







