
/* Changed for truecolor 24bit support by 
 * Roberto Flor/ITC-Irst, Trento, Italy
 * August 1999
 *
 * added new parameter "nlev" to specify number of colors per color channel
 * example; nlev=8 means 8bit for each R, G, B equal to 24bit truecolor
*/
     

/* This driver extensively updated by P. Thompson
 * (phils@athena.mit.edu) on 9/13/90 Driver modified to work with
 * Decstation X11r3 by David B. Satnik on 8/90 */
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include "includes.h"
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include "icon.bit"
#include "win.h"
#include "version.h"

#define BORDER  3
#define FONT "fixed"

#define HORIZ_PIXELS 1000        /* 512 x 512 for small driver */
#define VERT_PIXELS  800        /* 1000 x 800 for big driver */


/* This program is a rewrite of the original Grah_Set from the GRASS
 * 3.0 version. All suncore and sunview related stuff (which was the
 * bulk of the original code) has been replaced by X11 library calls.
 * All non-suncore code has been retained. */

/* declare global variables */
int SCREEN_LEFT, SCREEN_RIGHT, SCREEN_BOTTOM, SCREEN_TOP;
unsigned SC_WID = 0, SC_HITE = 0;
int NCOLORS, backing_store;
#ifndef ORIG
Visual        *theVisual;
#endif

int scrn = 0;
GC gc;
Colormap floatcmap, fixedcmap = 0;
Cursor grcurse, grxh;
u_long gemask;
XFontStruct *fontstruct;        /* Font descpritor */
Atom wmProtocolsAtom;
Pixmap bkupmap = (Pixmap)0;
int truecolor=0;
static void sigint();
static int get_user (char *,int *,int);

