/*  %W%  %G%  */
char *getenv();
#include <stdio.h>
#include <signal.h>
#include "graph.h"
#include "monitors.h"
#include "gis.h"
#include <errno.h>

extern int errno;

#define BUFFERSIZ	4096
#define FIFO           /* for using fifo files */
/*#define SOCKET         /* for using internet sockets */

static char outbuf[BUFFERSIZ] ;
static int cursiz = 0 ;
static int no_mon ;

static int _rfd;
static int _wfd;

_send_ident(anint)
	int anint ;
{
	char achar ;
	achar = anint;

	if( (cursiz+1) >= BUFFERSIZ)
		flushout() ;
	outbuf[cursiz++] = achar ;
}

_send_char(achar)
	char *achar ;
{
	if( (cursiz+1) >= BUFFERSIZ)
		flushout() ;
	outbuf[cursiz++] = *achar ;
}

_send_char_array(num, achar)
	int num ;
	unsigned char *achar ;
{
	register unsigned char *ptr ;
	register int i ;

	ptr = achar ;
	for(i=0; i<num; i++)
	{
		if (cursiz + 1 >= BUFFERSIZ)
			flushout() ;
		outbuf[cursiz++] = *ptr++ ;
	}
}

_send_int_array(num, anint)
	int num ;
	int *anint ;
{
	_send_char_array(num * sizeof(int), (unsigned char *)anint) ;
}

_send_float_array(num, afloat)
	int num ;
	float *afloat ;
{
	_send_char_array(num * sizeof(float), (unsigned char *)afloat) ;
}

_send_int(anint)
	int *anint ;
{
	_send_char_array(sizeof(int), (unsigned char *)anint) ;
}

_send_float(afloat)
	float *afloat ;
{
	_send_char_array(sizeof(float), (unsigned char *)afloat) ;
}

_send_text(text)
	char *text ;
{
	_send_char_array(1 + strlen(text), (unsigned char *)text) ;
}

_get_char(achar)
	char *achar ;
{
	flushout() ;
	_get (achar, 1); 
}

_get_int(anint)
	int *anint ;
{
	flushout() ;
	_get( (char *)anint, sizeof(int)); 
}

_get_float(afloat)
	float *afloat ;
{
	flushout() ;
	_get( (char *)afloat, sizeof(float)); 
}

_get_text (buf)
    char *buf;
{
    char *b;

    b = buf;
    do
	_get_char (b);
    while (*b++ != 0);
}

_get (buf, n)
    char *buf;
{
    int x;
    while (n > 0)
    {
	x = read (_rfd, buf, n);
	if (x <= 0)
	{
	    fprintf (stderr, "ERROR %s from graphics driver\n", x?"reading":"eof");
	    exit(1);
	}
	n -= x;
	buf += x;
    }
}

flushout()
{
	if (cursiz)
	{
		write (_wfd, outbuf, cursiz);
		cursiz = 0 ;
	}
}

#ifdef SOCKET
R_open_driver() 
{
	char buffer[128] ;
	int status ;

/* Try to get contact established with monitor program thru a socket */
			
	socketto (1000) ;

	return(0) ;
}

/*************************************************
* socketto(time)
*      this is the plumbing, the idea is to
*      open a socket for read and write.
*************************************************/
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<netdb.h>

#define READ  0
#define WRITE 1

socketto(alarm_time)
	int alarm_time ;
{
	int 			sock, c;
	char 			*host = "illinois", *service = "sunmonitor";
	struct sockaddr_in 	sin;
	struct hostent 		*host_p;
	struct servent 		*serv_p;

	serv_p = getservbyname("sunmonitor", "tcp") ;
	if (serv_p == NULL)
	{
		fprintf(stderr, "client: %s: service unknown\n", service);
		exit(1);
	}
	host_p = gethostbyname(host) ;
	if (host_p == NULL)
	{
		fprintf(stderr, "client: %s: host unknown\n", host);
		exit(1);
	}
	bzero((char *)&sin, sizeof sin) ;
	bcopy(host_p->h_addr, (char *)&sin.sin_addr, host_p->h_length) ;
	sin.sin_family = host_p->h_addrtype ;
	sin.sin_port = serv_p->s_port ;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		perror("client: socket");
		exit(2);
	}
#ifdef DEBUG
	fprintf(stderr, "Connect to %s... port %d\n", host, sin.sin_port);
#endif
	if (connect(sock, &sin, sizeof(sin)) < 0)
	{
		perror("client: connect");
		exit(5);
	}

	if ( (_wfd = dup(sock)) < 0 || (_rfd = dup(sock)) < 0)
	{
		perror("dup");
		_exit(1);
	}

	return(1) ;
}
#endif SOCKET

#ifdef FIFO

/* R_open_driver for communication over fifos -- 7 Oct 87 */
/* In verbose mode, errors print a message and exit.  In quiet mode, errors */
/* return a code and no messages are printed.  The flag _verbose is cleared */
/* by calling R__open_quiet just before calling R_open_driver. */
/* Returns in quiet mode opens are defined in driver_open.h */

