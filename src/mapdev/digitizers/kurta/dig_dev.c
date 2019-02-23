/*  @(#)dig_dev.c	1.1  5/4/87  */

#include <stdio.h>
#include <signal.h>
#include <setjmp.h>

#ifdef  USE_TERMIO
#include <termio.h>
#include <fcntl.h>
#endif

#ifndef USE_TERMIO
#include <sgtty.h>
#include <sys/file.h>
#endif

#define	CHARS_RD	6
#define ALARM_TIME	3
#define TRIES		10

static char    InBuffer[200];

static float Scale ;


/*  digitizer device variables  */

static int	IORser;
static int	N_read;



/*  Digitizer initialization -
*	set digitizer resolution,
*	set program scale.
*/
D_digit_init()
{


	/*  reset  */
	D_write_digit ("5") ;
	delay(5) ;

	D_write_digit ("F") ;		/*  resolution to .001 of a inch  */
	Scale = .001;
	delay(5) ;

	D_flush() ;

}




D_readall (Xraw, Yraw)
	int     *Xraw;
	int     *Yraw;
{
	int     len, tries, no_response ;
	int     a;
	int     K;
	int     S;
	char	debug[90] ;

	no_response = 0 ;
	tries = 0;
	N_read = 0;


	while (tries < TRIES)
	{

	/* request data */
		D_write_digit ("2") ;

#ifdef	DEBUG
	sprintf( debug, "   tries: %d,  N_read: %d,  no_response: %d",
			tries, N_read, no_response) ;
	Write_base( 12, debug) ;
#endif


	/*  no response  */
		if ( D__read(ALARM_TIME) < 0)
			continue ;


	/*  are we still in sync,  first bit should be on  */
	if (  ! (InBuffer[0] & 0x80) )
	 {
		fprintf( stderr, "HOLD:  Digitizer is out of sync\n") ;
		fprintf( stderr, " Trying to recover\n") ;
		D_flush() ;
		D_digit_init() ;
		fprintf( stderr, " Recovered.  Continue\n") ;
		++tries ;
		continue ;
	 }


	/*  K is which key is hit 0-15  */
		K =   (int)((InBuffer[0] & 0x3c)  >> 2) ;


		if ( N_read < 0  ||  N_read != CHARS_RD ||  K<0)
		{
			++tries ;
fprintf( stderr, "Digitizer read error. N_read: %d \n", N_read) ;
			D_flush() ;
			continue ;
		}
		else
			break ;
	}


	if (tries >= TRIES)
	{
		sprintf (debug,"tries %d, no_response %d", tries, no_response);
		Write_base (8,debug);
		return (-1);
	}



	/*  output format 2  */

	/*  S is true if cursor key was hit  */
		if ((S =  (int)(InBuffer[0] & 0x40)))
			++K ;			/*  0-15 make range 1-16  */
		else
			K = 0 ;

	*Xraw =    ((int)(InBuffer[0] & 0x3))  << 14
		|  ((int)(InBuffer[1] & 0x7f))  << 7
		|  ((int)(InBuffer[2] & 0x7f)) ;


	*Yraw =    ((int)(InBuffer[3] & 0x3))  << 14
		|  ((int)(InBuffer[4] & 0x7f))  << 7
		|  ((int)(InBuffer[5] & 0x7f)) ;

#ifdef	DEBUG
	sprintf( debug, "   X: %d,  Y: %d", *Xraw, *Yraw) ;
	Write_base( 13, debug) ;
#endif


/*  return -1 on read error,
*	return 0 on succesful read with no key cursor key hit
*	return # of cursor key hit 1-16
*/

	return(K) ;

}


D_ask_if_err()
{
	if ( ! curses_yes_no( 2, " Digitizer read error. Do we continue(y,n)? ") )
	close_down(-1) ;
}

D_get_scale(scale)
	float *scale ;
{
	*scale = Scale ;
}


D_end_digit ()
{

	close (IORser);
}


static	jmp_buf		jmp_alarm;

int  D__read (alarm_time)
	int  alarm_time ;
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


	alarm (alarm_time);

	cp = InBuffer ;
	len = CHARS_RD ;
	while (len > 0)
	{
		N_read = read (IORser, cp, len);	
		cp += N_read ;
		len -= N_read ;
	}

	alarm (0);
	signal (SIGALRM, SIG_DFL);


	if (N_read < 0)					  /*  did it timeout  */
		return (-1);


		/*  strip parity  */