#ifdef ORIG
int Graph_Set (int argc, char **argv)
#else /* ORIG */
int Graph_Set (int argc, char **argv, int nlev)
#endif /* ORIG */
{
    int i;
    XGCValues gcv;              /* Struct for creating GC */
    XSetWindowAttributes xswa;  /* Set Window Attribute struct */
    XWindowAttributes xwa;      /* Get Window Attribute struct */
    Atom closedownAtom;
#ifndef X11R3
    XTextProperty windowName, iconName;
#endif /* X11R3 */
    XSizeHints *szhints;
    XClassHint *clshints;
    XWMHints *wmhints;
    XColor fg_color, bg_color, xcolors[256];
    char title[1024];
    Visual	*use_visual = 0;
    Display	*use_display = 0;
    Depth	*use_depth = 0;
    Screen	*use_screen = 0;    

    int		result, use_bit_depth, use_depth_num;
    XVisualInfo	*mvisual_info;
    char classname [6][25]; 
         strcpy(classname [StaticGray],"StaticGray");
         strcpy(classname [GrayScale],"GrayScale");
         strcpy(classname [StaticColor],"StaticColor");
         strcpy(classname [PseudoColor],"PseudoColor");
         strcpy(classname [TrueColor],"TrueColor");
         strcpy(classname [DirectColor],"DirectColor");
        
    /* Open the display using the $DISPLAY environment variable to
     * locate the X server. Return 0 if cannot open. */
    if (!(dpy = XOpenDisplay(NULL))) {
        fprintf(stderr, "Graph_Set: can't open Display %s\n", XDisplayName(NULL));
        return 0;
    }
    scrn = DefaultScreen(dpy);
    /* scrn is the screen number */
    /* use_screen is a pointer to that screen structure */
    use_screen = ScreenOfDisplay(dpy, scrn);
    
    backing_store = (DoesBackingStore(DefaultScreenOfDisplay(dpy)) == Always);

    /* Load the font to use. GRASS doesn't at this point use fonts, but
     * may someday in the future. */
    if (!(fontstruct = XLoadQueryFont(dpy, FONT))) {
        fprintf(stderr, "Graph_SetX: display %s doesn't know font %s\n",
                DisplayString(dpy), FONT);
        return 0;
    }
    /* Allocate and set the fixed colormap, in the default X colormap
     * if possible, and set background and foreground of gc */
    /****************************************************************
     * this code change was suggested by Ann Stout, SCS contractor
     * (303)-229-0414 to solve the problem if two drivers are started,
     * the second driver has less colors to work with than the first.
     * It works wonderfully sometimes, other times the second driver
     * will die. The color logic in this code (and in GRASS generally)
     * is convoluted, and needs close scrutiny and revision. This
     * "hack" just solves one problem in certain cases while creating
     * more problems in other scenarios
     ***************************************************************/
/*  NCOLORS = DisplayCells(dpy, scrn) ; */
    NCOLORS = 0;

/*  try each depth available on the default screen 
 *   looking for a depth that can run TrueColor 
 *   be desirous of an 24 bit depth display
*/  
    use_visual = 0;
    
/*	The Following line dead loops under XFree85 3.3.5 if the bit depth is 
/*	the same as your bit depth.  The same is not true under SUN openwin.		*/
/*    result = XMatchVisualInfo (dpy, scrn, 16, TrueColor, mvisual_info);
*/

/* special flag to indicate a search for a True Color Display */
   if ( nlev == -1 )
   {
    int num_visuals;
    int search_mask = VisualClassMask;
    int highest_bit_depth = 0;
    int vis_num;

    XVisualInfo search_template;

    search_template.class = TrueColor;

    mvisual_info  = 
	XGetVisualInfo(dpy, search_mask, &search_template, &num_visuals );

    fprintf (stderr,"found %d visuals of type %s...", num_visuals, 
             classname[search_template.class]);
    for ( vis_num = 0; vis_num < num_visuals; vis_num++ )
    {
      fprintf (stderr," searching for highest bit depth \n");
	if ( mvisual_info[vis_num].depth > highest_bit_depth )
        {
          use_visual = mvisual_info[vis_num].visual;
	  scrn = mvisual_info[vis_num].screen;
	  use_bit_depth =  mvisual_info[vis_num].depth;
	  highest_bit_depth = use_bit_depth;
	  
	  /* this next bit forces the use of a private colormap 			   */
	  /* a must have for read only visuals   							   */
	  /* since we could allow any type to be selected above assume that    */
	  /* if this is not the default visual we must use a private colormap  */
          if ( use_visual != DefaultVisual(dpy,scrn) )
          {
            fixedcmap = XCreateColormap(dpy, DefaultRootWindow(dpy), 
                                        use_visual, AllocNone);
          } 
          
          if ( use_visual->class == TrueColor || use_visual->class == DirectColor ) {
  	    nlev = mvisual_info[vis_num].colormap_size;
  	  } else {
  	    NCOLORS = mvisual_info[vis_num].colormap_size;
  	  }
  	  /* dont let GRASS try to allocate every color possible  				*/
	  if ( nlev > 32 ) { nlev = 32; } 
 	}
    }
    XFree (mvisual_info);

    if ( highest_bit_depth == 0 ) {
      fprintf (stderr, "unable to find a %s display ...\n", 
                 classname[search_template.class]); 
    }
    else
    {
      fprintf (stderr, "selected %d bit depth\n", use_bit_depth);     
    }
   }

    if ( nlev == -1 )
    {
    /* if nlev is still -1 let it fall through to the default value of 32 		*/
      nlev = 32;
    }

/*  If We can't find the visual class secified above then drop to the 			*/
/*  DefaultVisual																*/  
    if ( use_visual == 0 ) {
      use_bit_depth = DefaultDepth (dpy, scrn);
      use_visual = DefaultVisual (dpy, scrn); 
      fprintf (stderr, "using default visual which is %s\n",classname[use_visual->class]);
    }  

    if ( use_visual->class == DirectColor || use_visual->class == StaticColor ||
    	 use_visual->class == StaticGray )
    { 
      fprintf (stderr, "the visual you have is not fully supported at this time\n");
      fprintf (stderr, "expect oddities\n\n");
    }
           
/* These are all read-only Visual Classes     									 */
/* plus don't allocate default colormap if we are using a private colormap 		 */
    if ( use_visual->class == TrueColor || use_visual->class == StaticGray ||
         use_visual->class == StaticColor  ) {
#ifndef ORIG
      theVisual = use_visual;
#endif /* ORIG */
/*    the truecolor really indicates that a private colormap should be used      */
      truecolor++;
      fprintf(stderr, "Visual is read-only or using a private colormap\n");

#ifdef ORIG
      NCOLORS=256;
#else /* ORIG */
      if ( !NCOLORS ) {
        NCOLORS=nlev*nlev*nlev; 	
      }
#endif /* ORIG */

#ifndef ORIG
      fprintf(stderr,"ncolors: %d\n",NCOLORS);
      fprintf(stderr,"allocating memory...\n");
#endif /* ORIG */

    } else {
      NCOLORS = GetRealNumberOfColors(dpy, scrn) ;
#ifdef ORIG
       fprintf(stderr,"Found %d colors on Display\n",NCOLORS);
#endif /* ORIG */
      NCOLORS = use_visual->map_entries;

      if (!NCOLORS) {
	fprintf(stderr,"can not allocate any read/write color cells\n");
	exit(1);
      }
      fprintf(stderr,"ncolors: %d\n",NCOLORS);
    }
    
    bg_color.pixel = BlackPixel(dpy, scrn);
    fg_color.pixel = WhitePixel(dpy, scrn);

    if ( fixedcmap  == 0 ) {
      fixedcmap = DefaultColormap(dpy, scrn); 
    }

    XQueryColor(dpy, fixedcmap, &bg_color);
    XQueryColor(dpy, fixedcmap, &fg_color);
    fixedcmap = InitColorTableFixed(fixedcmap); 

/*  Below Here there should no more use DefaultXxxxxx routines from X   */
/*  Just in case we dont want to use the Defaults 			*/

    if (!truecolor) {
      (void) XAllocColor(dpy, fixedcmap, &bg_color);
      (void) XAllocColor(dpy, fixedcmap, &fg_color);

    /* Allocate floating colormap and set with b/w stripes */

      floatcmap = XCreateColormap(dpy, use_screen->root,
				  use_visual, AllocAll);
      for (i = 0; i < NCOLORS; i++) {
        xcolors[i].pixel = i;
      }
      XQueryColors(dpy, fixedcmap, xcolors, NCOLORS);
      XStoreColors(dpy, floatcmap, xcolors, NCOLORS);
    }

    /* Deal with providing the window with an initial position & size.
     * Window is is not resizable */
#ifdef X11R3
    szhints = (XSizeHints *)G_malloc((size_t) sizeof(XSizeHints));
#else
    szhints = XAllocSizeHints();
#endif

    szhints->flags = (USPosition | USSize | PMinSize | PMaxSize);
	szhints->max_width	= DisplayWidth(dpy, scrn) - 20;
	szhints->min_width	= 0;
	szhints->min_height = 0;
	szhints->max_height = DisplayHeight(dpy, scrn) - 30 ;
	get_user ("XDRIVER_HEIGHT", &szhints->height, 400);
	get_user ("XDRIVER_WIDTH",  &szhints->width,  450);
	get_user ("XDRIVER_LEFT",   &szhints->x,      10);
	get_user ("XDRIVER_TOP",    &szhints->y,      10);

    /* Start with the window's colormap field pointing to the default
     * colormap. Later pointer enter events will cause a switch to the
     * grass color map. Also, set Bit Gravity to reduce Expose events. */
    xswa.event_mask = gemask = 
	    ExposureMask | ButtonPressMask | StructureNotifyMask;

    xswa.backing_store = (backing_store ? Always : NotUseful);
    xswa.colormap = fixedcmap;
    xswa.border_pixel = fg_color.pixel;
    xswa.background_pixel = bg_color.pixel;
    /* Create the Window with the information in the XSizeHints, the
     * border width,  and the border & background pixels. */

    grwin = XCreateWindow(dpy, RootWindow(dpy, scrn), szhints->x,
            szhints->y, (unsigned)szhints->width, (unsigned)szhints->height, 
			BORDER, use_bit_depth,
			InputOutput, use_visual,
			(CWEventMask | CWBackingStore | CWColormap | CWBorderPixel |
			CWBackPixel), &xswa);

    /* properties for window manager */
#ifdef X11R3
    wmhints = (XWMHints *)G_malloc((size_t) sizeof(XWMHints));
#else
    wmhints = XAllocWMHints();
#endif
    wmhints->icon_pixmap = XCreateBitmapFromData(dpy, grwin, icon_bits,
            icon_width, icon_height);
    wmhints->flags |= IconPixmapHint;

#ifdef X11R3
    clshints = (XClassHint *)G_malloc((size_t) sizeof(XClassHint));
#else
    clshints = XAllocClassHint();
#endif
    clshints->res_name = NULL;
    clshints->res_class = WIN_NAME;
#ifndef X11R3
    sprintf (title, "%s - Monitor: %s", WIN_NAME, argv[0]);
    iconName.encoding = XA_STRING;
    iconName.format = 8;
    iconName.value = (u_char *) title;
    iconName.nitems = strlen((char *) iconName.value);
    windowName.encoding = iconName.encoding = XA_STRING;
    windowName.format = iconName.format = 8;
    windowName.value = (u_char *) title;
    windowName.nitems = strlen((char *) windowName.value);
    XSetWMProperties(dpy, grwin, &windowName, &iconName, argv, argc,
            szhints, wmhints, clshints);
#endif
    closedownAtom = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    wmProtocolsAtom = XInternAtom(dpy, "WM_PROTOCOLS", False);
#ifdef X11R3
    {
        char data[25];
        Long2Bytes((long)closedownAtom, data);
        AppendProperty(dpy,grwin,wmProtocolsAtom,XA_ATOM,32,data,1);
    }
#else
    (void) XSetWMProtocols(dpy, grwin, &closedownAtom, 1);
#endif

    /* Create the cursors to be used later */
    grcurse = grxh = XCreateFontCursor(dpy, XC_crosshair);

    /* Create the GC for writing the text. */
    gcv.font = fontstruct->fid;
    gcv.foreground = fg_color.pixel;
    gcv.background = bg_color.pixel;
    gc = XCreateGC(dpy, grwin, (GCFont | GCForeground | GCBackground), &gcv);

    /* Map the window to make it visible. This causes an expose event */
    XMapWindow(dpy, grwin);
    XFlush(dpy);

    /* Find out how big the window really is (in case window manager
     * overrides our request) and set the SCREEN values. */
    SCREEN_LEFT = SCREEN_TOP = 0;
    if (XGetWindowAttributes(dpy, grwin, &xwa) == 0) {
        fprintf(stderr, "Graph_Set: cannot get window attributes\n");
        return 0;
    }
    SCREEN_RIGHT = xwa.width - 1;
    SCREEN_BOTTOM = xwa.height - 1;
    SC_WID = xwa.width;
    SC_HITE = xwa.height;

    if (!backing_store) {
        /* Now create a pixmap that will contain same contents as the
         * window. It will be used to redraw from after expose events */
        bkupmap = XCreatePixmap(dpy, grwin, SC_WID, SC_HITE, xwa.depth);
        XCopyArea(dpy, grwin, bkupmap, gc, 0, 0, (unsigned) SC_WID,
                (unsigned) SC_HITE, 0, 0);
    }
    /* prepare to catch signals */
    signal(SIGHUP, sigint);
    signal(SIGINT, sigint);
    signal(SIGQUIT, sigint);
    signal(SIGILL, sigint);
    signal(SIGTSTP, SIG_IGN);
    XFlush(dpy);
    return 1;
}

