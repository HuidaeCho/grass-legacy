#include	<stdio.h>
#include	<signal.h>

#include	<termio.h>
#include	<fcntl.h>


#define	CHARS_RD	6
#define	ALARM_TIME	3

static	int     IORser;
static	int		N_read ;

static	char    Buffer[1000] ;
static	char    *InBuffer = Buffer;

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

    InBuffer=Buffer;
    for (i=0; i<30; i++)
	    InBuffer[i] = NULL ;


    setbuf (stdout, NULL) ;

    printf("\n this is set to 9600 baud\n\n") ;

    prompt ( tty, "  Which tty is the digitizer connected to (tty0):  ") ;

    /*  tty14 is P6 on out masscomp  */

    if (*tty == NULL)
	    sprintf ( buf, "/dev/tty0") ;  /* CHANGE */
    else
	    sprintf ( buf, "/dev/%s", tty) ;


    printf ("\n Now opening digitizer on %s\n", buf) ;

    if (D_open_serial (buf) < 0)
	    exit (-1);

delay(9000) ;
    printf ("\n\n\n\n       ALTEK DIGITIZER:  binary format 6 characters.\n\n\n\n\n") ;

    D_init_digit() ;
    set_resolution(buf) ;

    printf ("\n\n\n\n") ;

    good_r = bad_r = no_resp = 0 ;


    printf ("     X           Y        Key #  reads:   good       bad     none  Nread \n") ;

    D_flush() ;

while(1)
{
    /*  ask for a coordinate   */
    /*
    D_write_digit ("V") ;
    */

delay(999999) ;


    fprintf (stderr, "\ncalling Dread\n\r");
    if (D__read () < 0)		/*  no response  */
    {
	    ++no_resp ;
	    fprintf (stderr, "\n\rNO RESPONSE\n\r");
	    printf ("\r   %35s  no response:  %ld  ", "", no_resp) ;
	    continue ;
    }
    fprintf (stderr, "\nok\n\r");

    if ( (N_read<0)	 ||   (N_read != CHARS_RD))
     {
	    ++bad_r ;
	    printf ("\r   %35s  bad read:  %ld           N_read:   %d ",
			    "", bad_r, N_read) ;
	    continue ;
     }
		    

    /*  binary standard format   */


    SB =   (int)(InBuffer[0] & 0x40) ;
    PR =   (int)(InBuffer[3] & 0x20) ;
    FC =   (int)( (InBuffer[0] & 0x3c) >> 2) ;
    HP =   (int)(InBuffer[3] & 0x10) ;

    if (PR)
     {
	    printf ("\r%70s", "  cursor off the tablet           ") ;
	    continue ;
     }

    /****
    if (XX)
     {
	    ++bad_r ;
	    printf ("\r  %35s  bad read:  %ld  N_read: %d", "",
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
	    printf ("\r %25s %d    same point  ", "", FC) ;
	    continue ;
    }
    *****/


    printf ("\r    %d       %d      %d              %ld,       %ld        %ld     %d  ",
    X, Y, FC, good_r, bad_r, no_resp,  N_read) ;


    x = X ;     y = Y ;

}
    /*NOTREACHED*/

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
		printf ("\n  digitizer to low resolution.") ;
	 }
	else
	 {
		D_write_digit ("M1\r") ;		/*  high resolution  */
		printf ("\n  digitizer to high resolution.") ;
	 }

}

prompt( buf, p_string)
	char	*buf, *p_string ;
{
	printf ("%s", p_string) ;
	*buf = NULL ;
	if (gets(buf) == NULL)
	{
		clearerr(stdin) ;
		exit(0) ;
	}
}

delay(n)
	int  n ;
{
	for ( ; n < 0; n--)
		;
}

close_down()
{
	signal( SIGALRM, SIG_DFL) ;

	printf(" closing tty ... ") ;
	D_end_digit() ;

	printf("  and exiting \n\n") ;
	exit(0) ;
}


/* device driver code below this point  */


struct	termio	termio ;

