/*  %W%  %G%  */
#include "graph.h"
#include <stdio.h>
#include <signal.h>
#define SWITCHER
#include "driver.h"

#define ITEM struct _item_
#define LIST struct _list
#define PAD struct _pad_

LIST
{
    char *value;
    LIST *next;
};

ITEM
{
    char *name;
    LIST *list;
    ITEM *next;
    ITEM *prev;
};

PAD
{
    char *name;
    ITEM *items;
    PAD *next;
    PAD *prev;
};

char *malloc();
char *store();
ITEM *new_item();
ITEM *find_item();
PAD *find_pad();

static char *me;
static int _wfd ;
static int _rfd ;
static int eof ;
static int broken_pipe ;
static int n_read ;
static int atbuf  ;
static char inbuf[4096] ;
static char current_command ;

static PAD *padlist ;  /* user created pads */
static PAD *curpad;       /* current selected pad */
static PAD *pad;        /* general purpose pointers */
static ITEM *item;
static LIST *list;

#define REC(x,y) if (eof=rec(x,y)) break
#define RECTEXT(x) if (eof=rectext(x)) break

main(argc,argv)
int argc;
char *argv[];
{
    char *xalloc();
    char achar ;
    char c;
    char lc;
    char name[200] ;
    char text[1024] ;
    float wx ;
    float wy ;
    int *xarray;
    int *yarray;
    int button ;
    int catch();
    int close_mon();
    int index ;
    int min, max ;
    int number ;
    int pid, syntax, fifos;
    int ras_elem ;
    int ras_rows ;
    int stat ;
    int t, b, l, r ;
    int x ;
    int y ;
    static int blu_alloc = 0 ;
    static int grn_alloc = 0 ;
    static int n_xarray = 0 ;
    static int n_yarray = 0 ;
    static int red_alloc = 0 ;
    unsigned char *blua ;
    unsigned char *grna ;
    unsigned char *reda ;
    unsigned char blu ;
    unsigned char grn ;
    unsigned char red ;

    /* whoami */
    me = argv[0];
    /* initialize the pads */
    padlist = NULL;
    curpad = NULL;
    create_pad ("");    /* scratch pad */

    /* multiple monitor startup */

    /* The calling syntax is as follows: */
    /*    monitor_name [-] "input_fifo output_fifo" */
    /* The "-", if present, causes the monitor to run in foreground. */
    /* Otherwise, once it has been determined that the monitor is not */
    /* already running, we will fork and the parent will exit, so that */
    /* the monitor is left running in background.  For more information, */
    /* see the comments in Dstart_mon.c. */

    syntax = 0;
    switch (argc)               /* syntax check */
    {
    case 2:
        if (*argv[1] == '-')
            syntax = 1;
        else
            fifos = 1;
        break;
    case 3:
        if (*argv[1] != '-')
            syntax = 1;
        else
            fifos = 2;
        break;
    default:
        syntax = 1;
    }
    if (syntax)
    {
        fprintf(stderr,"Usage:  %s [-] \"input_fifo output_fifo\"\n",me);
        exit(-1);
    }

    /* Syntax is all right.  Now we must check and see whether or not someone */
    /* (possibly another invocation of ourself) is using our fifo files. */
    /* If so, we can't run.  The message printed in this case is that the */
    /* monitor is already running.  If the monitorcap file is incorrect, this */
    /* may not actually be the case. */

    if (check_connection(me, argv[fifos]))  /* are our fifos available? */
    {                   /* if so, we can't run now */
        exit(-1);
    }

    /* We are free to run now.  No one is using our fifo files.  If we are */
    /* to run in background, we will have fifos == 1 from the syntax check */
    /* above. */

    if (fifos == 1)
    {
        if (pid = fork())
        {
            if (pid > 0)        /* parent exits */
            {
                fprintf(stderr,"Graphics driver [%s] started\n", me);
                fflush (stderr);
                exit(0);
            }
            else    /* weren't able to fork */
            {
                fprintf(stderr,"Error - Could not fork to start [%s]\n",me);
                fflush(stderr);
                exit(-1);
            }
        }
	else
	{
	/* change process groups to be shielded from keyboard signals */
	/* note: use BSD form of call, which will also work for ATT   */
	    setpgrp (0, getpid());
	}
    }               /* monitor runs */

#ifdef SIGPIPE
    signal (SIGPIPE, catch);
#endif
    signal(SIGTERM,close_mon);  /* exit gracefully if terminated */

    /* end of multiple monitor startup */

    /* initialize graphics */
    Graph_Set() ;

    /* Initialize color stuff */
    Color_table_fixed() ;

#ifdef DEBUG
#define debug(x) fprintf (stderr,"%s\n",x)
#else
#define debug(x)
#endif DEBUG

    while (1)   /* re-open upon EOF */
    {
	debug("Open the fifos");
        get_connection(argv[fifos],&_rfd, &_wfd) ;
        if (_rfd < 0 || _wfd < 0)
        {
            perror("no connection") ;
            exit(-1) ;
        }
	debug("Opened");

        atbuf = n_read = 0;

        eof = broken_pipe = 0;
        current_command = 0;

        while (eof <= 0 && !broken_pipe)
        {
	    debug ("get_command ...");
            if(get_command (&c) != 0)
	    {
		debug (" failed!");
                break;
	    }
#ifdef DEBUG
            switch(c)
            {
	    case BEGIN:
		debug("BEGIN");
		break;
            case GET_NUM_COLORS: 
                debug("GET_NUM_COLORS") ;   
                break ;
            case STANDARD_COLOR:        
                debug("STANDARD_COLOR") ;              
                break ;
            case RGB_COLOR:        
                debug("RGB_COLOR") ;              
                break ;
            case COLOR:        
                debug("COLOR") ;              
                break ;
            case COLOR_TABLE_FIXED: 
                debug("COLOR_TABLE_FIXED") ; 
                break ;
            case COLOR_TABLE_FLOAT: 
                debug("COLOR_TABLE_FLOAT") ; 
                break ;
            case COLOR_OFFSET: 
                debug("COLOR_OFFSET") ;       
                break ;
            case COLOR_PRINT:  
                debug("COLOR_PRINT") ;        
                break ;
            case CONT_ABS:     
                debug("CONT_ABS") ;           
                break ;
            case CONT_REL:     
                debug("CONT_REL") ;           
                break ;
            case ERASE:        
                debug("ERASE") ;              
                break ;
            case GET_LOCATION_WITH_BOX:
                debug("GET_LOCATION_WITH_BOX") ;         
                break ;
            case GET_LOCATION_WITH_LINE:
                debug("GET_LOCATION_WITH_LINE") ;        
                break ;
            case GET_LOCATION_WITH_POINTER:
                debug("GET_LOCATION_WITH_POINTER") ;     
                break ;
            case GRAPH_CLOSE:  
                debug("GRAPH_CLOSE") ;        
                break ;
            case LINEMOD:      
                debug("LINEMOD") ;            
                break ;
            case MOVE_ABS:     
                debug("MOVE_ABS") ;           
                break ;
            case MOVE_REL:     
                debug("MOVE_REL") ;           
                break ;
            case POLYGON_ABS:  
                debug("POLYGON_ABS") ;        
                break ;
            case POLYGON_REL:  
                debug("POLYGON_REL") ;        
                break ;
            case POLYLINE_ABS: 
                debug("POLYLINE_ABS") ;       
                break ;
            case POLYLINE_REL: 
                debug("POLYLINE_REL") ;       
                break ;
            case POLYDOTS_ABS: 
                debug("POLYDOTS_ABS") ;       
                break ;
            case BOX_REL: 
                debug("BOX_REL") ;     
                break ;
            case BOX_ABS: 
                debug("BOX_ABS") ;     
                break ;
            case POLYDOTS_REL: 
                debug("POLYDOTS_REL") ;       
                break ;
            case RASTER_CHAR:       
                debug("RASTER_CHAR") ;             
                break ;
            case RASTER_INT:       
                debug("RASTER_INT") ;             
                break ;
            case RGB_RASTER:       
                debug("RGB_RASTER") ;             
                break ;
            case RGB_COLORS:      
                debug("RGB_COLORS") ;            
                break ;
            case RESET_COLORS: 
                debug("RESET_COLORS") ;       
                break ;
            case RESET_COLOR:  
                debug("RESET_COLOR") ;        
                break ;
            case SCREEN_LEFT:  
                debug("SCREEN_LEFT") ;        
                break ;
            case SCREEN_RITE:  
                debug("SCREEN_RITE") ;        
                break ;
            case SCREEN_BOT:   
                debug("SCREEN_BOT") ;         
                break ;
            case SCREEN_TOP:   
                debug("SCREEN_TOP") ;         
                break ;
            case SET_WINDOW:   
                debug("SET_WINDOW") ;         
                break ;
            case GET_TEXT_BOX: 
                debug("GET_TEXT_BOX") ;       
                break ;
            case FONT:         
                debug("FONT") ;               
                break ;
            case TEXT:         
                debug("TEXT") ;               
                break ;
            case TEXT_SIZE:    
                debug("TEXT_SIZE") ;          
                break ;
            case TEXT_ROTATION: 
                debug("TEXT_ROTATION") ;     
                break ;
            case RESPOND: 
                debug("RESPOND") ;     
                break ;
            case PANEL_SAVE:
                debug("PANEL_SAVE") ;
                break ;
            case PANEL_RESTORE:
                debug("PANEL_RESTORE") ;
                break ;
            case PANEL_DELETE:
                debug("PANEL_DELETE") ;
                break ;
            case PAD_CREATE:      
                debug("PAD_CREATE") ;      
                break ;
            case PAD_CURRENT:     
                debug("PAD_CURRENT") ;     
                break ;
            case PAD_DELETE:      
                debug("PAD_DELETE") ;      
                break ;
            case PAD_INVENT:      
                debug("PAD_INVENT") ;      
                break ;
            case PAD_LIST:        
                debug("PAD_LIST") ;        
                break ;
            case PAD_SELECT:      
                debug("PAD_SELECT") ;      
                break ;

            case PAD_APPEND_ITEM: 
                debug("PAD_APPEND_ITEM") ; 
                break ;
            case PAD_DELETE_ITEM: 
                debug("PAD_DELETE_ITEM") ; 
                break ;
            case PAD_GET_ITEM:    
                debug("PAD_GET_ITEM") ;    
                break ;
            case PAD_LIST_ITEMS:  
                debug("PAD_LIST_ITEMS") ;  
                break ;
            case PAD_SET_ITEM:    
                debug("PAD_SET_ITEM") ;    
                break ;

            default: 
                break ;
            }
#endif DEBUG
            switch(c)
            {
	    case BEGIN:
		c = 0;
		for (index = -10; index < BEGIN_SYNC_COUNT; index++)
		    send (&c, 1);
		c = COMMAND_ESC;
		send (&c, 1);
		break;
            case RESPOND:
                send (&c, 1);
                break;
            case GET_NUM_COLORS:
                Number_of_colors(&index) ;
                send (&index, sizeof index) ;
                break;
            case STANDARD_COLOR:
                REC (&index, sizeof index) ;
                Standard_color(index) ;
		break;
            case COLOR:
                REC (&index, sizeof index) ;
                Color(index) ;
                break ;
            case RGB_COLOR:
                REC (&red, sizeof red) ;
                REC (&grn, sizeof grn) ;
                REC (&blu, sizeof blu) ;
                RGB_color(red,grn,blu) ;
                break ;
            case COLOR_TABLE_FIXED:
                x = Color_table_fixed() ;
                send(&x,sizeof x) ;
                break ;
            case COLOR_TABLE_FLOAT:
                x = Color_table_float() ;
                send(&x,sizeof x) ;
                break ;
            case COLOR_OFFSET:
                REC (&index, sizeof index) ;
                Color_offset(index) ;
                break ;
            case COLOR_PRINT:
                break ;
            case CONT_ABS:
                REC (&x, sizeof x);
                REC (&y, sizeof y);
                Cont_abs(x,y) ;
                break ;
            case CONT_REL:
                REC (&x, sizeof x);
                REC (&y, sizeof y);
                Cont_rel(x,y) ;
                break ;
            case BOX_ABS:
                REC (&l, sizeof l);
                REC (&t, sizeof t);
                REC (&r, sizeof r);
                REC (&b, sizeof b);
                Box_abs(l, t, r, b) ;
                break ;
            case BOX_REL:
                REC (&l, sizeof l);
                REC (&t, sizeof t);
                Box_rel(l, t) ;
                break ;
            case ERASE:
                Erase() ;
                break ;
            case GET_LOCATION_WITH_BOX:
                REC (&t, sizeof t);
                REC (&b, sizeof b);
                REC (&x, sizeof x);
                REC (&y, sizeof y);
                Get_location_with_box(t,b,&x,&y,&button) ;
                send(&x,sizeof x) ;
                send(&y,sizeof y) ;
                send(&button,sizeof button) ;
                break ;
            case GET_LOCATION_WITH_LINE:
                REC (&t, sizeof t);
                REC (&b, sizeof b);
                REC (&x, sizeof x);
                REC (&y, sizeof y);
                Get_location_with_line(t,b,&x,&y,&button) ;
                send(&x,sizeof x) ;
                send(&y,sizeof y) ;
                send(&button,sizeof button) ;
                break ;
            case GET_LOCATION_WITH_POINTER:
                REC (&x, sizeof x);
                REC (&y, sizeof y);
                Get_location_with_pointer(&x,&y,&button) ;
                send(&x,sizeof x) ;
                send(&y,sizeof y) ;
                send(&button,sizeof button) ;
                break ;
            case GRAPH_CLOSE:
                Graph_Close() ;
                exit(0) ;
            case LINEMOD:
                REC (&index, sizeof index) ;
                Linemod(index) ;
                break ;
            case MOVE_ABS:
                REC (&x, sizeof x) ;
                REC (&y, sizeof y) ;
                Move_abs(x,y) ;
                break ;
            case MOVE_REL:
                REC (&x, sizeof x) ;
                REC (&y, sizeof y) ;
                Move_rel(x,y);
                break ;
            case RASTER_CHAR:
                REC (&x, sizeof x) ;
                REC (&y, sizeof y) ;
                REC (&index, sizeof index) ;
		blua = (unsigned char *) xalloc (blua, &blu_alloc, x, sizeof (*blua));
                REC (blua, x * sizeof(char)) ;
		if (index !=0) index = 1 ;
		Raster_char(x, y, blua, index, 1) ;
                break ;
            case RASTER_INT:
                REC (&x, sizeof x) ;
                REC (&y, sizeof y) ;
                REC (&index, sizeof index) ;
                xarray = (int *) xalloc (xarray, &n_xarray, x, sizeof (*xarray));
                REC (xarray, x * sizeof (*xarray)) ;
		if (index !=0) index = 1 ;
		Raster_int(x, y, xarray, index, 1) ;
                break ;
            case RGB_RASTER:
                REC (&x, sizeof x) ;
                REC (&y, sizeof y) ;
		reda = (unsigned char *) xalloc (reda, &red_alloc, x, sizeof (*reda));
		grna = (unsigned char *) xalloc (grna, &grn_alloc, x, sizeof (*grna));
		blua = (unsigned char *) xalloc (blua, &blu_alloc, x, sizeof (*blua));
                REC (reda, x * sizeof(char)) ;
                REC (grna, x * sizeof(char)) ;
                REC (blua, x * sizeof(char)) ;
                REC (&t, sizeof t) ;
		RGB_raster(x, y, reda, grna, blua, t) ;
                break ;
            case RGB_COLORS:
		reda = (unsigned char *) xalloc (reda, &red_alloc, 256, sizeof (*reda));
		grna = (unsigned char *) xalloc (grna, &grn_alloc, 256, sizeof (*grna));
		blua = (unsigned char *) xalloc (blua, &blu_alloc, 256, sizeof (*blua));
                REC (reda, 256) ;
                REC (grna, 256) ;
                REC (blua, 256) ;
		Set_RGB_color(reda, grna, blua) ;
		break ;
            case POLYGON_ABS:
                REC (&number, sizeof number) ;
                xarray = (int *) xalloc (xarray, &n_xarray, number, sizeof (*xarray));
                yarray = (int *) xalloc (yarray, &n_yarray, number, sizeof (*yarray));
                REC (xarray, number * sizeof(xarray[0])) ;
                REC (yarray, number * sizeof(yarray[0])) ;
                Polygon_abs(xarray,yarray,number) ;
                break ;
            case POLYGON_REL:
                REC (&number, sizeof number) ;
                xarray = (int *) xalloc (xarray, &n_xarray, number, sizeof (*xarray));
                yarray = (int *) xalloc (yarray, &n_yarray, number, sizeof (*yarray));
                REC (xarray, number * sizeof(xarray[0])) ;
                REC (yarray, number * sizeof(yarray[0])) ;
                Polygon_rel(xarray,yarray,number) ;
                break ;
            case POLYLINE_ABS:
                REC (&number, sizeof number) ;
                xarray = (int *) xalloc (xarray, &n_xarray, number, sizeof (*xarray));
                yarray = (int *) xalloc (yarray, &n_yarray, number, sizeof (*yarray));
                REC (xarray, number * sizeof(xarray[0])) ;
                REC (yarray, number * sizeof(yarray[0])) ;
                Polyline_abs(xarray,yarray,number) ;
                break ;
            case POLYLINE_REL:
                REC (&number, sizeof number) ;
                xarray = (int *) xalloc (xarray, &n_xarray, number, sizeof (*xarray));
                yarray = (int *) xalloc (yarray, &n_yarray, number, sizeof (*yarray));
                REC (xarray, number * sizeof(xarray[0])) ;
                REC (yarray, number * sizeof(yarray[0])) ;
                Polyline_rel(xarray,yarray,number) ;
                break ;
            case POLYDOTS_ABS:
                REC (&number, sizeof number) ;
                xarray = (int *) xalloc (xarray, &n_xarray, number, sizeof (*xarray));
                yarray = (int *) xalloc (yarray, &n_yarray, number, sizeof (*yarray));
                REC (xarray, number * sizeof(xarray[0])) ;
                REC (yarray, number * sizeof(yarray[0])) ;
                Polydots_abs(xarray,yarray,number) ;
                break ;
            case POLYDOTS_REL:
                REC (&number, sizeof number) ;
                xarray = (int *) xalloc (xarray, &n_xarray, number, sizeof (*xarray));
                yarray = (int *) xalloc (yarray, &n_yarray, number, sizeof (*yarray));
                REC (xarray, number * sizeof(xarray[0])) ;
                REC (yarray, number * sizeof(yarray[0])) ;
                Polydots_rel(xarray,yarray,number) ;
                break ;
            case RESET_COLORS:
                REC (&min, sizeof min) ;
                REC (&max, sizeof max) ;
                number = max - min + 1 ;
	        reda = (unsigned char *)
		    xalloc (reda, &red_alloc, number,
		    sizeof (*reda));
	        grna = (unsigned char *)
		    xalloc (grna, &grn_alloc, number,
		    sizeof (*grna));
	        blua = (unsigned char *)
		    xalloc (blua, &blu_alloc, number,
		    sizeof (*blua));
                REC (reda, number * sizeof(char)) ;
                REC (grna, number * sizeof(char)) ;
                REC (blua, number * sizeof(char)) ;
                Reset_colors(min, max, reda, grna, blua) ;
                break ;
            case RESET_COLOR:
                REC (&red, sizeof red) ;
                REC (&grn, sizeof grn) ;
                REC (&blu, sizeof blu) ;
                REC (&number, sizeof number) ;
                Reset_color(red,grn,blu,number) ;
                break ;
            case SCREEN_LEFT:
                Screen_left(&index) ;
                send (&index, sizeof index) ;
                break ;
            case SCREEN_RITE:
                Screen_rite(&index) ;
                send (&index, sizeof index) ;
                break ;
            case SCREEN_BOT:
                Screen_bot(&index) ;
                send (&index, sizeof index) ;
                break ;
            case SCREEN_TOP:
                Screen_top(&index) ;
                send (&index, sizeof index) ;
                break ;
            case SET_WINDOW:
                REC (&t, sizeof t) ;
                REC (&b, sizeof b) ;
                REC (&l, sizeof l) ;
                REC (&r, sizeof r) ;
                Set_window(t, b, l, r) ;
                break ;
            case GET_TEXT_BOX:
                RECTEXT (text) ;
                Get_text_box(text, &t, &b, &l, &r) ;
                send (&t, sizeof t) ;
                send (&b, sizeof b) ;
                send (&l, sizeof l) ;
                send (&r, sizeof r) ;
                break ;
            case FONT:
                RECTEXT (text) ;
                x = Font(text) ;
		send (&x, sizeof x);
                break ;
            case TEXT:
                RECTEXT (text) ;
                Text(text) ;
                break ;
            case TEXT_SIZE:
                REC (&x, sizeof x) ;
                REC (&y, sizeof y) ;
                Text_size(x, y) ;
                break ;
            case TEXT_ROTATION:
                REC (&wx, sizeof wx) ;
                Text_rotation(wx) ;
                break ;
            case PANEL_SAVE:
                RECTEXT (text) ;
                REC (&t, sizeof t) ;
                REC (&b, sizeof b) ;
                REC (&l, sizeof l) ;
                REC (&r, sizeof r) ;
		Panel_save(text, t, b, l, r) ;
                break ;
            case PANEL_RESTORE:
                RECTEXT (text) ;
		Panel_restore(text) ;
                break ;
            case PANEL_DELETE:
                RECTEXT (text) ;
		Panel_delete(text) ;
                break ;
            case PAD_CREATE:
                RECTEXT (text);
                if (*text == 0) /* this is scratch pad */
                    RESULT (OK);
                else if (find_pad (text) != NULL)
                    RESULT (DUPLICATE); /* duplicate pad */
                else if (create_pad (text))
                    RESULT (OK);
                else
                    RESULT (NO_MEMORY);
                break;

            case PAD_CURRENT:
                if (curpad == NULL)
                {
                    RESULT (NO_CUR_PAD);
                    sendtext ("");
                }
                else
                {
                    RESULT (OK);
                    sendtext (curpad->name);
                }
                break;

            case PAD_DELETE:
                if (curpad == NULL)
                    RESULT (NO_CUR_PAD);
                else if (*curpad->name == 0)
                    RESULT (ILLEGAL);
                else
                {
                    delete_pad (curpad);
                    curpad = NULL ;
                    RESULT (OK);
                }
                break;

            case PAD_INVENT:
                invent_pad (text);
                sendtext (text);
                break;

            case PAD_LIST:
                for (pad = padlist; pad != NULL; pad = pad->next)
                    if (*pad->name)
                        sendtext (pad->name);
                sendtext ("");
                break;

            case PAD_SELECT:
                RECTEXT (text) ;    /* pad name */
                curpad = find_pad (text);
                if (curpad == NULL)
                    RESULT (NO_PAD);
                else
                    RESULT (OK);
                break;

            case PAD_GET_ITEM:
                RECTEXT (text); /* item name */
                if (curpad == NULL)
                {
                    RESULT (NO_CUR_PAD);
                    break;
                }
                item = find_item (curpad, text);
                if (item == NULL)
                {
                    RESULT (NO_ITEM);
                    break;
                }
                RESULT (OK);
                for (list = item->list; list != NULL; list = list->next)
                    if (*list->value)
                        sendtext (list->value);
                sendtext("");
                break;

            case PAD_SET_ITEM:
                RECTEXT (name); /* item name */
                RECTEXT (text); /* item value */
                if (curpad == NULL)
                {
                    RESULT (NO_CUR_PAD);
                    break;
                }
                delete_item (curpad, name);
                if(append_item (curpad, name, text))
                    RESULT (OK);
                else
                    RESULT (NO_MEMORY);
                break;

            case PAD_APPEND_ITEM:
                RECTEXT (name); /* item name */
                RECTEXT (text); /* item value */
                if (curpad == NULL)
                {
                    RESULT (NO_CUR_PAD);
                    break;
                }
                if(append_item (curpad, name, text))
                    RESULT (OK);
                else
                    RESULT (NO_MEMORY);
                break;

            case PAD_DELETE_ITEM:
                RECTEXT (text); /* item name */
                if (curpad == NULL)
                {
                    RESULT (NO_CUR_PAD);
                    break;
                }
                delete_item (curpad, text);
                RESULT (OK);
                break;

            case PAD_LIST_ITEMS:
                if (curpad == NULL)
                {
                    RESULT (NO_CUR_PAD);
                    break;
                }
                RESULT (OK);
                for (item = curpad->items; item != NULL; item = item->next)
                    if (*item->name)
                        sendtext (item->name);
                sendtext ("");
                break;

            default:
                fprintf(stderr,"\nUnknown command: %d last: %d", c, lc) ;
                break ;
            }
            lc = c ;
        }
        /* read encountered EOF. close fifos now */
        close (_wfd);
        close (_rfd);
    }
}