/**  we are checking the high byte for sync.  don't strip
	for ( cp = InBuffer;  cp < InBuffer + N_read;  cp++)
		*cp &= 0177;
*******/

	return (N_read = CHARS_RD - len);

}	/*  end of D__read ()  */



jtimeout ()
{
	longjmp (jmp_alarm, 1);
}

D_open_failed()
{

	alarm(0) ;
	signal( SIGALRM, SIG_DFL) ;
	delay(99999) ;
	fprintf(stderr, "\n Could not open digitizer...\n") ;

	close_down(-1) ;

}

D_write_digit( string)
	char	*string ;
{
	int	str_length ;

	str_length = strlen (string) ;

	if (write (IORser, string, str_length) != str_length)
	{
		fprintf( stderr, "\n Could not write string:\n'%s'\n to digitizer.\n",
			string	) ;	
		exit(-1) ;
	}
	return(0) ;
}



/*****    ALL CODE BEYOND THIS POINT IS SYSTEM DEPENDANT     ****/

/*   D_flush() and D_open_serial()   */


D_flush()
{

/*  flush digitizer (input) */



#ifdef  USE_TERMIO
	ioctl (IORser, TCFLSH, 0) ;
#else
	ioctl (IORser, TIOCFLUSH, 0) ;
	/*  changed FREAD to 0  */
#endif

	return(0) ;
}



/*    system V code for D_open_serial() begins  */

#ifdef  USE_TERMIO

struct	termio	termio ;

D_open_serial (tty_name)
char   *tty_name;
{

	int	D_open_failed() ;
	int	(*alarm_set)() ;

	alarm_set = signal(SIGALRM, D_open_failed) ;
	alarm(5) ;

	if ((IORser = open (tty_name, O_RDWR, 0)) < 0)
		D_open_failed() ;

	alarm(0) ;
	signal(SIGALRM, alarm_set) ;
 /* 
  * get the tty structure
  */

	if (ioctl (IORser, TCGETA, &termio) < 0)
		perror ("ioctl failed on device");

 /* 
  *  change the settings
  */

	termio.c_iflag = IGNBRK ;
	termio.c_oflag = 0 ;
	termio.c_cflag = B9600 | CS8 | CREAD| HUPCL | CLOCAL ;
	termio.c_lflag = 0 ;

	termio.c_cc[VEOF] = 0 ;
	termio.c_cc[VEOL] = 0 ;


 /* 
  * now set the modes and flags 
  */

	if (ioctl (IORser, TCSETA, &termio) < 0)
		perror ("ioctl failed on device");

	return (0);
}

#endif


/******      end of system V code     ***********/


#ifndef  USE_TERMIO

static	int		odisc ;
static	struct	sgttyb	osgttyb ;
static	struct	tchars	otchars;
static	struct	ltchars	oltchars;


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
  * get the tty structures and save
  */

	if (ioctl (IORser, TIOCGETP, &sgttyb) < 0)
		perror ("ioctl failed on device");
	if (ioctl (IORser, TIOCGETC, &tchars) < 0)
		perror ("ioctl failed on device");
	if (ioctl (IORser, TIOCGLTC, &ltchars) < 0)
		perror ("ioctl failed on device");
	if (ioctl (IORser, TIOCGETD, &disc) < 0)
		perror ("ioctl failed on device");



 /* 
  * copy over the structure 
  */

	osgttyb = sgttyb ;  otchars = tchars ;  oltchars = ltchars ;
	odisc = disc ;


	disc = OTTYDISC ;
	sgttyb.sg_ispeed = sgttyb.sg_ospeed = B9600;
	sgttyb.sg_flags = ODDP | RAW;



 /*
 *		eliminate effect of these characters.
 */

	tchars.t_intrc = tchars.t_quitc = tchars.t_startc = tchars.t_stopc =  -1 ;
	ltchars.t_suspc = ltchars.t_dsuspc = ltchars.t_flushc 
	= ltchars.t_lnextc =  -1 ;


 /* 
  * now set the modes and flags 
  */

	if (ioctl (IORser, TIOCSETP, &sgttyb) < 0)
		perror ("ioctl could not set parameters");
	if (ioctl (IORser, TIOCSETC, &tchars) < 0)
		perror ("ioctl could not set parameters");
	if (ioctl (IORser, TIOCSLTC, &ltchars) < 0)
		perror ("ioctl could not set parameters");
	if (ioctl (IORser, TIOCSETD, &disc) < 0)
		perror ("ioctl could not set parameters");


	return (0);
}

#endif

