

/*	Altek - diagnostics program. 
*	this is for binary format  six characters.
*
*	Cable used is straight thru on the 3B2.
*	Any command down loaded (written ) to the digitizer that takes an
*	argument must have a '\r' following the argument. Eg: "M7\r"
*	Commands of only a single character don`t use the '\r' . Eg: "P"
*/

/*  SB (Status byte):  1 - button down,  0 - button up  */
/*  FC (Flag Char.):  0-9,A-F   cursor button hit  */
/*  PR (Proximity):  1 - out of proximity,  0 - in proximity  */ 
/*  HP (host point requested by host with 'V' command) -
*		1 - if cursor is off the table when point is requested.
*/

/*  Altek will still send output even if the cursor is outside the 
*  the tablet range.  have to check the PR bit  
*/

/*	There are two ways that a coordinate can be sent by the cursor.
*	1)  When a set of coordinates is asked for with the V command.
*	2)	When a cursor key is hit
*
*	If the cursor is off the tablet and a coordinate is asked for; 
*	a set of coordinates will be sent with PR true (out of proximity) .
*
*	If cursor is on tablet:
*		Cursor key NOT hit, but coor. asked for.
*			PR = false
*			SB = false  button not down (up)
*			HP = true   point asked for
*			FC = true   but useless, because no key was hit
*
*	If cursor is on tablet:
*		Cursor key hit
*			PR = false
*			SB = true   button down (pressed)
*			HP = false  point NOT asked for
*			FC = true   contains number of cursor key hit
*
*/

#include	<stdio.h>
#include	<signal.h>

#include	<fcntl.h>

#define	CHARS_RD	6
#define	ALARM_TIME	5

static	int     IORser;
static	int		N_read ;

static	char    InBuffer[31] ;

main ()
{
	int		i ;
	int		SB ;
	int		HP ;
	int		FC ;
	int		PR ;
	int	X,	Y ;
	int	x,	y ;		/*  old X, and Y */

	long	good_r;		/*  no. of good reads  */
	long	bad_r;
	long	no_resp;

	char	buf[60] ;
	char	tty[60] ;

	int  close_down() ;

	signal(SIGINT, close_down ) ;

	for (i=0; i<30; i++)
		InBuffer[i] = NULL ;


	setbuf (stdout, NULL) ;

	fprintf (stdout,"\n this is set to 9600 baud\n") ;

	prompt ( tty, "  Which tty is the digitizer connected to (ttya):  ") ;
	fprintf (stdout,"\n Hit interrupt or break key to stop.\n\n") ;

	if (*tty == NULL)
		sprintf ( buf, "/dev/ttya") ;
	else
		sprintf ( buf, "/dev/%s", tty) ;


	fprintf (stdout,"\n Now opening digitizer on %s\n", buf) ;

	if (D_open_serial (buf) < 0)
		exit (-1);

	fprintf (stdout,"\n\n\n\n       ALTEK DIGITIZER:  binary format 6 characters.\n\n\n\n\n") ;

	D_init_digit() ;
	set_resolution(buf) ;

	fprintf (stdout,"\n\n\n\n") ;

	good_r = bad_r = no_resp = 0 ;


	fprintf (stdout,"     X           Y        Key #  reads:   good       bad     none  Nread \n") ;

	D_flush() ;

while(1)
{
	/*  ask for a coordinate   */
	D_write_digit ("V") ;



	if (D__read () < 0)		/*  no response  */
	{
		++no_resp ;
		fprintf (stdout,"\r   %35s  no response:  %ld  ", "", no_resp) ;
		continue ;
	}

	if ( (N_read<0) || N_read != CHARS_RD)
	 {
		++bad_r ;
		fprintf (stdout,"\r   %35s  bad read:  %ld           N_read:   %d  ",
				" flushing input", bad_r, N_read) ;
		sleep(2) ;
		D_flush() ;
		continue ;
	 }
			

	/*  binary standard format   */


	SB =   (int)(InBuffer[0] & 0x40) ;
	PR =   (int)(InBuffer[3] & 0x20) ;
	FC =   (int)( (InBuffer[0] & 0x3c) >> 2) ;
	HP =   (int)(InBuffer[3] & 0x10) ;

	if (PR)
	 {
		fprintf (stdout,"\r%70s", "  cursor off the tablet           ") ;
		continue ;
	 }

	/****
	if (XX)
	 {
		++bad_r ;
		fprintf (stdout,"\r  %35s  bad read:  %ld  N_read: %d", "",
			bad_r, N_read ) ;
		continue ;
	 }
	****/


	X =    ((int) (InBuffer[3] & 0x08))  << 16
		|  ((int) (InBuffer[0] & 0x3))  << 14
		|  ((int) (InBuffer[1] & 0x7f))  << 7
		|  ((int) (InBuffer[2] & 0x7f)) ;


	Y =    ((int) (InBuffer[3] & 0x7))  << 14
		|  ((int) (InBuffer[4] & 0x7f))  << 7
		|  ((int) (InBuffer[5] & 0x7f)) ;

	/*  button not down, don`t use   */
	if (!SB)
		FC = -1 ;

	++good_r ;

	/*****
	if ( (X == x)  &&  (Y == y))
	{
		fprintf (stdout,"\r %25s %d    same point  ", "", FC) ;
		continue ;
	}
	*****/


	fprintf (stdout,"\r    %d       %d      %d              %ld,       %ld        %ld     %d  ",
	X, Y, FC, good_r, bad_r, no_resp,  N_read) ;

/*************
	if(HP)
		fprintf (stdout," HP  ") ;
	else
		fprintf (stdout," hp  ") ;
*************/

	x = X ;     y = Y ;

}

	if (D_end_digit () < 0)
		exit (-1);


}	/*  main()  */


