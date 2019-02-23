/* %W% %G% */
#define TELLME(x)	fprintf(stderr,"V_call %d\n",x) ;
#
/***********************************************************************

NAME:		V_call()

FUNCTION:	Interactively allow the user to enter answers into all
			available fields (as previously defined).  
			Answer fields have been created with calls to V_ques()
			Information fields have been created using V_const()
			General text has been created with calls to V_line()

USAGE:		V_call()

PARAMETERS:

RETURNS:	1 user entered ESC to continue
		0 user entered ctrl-C to cancel

ALGORITHM:	
		|	Zero out screen answer locations
		|	Initial curses screens
		|	Display text, constants, and answer fields
		|		Write text      (results from V_line() calls)  to curses window
		|		Write constants (results from V_const() calls) to curses window
		|		Write answers   (results from V_ques() calls)  to curses window
		|	Take commands from the keyboard
		|		switch on commands:
		|			case CR: case NL: case UP: case ESC:
		|				switch on answer type
		|					case string
		|						remove trailing non-alphanumeric characters
		|						copy answer to target denoted in V_ques() call
		|						blank out screen line
		|						copy target to curses window
		|					case integer
		|						run atoi on answer, putting results in target 
		|							denoted in V_ques() call
		|						blank out screen line
		|						printf target to curses window
		|					case long
		|						run atol on answer, putting results in target 
		|							denoted in V_ques() call
		|						blank out screen line
		|						printf target to curses window
		|					case float, double
		|						run sscanf on answer, putting results in target 
		|							denoted in V_ques() call
		|						blank out screen line
		|						printf target to curses window
		|					default:
		|						do nothing
		|				if ESC return from V_call()
		|				if UP  shift to previous question
		|				if CR or NL  shift to next question
		|			case BS:   Move cursor back one column in current question
		|			case FS:   Move cursor forward one column in current 
		|				question
		|			case RPLT: Replot the current screen image
		|			case DUMP: Dump (append) the current window to the user's 
		|				home dir.
		|			default:   If an alphanumeric, put that char on the screen 
		|				and in the current answer field
		|	call V_exit  (erase screen and exit curses)

CALLS:		
		V_init()        Vask routine to initialize curses
		V_exit()        Vask routine to exit curses
		V__dump_window()   V_ask
		V__remove_trail()   V_ask
		addch()         curses routine
		addstr()        curses routine
		move()          curses routine
		putc()          curses routine
		refresh()       curses routine
		getch()         curses routine
		wrefresh()      curses routine
		sprintf()       UNIX
		strcpy()        UNIX

***********************************************************************/

#include "vask.h"

/* define the V__ struct defined in vask.h */
struct V__ V__ ;

#define DUMP	001
#define BELL	007
#define BS		010
#define FS		014
#define NL		012
#define UP		013
#define CR		015
#define RPLT	022
#define ESC		033
#define CTRLC		003

#define TARGET	V__.usr_answ[at_answer].targetptr
#define ROW		V__.usr_answ[at_answer].row
#define COL		V__.usr_answ[at_answer].col
#define LENGTH	V__.usr_answ[at_answer].length
#define TYPE	V__.usr_answ[at_answer].var_type
#define ANSWER	scr_answ[at_answer].position
#define RELINE	move(ROW, COL) ;                       \
				for (incr2=0;incr2<LENGTH; incr2++)    \
					addch('_')  ;                      \
				move(ROW, COL) 

/* flag ctrl-c is to be allowed */

static int interrupts_ok = 0;			/* mod shapiro */

