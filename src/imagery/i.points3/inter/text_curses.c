/*=======================================================================
				i.points
  curses.c --
    All whole bunch of routines that deal with writing to the ascii
    terminal.  All taken from the original i.points program.

    In the ascii terminal we will setup the following WINDOWS

        +--------------------------+---------------------------+
        +                          +                           +
        +                          +                           +
        +                          +                           +
        +                          +                           +
        +      INFO_WINDOW         +        PROPMT_WINDOW      +
        +                          +                           +
        +                          +                           +
        +                          +                           +
        +                          +                           +
        +--------------------------+---------------------------+
        +      MENU_WINDOW                                     +
        +--------------------------+---------------------------+


    Begin_curses (void)
        Sets up the windows.
    End_curses (void)
        Shuts them down on exit.

    Suspend_curses (void)
        Suspend the windowing so that we can return to normal for
	any text enter (i.e. V_ask routines).

    Resume_curses (void)
        Resuses curses after a Suspend_curses call.  Uses the static
	variable WINDOW *save to restore.

    Curses_allow_interrupts (boolean true)

    Curses_clear_window (Window *window)
        Clear any text from the window.
    Curses_outline_window (Window *window)
        Draw the +'s and -'s that outline the window

    Curses_write_window (Window *window, int line, int col, char *message)
        Write the message into the window.
	line and col are relative to the WINDOW ??

    Curses_replot_screen(void)
        Replot everything on the ascii terminal ??

    Curses_propmt_gets (char *prompt, char *answer)
        Prompt the users and get an answer

    Beep (void)
        Beep to let user know somethings up

    Curses_getch (boolean with_echo)
        Get a single char from user.  Either with_echo or not 
	alows character echoing to terminal.
    
=======================================================================*/

#include "globals.h"

/*---------------------------------------------------------------------*/

         /* internal function prototypes */
#ifdef _NO_PROTO
   static Window * make_window ();
   WINDOW *newwin();
#else
   static Window * make_window (int top, int bottom, int left, int right);
   WINDOW *newwin();
#endif

/*---------------------------------------------------------------------*/

static int inited = 0;       /* set == 1 after Begin_curses  */
                             /* most of the Curses routines just return */
                             /* if inited != 1 */
static WINDOW *save;         /* used in suspend_curses() and */ 
			     /*	resume_curses() */

/*---------------------------------------------------------------------*/
Begin_curses()
{
/* should only be called once at program outset */

    Get_old_tty() ;    /* get current tty state                */

    initscr () ;       /* initialize curses standard screens   */
    raw() ;            /* set tty modes via curses calls       */
    noecho() ;
    nonl()   ;

    Get_new_tty() ;    /* get the tty state as set by curses   */

    inited = 1;

/* make a window to save stdscr */
    save = newwin(LINES,COLS,0,0);

/* make_window (nrows, ncols, start_row, start_col) */
    INFO_WINDOW   = make_window (0, LINES-4, COLS/2, COLS-1);
    MENU_WINDOW   = make_window (0, LINES-4, 0, COLS/2);
    PROMPT_WINDOW = make_window (LINES-4, LINES-1, 0, COLS-1);

    refresh();

}

/*---------------------------------------------------------------------*/
End_curses()
{
/* should only be called upon program exit */

    clear() ;       /* clear the screen */
    refresh() ;
    endwin() ;      /* let curses reset the tty now */
}

/*---------------------------------------------------------------------*/
Suspend_curses()
{
    overwrite (stdscr, save);
    clear();
    refresh();

    Old_tty();
}

/*---------------------------------------------------------------------*/
Resume_curses()
{
    New_tty();
    clear();
    refresh();
    overwrite (save, stdscr);
    refresh();
}

/*---------------------------------------------------------------------*/
Curses_allow_interrupts (true)
{
    refresh();
    if (true)
	noraw();
    else
	raw();
}

/*---------------------------------------------------------------------*/
Curses_clear_window(window)
    Window *window;
{
    int y,x;

if (!inited) return;
    for (y = window->top+1; y < window->bottom; y++)
    {
	move (y, x=window->left+1);
	while (x++ < window->right)
	    addch (' ');
    }
    Curses_outline_window (window);
    refresh();
}