static
get_command(c)
    char *c;
{
/* is there a command char pending? */
    if (*c = current_command)
    {
	current_command = 0;
	return 0;
    }

/*
 * look for 1 (or more) COMMAND_ESC chars
 * followed by a non-zero comamnd token char
 */
    while (read1(c) == 0)   /* while !EOF */
    {
	if (*c != COMMAND_ESC)
	    continue;
#ifdef DEBUG
fprintf (stderr, "Got command esc(%d)\n", *c);
#endif DEBUG
	while (*c == COMMAND_ESC)
	    if (read1(c) != 0)
		return 1;		/* EOF */
	if (*c)
	    return 0;		/* got the command token */
    }
    return 1;	/* EOF */
}

static
rec (buf, n)
    char *buf;
{
    int stat;
    while (n-- > 0)
        if ((stat=get1(buf++)) != 0)
	    return stat; /* EOF or COMMAND_ESC */
    return 0;
}

static
rectext (s)
    char *s;
{
    int stat;
    char buf[40];
    debug("rectext");
    stat = _rectext(s);
    sprintf (buf, "  stat=%d\n", stat);
    debug(buf);
    return stat;
}

static
_rectext (s)
    char *s;
{
    int stat;
    while ((stat=get1(s)) == 0)
        if (*s++ == 0) return 0;
    return stat; /* EOF or COMMAND_ESC */
}