#include "driver_open.h"
static int _verbose = 1;		/* #9 Sep 87 */

R_open_driver() 
{
  int try, offset, key, lock;
  char our_input_file[512], our_output_file[512];
  struct MON_CAP *mon, *parse_mon();
  char *name, *G__getenv(), *getenv(), *key_string;
  char gis_file[100];
  char *user, *who_locked();

  if ((name = G__getenv("MONITOR")) == NULL)
  {
    if (_verbose)			/* #31 Aug 87 - want error stuff */
    {
      fprintf(stderr,"No graphics monitor has been selected for output\n");
      fprintf(stderr,"Please use Dnew_mon to select a graphics monitor\n");
      exit(-1);
    }
    else
    {
      _verbose = 1;			/* #31 Aug 87 - assume verbose mode */
      return(NO_MON);
    }
  }
  else
  {
    strcpy(gis_file,G__getenv("GISBASE"));
    offset = strlen(gis_file);
    strcat(gis_file,"/etc/monitorcap");
    if ((mon = parse_mon(gis_file,MON_NAME,name)) == NULL)
    {
      if (_verbose)
      {
        fprintf(stderr,"No such graphics monitor as '%s'\n",name);
        fprintf(stderr,"Please use Dnew_mon to select a valid graphics monitor\n");
        fprintf(stderr,"Dlist_mon will list available monitors\n");
        exit(-1);
      }
      else
      {
        _verbose = 1;			/* #31 Aug 87 - assume verbose mode */
        return(NO_MON);
      }
    }
    else
    {
/*
      if ((key_string = getenv("GIS_LOCK")) == NULL)
      {
        if (_verbose)
        {
          fprintf(stderr,"Error - No key available to lock monitor\n");
          exit(-1);
        }
        else
        {
          _verbose = 1;
          return(NO_KEY);
        }
      }
      sscanf(key_string,"%d",&key);
*/
      key_string = getenv("GIS_LOCK");
      if (key_string == NULL || sscanf(key_string,"%d",&key) != 1 || key <= 0)
	key = getpid();
      sprintf(&gis_file[offset],"/locks/%s",name);
      lock = lock_file(gis_file,key);
      if (lock == 0)
      {
        if (_verbose)
        {
          if ((user = who_locked(gis_file)) == NULL)
            fprintf(stderr,"Error - Monitor '%s' is in use\n",name);
          else
            fprintf(stderr,"Error - Monitor '%s' is in use by %s\n",name,user);
	  exit(-1);
        }
        else
        {
          _verbose = 1;
          return(LOCKED);
        }
      }
      if (lock < 0)
      {
        if (_verbose)
        {
          fprintf(stderr,"Error - Could not complete locking process for monitor '%s'\n",name);
          exit(-1);
        }
        else
        {
          _verbose = 1;
          return(LOCK_FAILED);
        }
      }
      sscanf(mon->link,"%s %s",our_output_file,our_input_file);
      if (_verbose)
      {
        for (try = 0; try < 2; try++)
        {
          switch (fifoto (our_input_file,our_output_file,try?15:3))
          {
            case -1:
              fprintf(stderr, "\07Error - Can't set up pipe to graphics device\n");
              unlock_file(gis_file);
              exit(-1);
            case 0:
              if (try)
              {
                fprintf (stderr, "Error - Graphics monitor '%s' not running!\n",name);
                unlock_file(gis_file);
                exit(1);
              }
              fprintf (stderr, "\07Please start graphics monitor '%s' or use Dnew_mon to change\n",name);
              break;
            default:
              return(0);
          }				/* switch */
        }				/* for */
      }
      else				/* non-verbose mode */
      {
        _verbose = 1;			/* #31 Aug 87 - assume verbose mode */
        switch (fifoto(our_input_file,our_output_file,3))
        {
          case -1:
            unlock_file(gis_file);
            return(NO_OPEN);
          case 0:
            unlock_file(gis_file);
            return(NO_RUN);
          default:
            return(OKOK);
        }
      }
    }
  }
}

R__open_quiet()
{
  _verbose = 0;
}


/*************************************************
* fifoto(alarm_time)
*      this is the plumbing, the idea is to
*      open fifo pipes for read/write.
*************************************************/

#define READ  0
#define WRITE 1

static
fifoto(input,output,alarm_time)
char *input, *output;
{
    int dead() ;
    int (*sigalarm)();

    no_mon = 0;
    sigalarm = signal(SIGALRM, dead);
    alarm(alarm_time);
    _wfd = open(output, WRITE) ;
    alarm(0);
    signal(SIGALRM, sigalarm);
    if (no_mon) 
        return 0 ;

    no_mon = 0;
    signal(SIGALRM, dead);
    alarm(alarm_time);
    _rfd = open(input, READ) ;
    alarm(0);
    signal(SIGALRM, sigalarm);
    if (no_mon) 
        return 0 ;


    if( (_wfd == -1) || (_rfd == -1) ) 
        return -1;

    return 1 ;
}