static void 
sigint (void)
{
    Graph_Close();
    exit(-1);
}

#ifdef X11R3

int 
Long2Bytes (long l, char data[])
{
    int i;
    union {
        long l;
        char bytes[4];
    } converter;
    
    converter.l = 1;
    for ( i = 0; i < 4; i++ ) {
        data[i] = converter.bytes[i];
    }
}

int AppendProperty (Display *display, Window window,
    Atom property, Atom target, int format, char *data, int number_items)
{
    if ( number_items > 0 ) {
        XChangeProperty(display, window, property, target, format,
              PropModeAppend, data, number_items);
    }
}
#endif

int GetRealNumberOfColors(Display *dpy, int screen)
{
    int i = DisplayCells(dpy,screen);
    int cant_alloc = 1;
    unsigned long *mypixels = (unsigned long *)calloc((size_t) i, (size_t) sizeof(unsigned long));

    while ( i && cant_alloc ) {
	int result;

	result = XAllocColorCells(dpy, DefaultColormap(dpy,screen), 0,
		    NULL, 0, mypixels, i);
	if ( result != 0 ) cant_alloc = 0;
	else i--;
    }
    if ( !cant_alloc ) 
        XFreeColors(dpy, DefaultColormap(dpy,screen), mypixels, i, 0);
    free(mypixels);
    return i;
}

static int get_user (char *name, int *n, int value)
{
    char *p;

    if ((p=getenv(name)) && sscanf (p, "%d", n) == 1) return 1;
    *n = value;

    return 0;
}
/*** end Graph_Set.c ***/