static
get1 (c)
    char *c;
{
    debug ("get1");
    if (read1(c) != 0)
    {
	debug (" EOF");
	return 1;	/* EOF */
    }
    if (*c != COMMAND_ESC)
    {
	debug (" OK");
	return 0;	/* OK */
    }
    debug (" COMMAND?");
    if (read1(c) != 0)
    {
	debug (" EOF!");
	return 1;	/* EOF */
    }
    if (*c)
    {
	debug (" YES!");
	current_command = *c;
	return -1;	/* Got command within data */
    }
    debug (" NO!");
    *c = COMMAND_ESC;	/* sequence COMMAND_ESC,0 becomes data COMMAND_ESC */
    return 0;		/* OK */
}

static
read1 (c)
    char *c;
{
    if (atbuf == n_read)
    {
        atbuf = 0;
        n_read = read (_rfd, inbuf, sizeof inbuf);
        if (n_read <= 0) return 1; /* EOF */
    }
    *c = inbuf[atbuf++];
    return 0;
}


static send (buf, n)
    char *buf ;
{
    write (_wfd, buf, n);
}

static sendtext (s)
    char *s;
{
    send (s, strlen(s) + 1);
}

static catch (n)
{
    signal (n, catch);
#ifdef SIGPIPE
    if (n == SIGPIPE)
        broken_pipe = 1;
#endif
}