/*---------------------------------------------------------------------*/
Curses_outline_window (window)
    Window *window;
{
    int x, y;

    move (window->top, x=window->left+1);
    while (x++ < window->right)
	addch ('-');
    move (window->bottom, x=window->left+1);
    while (x++ < window->right)
	addch ('-');
    for (y=window->top+1; y < window->bottom; y++)
    {
	move (y, window->left);
	addch ('|');
	move (y, window->right);
	addch ('|');
    }
    move (window->top, window->left);
    addch ('+');
    move (window->top, window->right);
    addch ('+');
    move (window->bottom, window->left);
    addch ('+');
    if (window->bottom < LINES-1 || window->right < COLS-1)
    {
	move (window->bottom, window->right);
	addch ('+');
    }
}

/*---------------------------------------------------------------------*/
Curses_write_window(window, line, col, message)
    Window *window;
    int line ;
    char *message ;
{
    int y,x,i;

if (!inited)
{
	fprintf (stderr, "%s\n", message);
	return;
}
    if (line <= 0 || line >= window->bottom-window->top)
	return;
    if (col <= 0 || col >= window->right-window->left)
	return;
    move(y=window->top+line, x=window->left+col);
    while (*message != 0 && *message != '\n' && x < window->right)
    {
	addch (*message);
	message++;
	x++;
    }
    if (*message == '\n')
	for (i = x; i < window->right; i++)
	    addch (' ');
    move (y, x);
    refresh();
}


/*---------------------------------------------------------------------*/
Curses_replot_screen() 
{
    int x,y;
    getyx (stdscr, y, x);
    wrefresh(curscr) ;
    move (y, x);
    refresh();
}

/*---------------------------------------------------------------------*/
Curses_prompt_gets (prompt, answer)
    char *prompt ;
    char *answer ;
{
    char c ;
    int n;
    int y,x;

    *answer = 0;
    n = 0;

    Curses_write_window (PROMPT_WINDOW, 1, 1, "\n");
    Curses_write_window (PROMPT_WINDOW, 1, 1, prompt);

    for(;;)
    {
	refresh ();
	c = Curses_getch(0) ;
	if (c == '\n' || c == '\r')
	    break;

	getyx (stdscr, y, x);
	if (c > 037 && c < 0177)
	{
	    if (x < PROMPT_WINDOW->right)
	    {
		*answer++ = c ;
		*answer = 0 ;
		addch(c) ;
		n++;
	    }
	    continue;
	}
	if (c == '\b')
	{
	    if (n > 0)
	    {
		answer--;
		*answer = 0;
		move (y, x-1);
		addch (' ');
		move (y, x-1);
		n--;
	    }
	    continue;
	}
	Beep();
    }
}

/*---------------------------------------------------------------------*/
Beep()
{
    putchar ('\7');
    fflush (stdout);
}

/*---------------------------------------------------------------------*/
Curses_getch(with_echo)
{
    char achar;
    int c;
    int kill;

if (!inited) return 0;
    kill = 0;
    while(1)
    {
	c = getch() & 0177;
	if (c == interrupt_char)
	{
	    if (kill++ >= 3)
	    {
		End_curses();
		exit(0);
	    }
	    continue;
	}
	kill = 0;
	if (c != 18)
	    break;
	Curses_replot_screen();
    }
    if (with_echo)
    {
	achar = c;
	addch(achar);
	refresh();
    }
    return c;
}


/*---------------------------------------------------------------------*/
static
Window *
make_window (top, bottom, left, right)
{
    Window *window;

    if (top < 0 || bottom >= LINES || left < 0 || right >= COLS
    ||	bottom-top <= 1 || right-left <= 1)
    {
	End_curses();
	fprintf (stderr, "make_window(%d,%d,%d,%d): illegal screen values\n",
		top, bottom, left, right);
	sleep(3);
	exit(1);
    }
    window = (Window *) G_malloc (sizeof(Window));
    window->top    = top;
    window->bottom = bottom;
    window->left   = left;
    window->right  = right;
    Curses_clear_window (window);
    return window;
}
