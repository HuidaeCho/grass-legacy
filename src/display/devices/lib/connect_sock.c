
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>

#include "gis.h"
#include "driverlib.h"

#ifdef USE_G_SOCKS

int
get_connection_sock(int listenfd, int *rfd, int *wfd, int nonblock)
{
    int fd;

    if (nonblock)
    {
	fd_set waitset;
	struct timeval tv;

	tv.tv_sec = 0;
	tv.tv_usec = 10000;

	FD_ZERO(&waitset);
	FD_SET(listenfd, &waitset);
	if (select(FD_SETSIZE, &waitset, NULL, NULL, &tv) < 0)
	{
	    perror("get_connection_sock: select");
	    exit(EXIT_FAILURE);
	}

	if (!FD_ISSET(listenfd, &waitset))
	    return -1;
    }

    /* G_sock_accept will block until a connection is requested */
    fd = G_sock_accept(listenfd);
    if (fd >= 0)
    {
	*rfd = fd;
	*wfd = dup(fd);
	return 0;
    }

    if (errno == EINTR)
	return -1;

    fprintf(stderr, "G_sock_accept: error \"%s\"\n",
	    strerror(errno));
    Graph_Close();
    exit(EXIT_FAILURE);
}

int
prepare_connection_sock(char *me, char *sockpath)
{
    int fd;

    if ((fd = G_sock_bind(sockpath)) < 0)
    {
	fprintf(stderr, "Can't bind to socket; already in use?\n");
	exit(EXIT_FAILURE);
    }

    /* Now set up listen */
    if (G_sock_listen(fd, 1) != 0)
    {
	G_fatal_error("G_sock_listen: error \"%s\"\n", 
		      strerror(errno));
    }

    return fd;
}

int
check_connection(char *me, char *link)
{
    int fd;

    if (G_sock_exists(link))
    {
	if (fd = G_sock_connect(link) >= 0)
	{
	    close(fd);
	    fprintf(stderr,"Graphics driver [%s] is already running\n", me);
	    return(-1);
	}
	if (unlink(link) < 0)
	{
	    fprintf(stderr, "Failed to remove stale socket file:\n"
		    "\t%s\n", link);
	    return(-2);
	}
    }

    return(0);
}

#endif /* USE_G_SOCKS */