set_resolution( buf)
	char  *buf ;
{
	/*  there are actually 7 possible resolutions for this digitizer  */

	prompt ( buf, "  High or low resolution, default is high. (h/l):  ") ;

	if ( buf[0] == 'l')
	 {
		D_write_digit ("M5\r") ;		/*  low resolution  */
		fprintf (stdout,"\n  digitizer to low resolution.") ;
	 }
	else
	 {
		D_write_digit ("M1\r") ;		/*  high resolution  */
		fprintf (stdout,"\n  digitizer to high resolution.") ;
	 }

}

prompt( buf, p_string)
	char	*buf, *p_string ;
{
	fprintf (stdout,"%s", p_string) ;
	*buf = NULL ;
	if (gets(buf) == NULL)
	{
		clearerr(stdin) ;
		exit(0) ;
	}
}

delay(n) /* delay n milliseconds */
	int  n ;
{
	char zero;
	int i;

	zero = 0;

/* this assumes 9600 baud to stderr */
	while (n-- > 0)
		for (i = 0; i < 10; i++)
			write (2, &zero, 1);
}

close_down()
{
	signal( SIGALRM, SIG_DFL) ;

	fprintf (stdout," closing tty ... ") ;
	D_end_digit() ;

	fprintf (stdout,"  and exiting \n\n") ;
	exit(0) ;
}


/* device driver code below this point  */


#include	<sgtty.h>


D_open_serial (tty_name)
char   *tty_name;
{


	int		disc ;
	struct	sgttyb	sgttyb ;
	struct	tchars	tchars;
	struct	ltchars	ltchars;


	if ((IORser = open (tty_name, 2, 0)) < 0)
	{
		perror ("Could not open device");
		return (-1);
	}
 /* 
  * get the tty structures
  */

	if (ioctl (IORser, TIOCGETP, &sgttyb) < 0)
		perror ("ioctl failed on device");

 /* 
  * go into RAW mode using OLD tty interface
  */

	disc = OTTYDISC ;
	sgttyb.sg_ispeed = sgttyb.sg_ospeed = B9600;
	sgttyb.sg_flags = RAW;



 /* 
  * now set the modes and flags 
  */

	if (ioctl (IORser, TIOCSETP, &sgttyb) < 0)
		perror ("ioctl could not set parameters");
	if (ioctl (IORser, TIOCSETD, &disc) < 0)
		perror ("ioctl could not set parameters");


	return (0);
}