static
dead()
{
    no_mon = 1 ;
}
#endif FIFO

_hold_signals (hold)
{
    static int (*sigint)();
    static int (*sigquit)();

    if (hold)
    {
	sigint = signal (SIGINT, SIG_IGN);
	sigquit = signal (SIGQUIT, SIG_IGN);
    }
    else
    {
	signal (SIGINT, sigint);
	signal (SIGQUIT, sigquit);
    }
}

/******************************************************************
* lock_file (file,pid)
*   char *file
*
*   this routine "locks" the file for process pid as follows:
*
*   1. if file exists, the old pid is read out of the file.
*
*      if the old pid and the new pid are the same, then
*      nothing more is done and the request is successful.
*
*      if this the old pid process is still running, the file is
*      considered locked.
*
*   2. if file does not exist, or if file exists but process is not
*      running (ie, lock was not removed), the file is locked for
*      process pid by writing pid into the file.
*
*
* note: since file is created by this routine, it shouldn't be
*       a file that is used for any other purpose.
*
*       also, this lock mechanism is advisory. programs must call this
*       routine and check the return status.
*
* returns:
*       1 ok lock is in place.
*       0 could not lock because another process has the file locked already
*      -1 error. could not create the lock file
*      -2 error. could not read the lock file.
*      -3 error. could not write the lock file.
******************************************************************/
#define LOCK_OK 1
#define ALREADY_LOCKED 0
#define CANT_CREATE -1
#define CANT_READ -2
#define CANT_WRITE -3

#include <pwd.h>
static lock_file (file, lock_pid)
    char *file;
{
    int fd;
    int locked;
    int mask;
    int n;
    int old_pid;
    struct passwd *pw;
    struct passwd *getpwuid();

    locked = 0;
    if (access (file, 0) == 0)		/* file exists */
    {
	for (n = 0; n < 2; n++)
	{
	    if (get_pid (file, &old_pid))
		break;
	    if (n == 0)
		sleep(1); /* allow time for file creator to write its pid */
	}
	if (n == 2)
	    return CANT_READ;
	if (lock_pid == old_pid)
	    return LOCK_OK;
	locked = find_process (old_pid);
    }
    if (locked)
	return ALREADY_LOCKED;
    mask = umask (0);
    fd = creat (file, 0666) ;
    umask (mask);
    if (fd < 0)
	return CANT_CREATE;
    if (write(fd, &lock_pid, sizeof lock_pid) != sizeof lock_pid)
    {
	close (fd);
	return CANT_WRITE;
    }
    if ((pw = getpwuid(getuid())) != NULL)
    {
        n = strlen(pw->pw_name);
        if (write(fd, pw->pw_name, n) != n)
        {
	    close (fd);
	    return CANT_WRITE;
        }
    }
    else
    {
        if (write(fd, "UNKNOWN", 7) != 7)
        {
	    close (fd);
	    return CANT_WRITE;
        }
    }
    close (fd);
    return LOCK_OK;
}

static char *who_locked(file)
char *file;
{
  int lock_pid;
  static char user[100];
  int n, fd;

  if ((fd = open(file,0)) < 0)
    return(NULL);
  if (read(fd,&lock_pid,sizeof lock_pid) != sizeof lock_pid)
  {
    close(fd);
    return(NULL);
  }
  n = read(fd,user,sizeof user - 1);
  close(fd);
  if (n <= 0)
    return(NULL);
  user[n] = '\0';
  return(user);
}

static
get_pid (file, old_pid)
    char *file;
    int *old_pid;
{
    int fd;
    int n;

    if ((fd = open (file, 0)) < 0)
	return 0;
    n = read (fd, old_pid, sizeof (*old_pid));
    close (fd);
    return n == sizeof (*old_pid);
}

static
find_process (pid)
{
/* attempt to kill pid with NULL signal. if success, then
   process pid is still running. otherwise, must check if
   kill failed because no such process, or because user is
   not owner of process
*/
    if (kill (pid, 0) == 0)
	return 1;
    return errno != ESRCH;
}

static unlock_file (file)
    char *file;
{
    if (access (file,0) != 0)
	return 0;
    unlink (file);
    if (access (file,0) != 0)
	return 1;
    return -1;
}

R_kill_driver()				/* #31 Aug 87 - stop a driver */
{
    _send_ident(GRAPH_CLOSE);		/* #31 Aug 87 - tell driver to exit */
    flushout();
    close (_rfd);
    close (_wfd);
    R_release_driver();
}

R_close_driver()
{
    char c;

    flushout();
    _send_ident (RESPOND);
    _get_char (&c);

    close (_rfd);
    close (_wfd);
}

R_release_driver()
{
  char *G__getenv(), *name;
  char file[200];

  strcpy(file,G__getenv("GISBASE"));
  strcat(file,"/locks/");
  if ((name = G__getenv("MONITOR")) != NULL)
  {
    strcat(file,name);
    unlock_file(file);
  }
}