D_open_serial (tty_name)
char   *tty_name;
{

	int  D_open_failed() ;

	signal(SIGALRM, D_open_failed ) ;
	alarm(5) ;

	if ((IORser = open (tty_name, O_RDWR, 0)) < 0)
	{
		perror ("Could not open device");
		return (-1);
	}

	alarm(0) ;
	signal( SIGALRM, SIG_DFL) ;
 /* 
  * get the tty structure
  */

	if (ioctl (IORser, TCGETA, &termio) < 0)
		perror ("ioctl failed on device");

 /* 
  *  change the settings
  */

	termio.c_iflag = IGNBRK | IGNPAR ;
	termio.c_oflag = 0 ;
	termio.c_cflag = B9600 | CS8 | CREAD| HUPCL | CLOCAL ;
/***********
termio.c_cflag = B9600 | CS8 | CREAD| HUPCL | CLOCAL ;
***********/
	termio.c_lflag = 0 ;

	termio.c_cc[VEOF] = 0 ;
	termio.c_cc[VEOL] = 0 ;


 /* 
  * now set the modes and flags 
  */

	if (ioctl (IORser, TCSETA, &termio) < 0)
	{
		perror ("ioctl failed on device");
		return (-1) ;
	}

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
	delay(3000) ;
	close (IORser);
}





static  int             timedout ;

int  D__read ()
{

	/***
	reads number of characters from the file ; if it can't read within
	five seconds it returns error status.
	**/

	int  len ;
	int  notfound;

	char  *cp ;

	int	jtimeout();		/*  uses jum_buf  */


	
/*  When a SIGALRM signal is sent  jtimeout will be executed.

    Set the alarm to five seconds before the read.  If the read executes before
    the five seconds is up then the alarm is reset to zero which nullifies the 
    alarm.  If the read doesn't occur before the five sec. ,  then alarm will 
    send the signal and jtimeout will be executed.
*/



	fprintf (stderr, "D__READ\n");
	signal (SIGALRM, jtimeout);
	alarm (ALARM_TIME);
	timedout = 0;

	InBuffer = Buffer;
	cp = InBuffer ;
	len = CHARS_RD ;
	notfound = 1;
	while (len > 0)
	{
		if (timedout) break;
		N_read = read (IORser, cp, len );	
/*
	if (N_read == 1)
	    fprintf (stderr, "    READ 1 char   '%c' %d\n", *cp, (int) *cp);
*/
		if (N_read < 0)
		{
			fprintf (stderr, "Digitizer read error\n");
			continue;
		}

		cp += N_read ;

		/* we are stuck w/out getting input */
		if (cp - Buffer > 50)
		{
/*DEBUG*/ fprintf (stderr, "YO!  walking too far down the buffer\n");
		    return (-1);
		}

		len -= N_read ;

#ifdef COMMENT_OUT
		if (notfound)
		{
		    for (/*InBuffer = Buffer*/ ; InBuffer < cp ; InBuffer++ , ((len >= CHARS_RD) ? len = CHARS_RD : len++))
			if (firstchar (InBuffer))
			{
			    notfound = 0;
			    break;
			}
			else
			    fprintf (stderr, "OOPS  %d  len %d\n", InBuffer - Buffer, len);
		}
#endif COMMENT_OUT

/***
fprintf( stderr, "\n len: %d, N_read: %d\n", len, N_read) ;
***/

	}

	alarm (0);
	signal (SIGALRM, SIG_DFL);


	if (timedout || N_read < 0)			  /*  did it timeout  */
		return (-1);

		/*  strip parity  */
	/*
	for ( cp = InBuffer;  cp < InBuffer + CHARS_RD;  cp++)
		*cp &= 0177;
	*/

	return (N_read = CHARS_RD - len);

}	/*  end of r_read ()  */

firstchar (p)
    char *p;
{
    return ((int) (*p & '\200'));
}


jtimeout (n)
{
    timedout = 1;
}
sigquit(n) {fprintf(stderr, "QUIT CALLED\n");}


D_init_digit()
{
		D_write_digit ("") ;			/*  reset digitizer  */
		delay(3000) ;

		/*	 set output mode  */
		D_write_digit ("F8\r") ;
		delay(3000) ;
		D_write_digit ("P") ;			/*  set point mode  */
		delay(3000) ;
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


D_flush()
{
	int  i ;

/*  flush digitizer by reading 10 coordinates or  until there no response */

	ioctl (IORser, TCFLSH, 0) ;

/******
	for (i = 0; i < 10; i++)
		if ( D__read(1) < 0)
			return(0) ;
******/

	return(0) ;
}