D_open_failed ()
{
	signal( SIGALRM, SIG_DFL) ;
	fprintf(stderr, "\n ERROR: Could not open digitizer\n") ;
	exit(-1) ;
}


D_end_digit ()
{
	D_write_digit ("") ;			/*  reset digitizer  */
	close (IORser);
}



#include	<setjmp.h>


static	jmp_buf		jmp_alarm;

int  D__read ()
{

	/***
	reads number of characters from the file ; if it can't read within
	five seconds it returns error status.
	**/

	int  timedout ;
	int  len ;

	char  *cp ;

	int	jtimeout();		/*  uses jum_buf  */

/* when setjmp() is called directly, it will return 0 (FALSE)
   when the longjmp() is called in jtimeout() it will appear that
   we have just executed the setjmp() call, but setjmp() will return
   1 (TRUE) since this is what we told longjmp() to ask setjmp()
   to return.
*/

	
/*  When a SIGALRM signal is sent  jtimeout will be executed.

    Set the alarm to five seconds before the read.  If the read executes before
    the five seconds is up then the alarm is reset to zero which nullifies the 
    alarm.  If the read doesn't occur before the five sec. ,  then alarm will 
    send the signal and jtimeout will be executed.
*/


	if (timedout = setjmp (jmp_alarm))
	{
		signal (SIGALRM, SIG_DFL);			/*  timed out, reset  */
		return(-1) ;
	}
	else
		signal (SIGALRM, jtimeout);


	alarm (ALARM_TIME);

	cp = InBuffer ;
	len = CHARS_RD ;
	while (len > 0)
	{
		N_read = read (IORser, cp, len );	
		if (N_read < 0)
		{
			fprintf (stderr, "Digitizer read error\n");
			continue;
		}

		cp += N_read ;
		len -= N_read ;
/***
fprintf( stderr, "\n len: %d, N_read: %d\n", len, N_read) ;
***/

	}

	alarm (0);
	signal (SIGALRM, SIG_DFL);


	if (N_read < 0)				  /*  did it timeout  */
		return (-1);

		/*  strip parity  */
	for ( cp = InBuffer;  cp < InBuffer + CHARS_RD;  cp++)
		*cp &= 0177;

	return (N_read = CHARS_RD - len);

}	/*  end of r_read ()  */



jtimeout ()
{
	longjmp (jmp_alarm, 1);
}


D_init_digit()
{
		D_write_digit ("") ;			/*  reset digitizer  */

		/*	 set output mode  */
		D_write_digit ("F8\r") ;
		D_write_digit ("P") ;			/*  set point mode  */
}

D_write_digit( string)
	char	*string ;
{
	int	str_length ;
	int count;

	str_length = strlen (string) ;

	if (write (IORser, string, str_length) != str_length)
	{
		fprintf( stderr, "\n Could not write string:\n'%s'\n to digitizer.\n",
			string	) ;	
		exit(-1) ;
	}
    while (1)
    {
	if (ioctl(IORser, TIOCOUTQ, &count) < 0)
	{
		fprintf (stderr, "ioctl error on digitizer\n");
		break;
	}
	if (count <= 0) break;
/********
	fprintf (stderr, "%d chars still in output queue\n",count);
********/
    }
    delay (15);
	return(0) ;
}


#include	<sys/file.h>
D_flush()
{
	int  i ;

/*  flush digitizer (input) */

	ioctl (IORser, TIOCFLUSH, FREAD) ;

/******
System V flush
	ioctl (IORser, TCFLSH, 0) ;
******/

	return(0) ;
}