V_call() 
{
    int incr ;
    int incr2 ;
    int num_answers ;
    int at_answer   ;
    int at_constant ;
    int ans_col     ;
    char newchar    ;
    int c;
    struct { char position[80]; } scr_answ[MAX_ANSW] ;
    long atol() ;
    int y,x;		/* shapiro */
    char temp[100];
    char *fmt();
    char *f;


/* Zero out screen answer locations */
    for(incr=0; incr<24; incr++)
	    for(incr2=0; incr2<80; incr2++)
		    scr_answ[incr].position[incr2] = 000 ;

/* Initialize the curses windows */
    V_init() ;

/* Display text              */
    for (incr=0; incr<MAX_LINE; incr++) 
    {
	move (incr, 0) ;
	addstr(V__.page.line[incr]) ;
    }

/* Display constants   */
    for (at_constant=0; at_constant < V__.NUM_CONST; at_constant++) 
    {
	move(V__.constant[at_constant].row, V__.constant[at_constant].col) ;
	switch (V__.constant[at_constant].var_type) 
	{
	case 's':
		addstr(V__.constant[at_constant].targetptr.c) ;
		break ;
	case 'i':
		sprintf(temp,"%d", *V__.constant[at_constant].targetptr.i) ;
		addstr (temp) ;
		break ;
	case 'l':
		sprintf(temp,"%ld", *V__.constant[at_constant].targetptr.l) ;
		addstr (temp) ;
		break ;
	case 'f':
		f = fmt ("f", V__.constant[at_constant].decimal_places);
		sprintf(temp, f, *V__.constant[at_constant].targetptr.f) ;
		addstr (temp) ;
		break ;
	case 'd':
		f = fmt ("lf", V__.constant[at_constant].decimal_places);
		sprintf(temp, f, *V__.constant[at_constant].targetptr.d) ;
		addstr (temp) ;
		break ;
	default:
		break ;
	}
    }

/* Display answer locations  */
    for (at_answer=0; at_answer < V__.NUM_ANSW; at_answer++) 
    {
	/* clear ANSWER */
	for (incr=0; incr<80; incr++)
	    scr_answ[at_answer].position[incr] = 000 ;

	switch (TYPE) 
	{
	case 's':
		strcpy(ANSWER, TARGET.c) ;
		RELINE ;
		for (incr=0; incr<LENGTH; incr++) 
		{
		    if ( *(TARGET.c + incr) == '\000')
			while (incr++ < LENGTH) 
			    addch('_') ;
		    else addch( *(TARGET.c + incr) ) ;
		}
		break ;
	case 'i':
		sprintf(ANSWER,"%d",*TARGET.i) ;
		RELINE ;
		addstr (ANSWER);
		break ;
	case 'l':
		sprintf(ANSWER,"%ld",*TARGET.l) ;
		RELINE ;
		addstr (ANSWER);
		break ;
	case 'f':
		f = fmt ("f", V__.usr_answ[at_answer].decimal_places);
		sprintf(ANSWER,f,*TARGET.f) ;
		RELINE ;
		addstr (ANSWER);
		break ;
	case 'd':
		f = fmt ("lf", V__.usr_answ[at_answer].decimal_places);
		sprintf(ANSWER,f,*TARGET.d) ;
		RELINE ;
		addstr (ANSWER);
		break ;
	default:
		break ;
	}
    }
    num_answers = at_answer ;
    if (interrupts_ok)
	    move(22,0) ;
    else
	    move(23,0) ;
    centered("AFTER COMPLETING ALL ANSWERS, HIT <ESC> TO CONTINUE") ;
    if (interrupts_ok)
    {
	sprintf(temp,"(OR <Ctrl-C> TO %s)", V__.interrupt_msg);
	centered(temp);
    }

/* Begin taking commands/answers from terminal */
    at_answer = 0 ;
    ans_col  = 0 ;

    move(ROW, COL) ;
    refresh() ;

    while(1)
    {
	getyx (stdscr, y, x);

	c = getch() & 0177;
	if(c == EOF)
	{
	    move (y, x);
	    refresh();
	    continue;
	}
	if (c == CTRLC && interrupts_ok == 0)
	{
	    move (y, x);
	    refresh();
	    continue;
	}
	if (V__.NUM_ANSW <= 0)
	{
	    if (c == ESC || c == CTRLC)
	    {
		interrupts_ok = 0;
		V_exit() ;
		return(c == ESC) ;
	    }
	    move (y, x);
	    refresh();
	    continue;
	}
	newchar = c;
	switch (newchar)  
	{
	case CR: case NL: case UP: case ESC: case CTRLC:
		switch (TYPE) 
		{
		case 's':
			ans_col = (LENGTH) ; 
			while((ANSWER[--ans_col] < 041) || (ANSWER[ans_col] > 0175));
			V__remove_trail(LENGTH, ANSWER) ;
/*					ANSWER[++ans_col] = 000 ; */
			strcpy(TARGET.c, ANSWER) ;
			RELINE ;
			addstr(TARGET.c) ;
			break ;
		case 'i':
			*TARGET.i = atoi(ANSWER) ;
			RELINE ;
			sprintf(temp,"%d", *TARGET.i) ;
			addstr (temp) ;
			break ;
		case 'l':
			*TARGET.l = atol(ANSWER) ;
			RELINE ;
			sprintf(temp,"%ld", *TARGET.l) ;
			addstr (temp) ;
			break ;
		case 'f':
			sscanf (ANSWER,"%f",TARGET.f);
			RELINE ;
			f = fmt ("f", V__.usr_answ[at_answer].decimal_places);
			sprintf(ANSWER, f, *TARGET.f) ;
			sscanf (ANSWER,"%f",TARGET.f);
			addstr (ANSWER) ;
			break ;
		case 'd':
			sscanf (ANSWER,"%lf",TARGET.d);
			RELINE ;
			f = fmt ("lf", V__.usr_answ[at_answer].decimal_places);
			sprintf(ANSWER, f, *TARGET.d) ;
			sscanf (ANSWER,"%lf",TARGET.d);
			addstr (ANSWER) ;
			break ;
		default:
			break ;
		}
		if (newchar == ESC || newchar == CTRLC) 
		{ 
		    interrupts_ok = 0;
		    V_exit() ;
		    return(c == ESC) ;
		}
		if (newchar == UP)
		    at_answer = (at_answer+num_answers-1) % num_answers ;
		else
		    at_answer = (at_answer+1) % num_answers ;
		ans_col  = 0 ;
		move(ROW, COL) ;
		refresh() ;
		break ;

	case BS:
		ans_col = (ans_col-1 >= 0) ? ans_col-1 : 0 ;
		move(ROW, COL + ans_col) ;
		refresh() ;
		break ;

	case FS:
		if (ANSWER[ans_col+1] != NULL)
		{
		    ans_col = (ans_col+1 <= LENGTH) ? ans_col+1 : LENGTH ;
		    move(ROW, COL + ans_col) ;
		    refresh() ;
		}
		else
		    putc(BELL, stderr) ;
		break ;

	case RPLT:
		getyx (stdscr, y, x);		/* shapiro */
		wrefresh(curscr) ;
		move (y, x);		/* shapiro */
		refresh();		/* shapiro */
		break ;

	case DUMP:
		V__dump_window() ;
		break ;

	default:
		if (ans_col >= LENGTH) 
		    break ;
		if ((newchar >= '\040') && (newchar < '\176')) 
		{
		    addch(newchar) ;
		    ANSWER[ans_col] = newchar ;
		    ans_col++ ;
		    refresh() ;
		}
		break ;
	} 
    } 
}
V_intrpt_ok()
{
    interrupts_ok = 1;		/* will be set false when V_call() exists */
}
V_intrpt_msg (msg)
    char *msg;
{
    strcpy (V__.interrupt_msg, msg);
}


static char *
fmt(s,n)
    char *s;
{
    static char buf[20];

    if (n > 0)
	sprintf (buf, "%%.%d%s", n, s);
    else
	sprintf (buf, "%%.0%s", s);
    return buf;
}

static
centered(msg)
    char *msg;
{
    int indent;

    indent = (80 - strlen(msg))/2;
    while (indent-- > 0)
	addstr(" ");
    addstr(msg);
    addstr("\n");
}