static
char *
store (s) char *s;
{
    char *buf;

    buf = malloc (strlen(s) + 1);
    if (buf != NULL)
        strcpy (buf, s);
    return buf;
}

static
RESULT(n)
{
    unsigned char c;

    c = n;
    send (&c, 1);
}

/*************** pad routines ************************************/
/*************** pad routines ************************************/


static
create_pad (name)
    char *name;
{
    PAD *pad;

    pad = (PAD *) malloc (sizeof(PAD));
    if (pad == NULL)
        return 0;

    pad->name = store (name);
    if (pad->name == NULL)
    {
        free (pad);
        return 0;
    }

    pad->items = NULL;
    pad->next = padlist ;
    if (pad->next != NULL)
        pad->next->prev = pad ;
    pad->prev = NULL;
    padlist  = pad ;

    return 1;
}

static
delink_pad (pad)
    PAD *pad;
{
    if (pad == NULL) return;

    if (pad->prev == NULL)
        padlist = pad->next;
    else
        pad->prev->next = pad->next;

    if (pad->next != NULL)
        pad->next->prev = pad->prev;
}

static
delete_pad (pad)
    PAD *pad;
{
    ITEM *item,*next;

    if (pad == NULL) return 0;

    delink_pad (pad);

    /* free the items */
    for (item = pad->items; item != NULL; item = next)
    {
        next = item->next;
        free_item (item);
    }
    free (pad);

    return 1;
}

static
PAD *
find_pad (name)
    char *name;
{
    PAD *pad;

    for (pad = padlist; pad != NULL; pad = pad->next)
        if (strcmp (name, pad->name) == 0)
            return pad;
    return (PAD *) NULL;
}

static
invent_pad (name)
    char *name;
{
    static int i = 0;

    do
        sprintf (name, "#%d", ++i);
    while (find_pad (name) != NULL) ;
}

static
migrate_pad (pad)
    PAD *pad;
{
    if (pad == NULL || pad == padlist)
        return;

    /* unlink it from list */
    delink_pad (pad);

    pad->prev = NULL;
    pad->next = padlist;
    padlist = pad;
}

static
append_item (pad, name, value)
    PAD *pad ;
    char *name;
    char *value;
{
    ITEM *item;
    LIST *cur, *prev;
    LIST *list;

    if (pad == NULL)
        return 0;

    /* allocate a list struct and put value into it */
    list = (LIST *) malloc (sizeof(LIST));
    if (list == NULL)
        return 0;
    list->next = NULL;
    list->value = store (value);
    if (list->value == NULL)
    {
        free (list);
        return 0;
    }

    /* find the named item for the current pad */
    item = find_item (pad, name);
    if (item == NULL)
        item = new_item (pad, name);
    if (item == NULL)
        return 0;

    /* add the LIST at the end of the item LIST */
    prev = NULL;
    for (cur = item->list; cur != NULL; cur = cur->next)
        prev = cur;

    if (prev == NULL)
        item->list = list;
    else
        prev->next = list;

    return 1;
}

static
delete_item (pad, name)
    PAD *pad;
    char *name;
{
    ITEM *item;

    item = find_item (pad, name);
    if (item == NULL) return 0;

    if (item->prev == NULL)
        pad->items = item->next;
    else
        item->prev->next = item->next;

    if (item->next != NULL)
        item->next->prev = item->prev;

    /* free the item */
    free_item (item);

    return 1;
}

static
free_item (item)
    ITEM *item;
{
    LIST *list, *next;

    if (item->name != NULL) free (item->name);
    for (list = item->list; list != NULL; list = next)
    {
        next = list->next;
        if (list->value) free (list->value);
        free (list);
    }
    free (item);
}

static
ITEM *
find_item (pad, name)
    PAD *pad;
    char *name;
{
    ITEM *item;

    if (pad != NULL)
        for (item = pad->items; item != NULL; item = item->next)
            if (strcmp (name, item->name) == 0)
                return item;
    return (ITEM *) NULL;
}

static
ITEM *
new_item (pad, name)
    char *name;
    PAD *pad;
{
    ITEM *item;

    item = (ITEM *) malloc (sizeof(ITEM));
    if (item == NULL) return (ITEM *) NULL;

    item->name = store (name);
    if (item->name == NULL)
    {
        free (item);
        return (ITEM *) NULL;
    }

    item->list = NULL;
    item->next = pad->items;
    if (item->next != NULL)
        item->next->prev = item;
    item->prev = NULL;
    pad->items = item ;

    return item;
}

static
int
close_mon()
{
    Graph_Close();
}

static
char *
xalloc (buf, cur, new, len)
    char *buf;
    int *cur;
{
    char *malloc(), *realloc();
    if (*cur >= new)
        return buf;
    if (*cur)
        buf = realloc (buf,new*len);
    else
        buf = malloc (new*len);
    *cur = new;
    if (buf != NULL)
        return buf;
    fprintf (stderr, "%s: Out of Memory\n", me);
    exit(1);
}
