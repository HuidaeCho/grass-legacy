/*
* $Id$
*
****************************************************************************
*
* LIBRARY:      unix_socks.c  -- Routines related to using UNIX domain 
*               sockets for IPC mechanisms (such as XDRIVER).
*
* AUTHOR(S):    Eric G. Miller
*
* PURPOSE:      Historically GRASS has used FIFO for interprocess communic-
*               ations for display functions.  Unfortunately, FIFO's are
*               not available on all target platforms.  An attempt has been
*               made to use IPC message passing, but the semantics are
*               variable and it also isn't available on all target platforms.
*               UNIX sockets, or local or domain sockets, are much more
*               widely available and consistent.  NOTE: This implementation
*               of UNIX sockets provides zero security checking so should
*               not be used from untrusted clients.
*
* COPYRIGHT:    (C) 2000 by the GRASS Development Team
*
*               This program is free software under the GNU General Public
*   	    	License (>=v2). Read the file COPYING that comes with GRASS
*   	    	for details.
*
*****************************************************************************/

#include "gis.h"
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

/** For systems where the *_LOCAL (POSIX 1g) is not defined 
 ** There's not really any difference between PF and AF in practice.
 **/
#ifndef AF_LOCAL
#define AF_LOCAL AF_UNIX
#endif
#ifndef PF_LOCAL
#define PF_LOCAL PF_UNIX
#endif

/* For switching between putting socket file in <mapset>/.tmp/<host>/
 * and $HOME/.grass (see user_config.c)
 */
#define USE_TEMP_ELEMENT

/* ----------------------------------------------------------------------
 * G_sock_get_fname(), builds the full path for a UNIX socket using the
 * G__temp_element() routine (tempfile.c).  Caller should free() the
 * return value when it is no longer needed.
 *
 * Notes:
 *     Local socket pathnames are limited to 104 or 108 bytes.  Since
 *     this implementation uses the mapset temp directory, deeply nested
 *     mapsets may cause a failure.  In the future, there should be a
 *     $HOME/.grass directory, with a "com" or "dev" directory where we
 *     can set up these communication channels.  Alternatively, we could
 *     use a /tmp/grass/$USER/ directory, where $USER is mode 0700 (for
 *     some security -- it wouldn't offer much, since sockets may be
 *     create with world rw permissions anyway [depends on the system]).
 * ---------------------------------------------------------------------*/

#ifdef USE_TEMP_ELEMENT
char *
G_sock_get_fname (char *name)
{
    char element[100], path[1024];

    if (name == NULL)
        return NULL;
    
    G__temp_element (element);
    
    G__file_name (path, element, name, G_mapset());

    return G_store (path);
}
#else
char *
G_sock_get_fname (char *name)
{
    if (name == NULL)
        return NULL;
    
    /* G_rc_path will make the whole path, and make sure the directories
     * exist with good permissions.
     */
    return G_rc_path ("com", name);
}
#endif

/* -------------------------------------------------------------------
 * G_sock_exists(char *): Returns 1 if path is to a UNIX socket that
 * already exists, 0 otherwise.
 * -------------------------------------------------------------------*/
    
int
G_sock_exists (char *name)
{
    struct stat theStat;

    if (name == NULL || stat (name, &theStat) != 0)
        return 0;

    if (S_ISSOCK (theStat.st_mode))
        return 1;
    else
        return 0;
}


/* -----------------------------------------------------------------
 * G_sock_bind (char *): Takes the full pathname for a UNIX socket
 * and returns the file descriptor to the socket after a successful
 * call to bind().  On error, it returns -1.  Check "errno" if you
 * want to find out why this failed (clear it before the call).
 * ----------------------------------------------------------------*/

int
G_sock_bind (char *name)
{
    int    sockfd;
    size_t size;
    struct sockaddr_un addr;

    if (name == NULL)
        return -1;

    /* Bind requires that the file does not exist. Force the caller
     * to make sure the socket is not in use.  The only way to test,
     * is a call to connect().
     */
    if (G_sock_exists (name))
    {
        errno = EADDRINUSE;
        return -1;
    }

    /* must always zero socket structure */
    memset (&addr, 0, sizeof(addr));

    /* The path to the unix socket must fit in sun_path[] */
    if (sizeof (addr.sun_path) < strlen(name) + 1)
        return -1;
    
    strncpy (addr.sun_path, name, sizeof (addr.sun_path) - 1);
    
    addr.sun_family = AF_LOCAL;

    sockfd = socket (PF_LOCAL, SOCK_STREAM, 0);

    size = (offsetof (struct sockaddr_un, sun_path) 
            + strlen (addr.sun_path) + 1);

    if (bind (sockfd, (struct sockaddr *) &addr, size) != 0)
        return -1;

    return sockfd;
}


/* ---------------------------------------------------------------------
 * G_sock_listen(int, unsigned int): Wrapper around the listen() 
 * function.
 * --------------------------------------------------------------------*/

int
G_sock_listen (int sockfd, unsigned int queue_len)
{
    return listen (sockfd, queue_len);
}

/* -----------------------------------------------------------------------
 * G_sock_accept (int sockfd):
 * Wrapper around the accept() function. No client info is returned, but
 * that's not generally useful for local sockets anyway.  Function returns
 * the file descriptor or an error code generated by accept().  Note,
 * this call will usually block until a connection arrives.  You can use
 * select() for a time out on the call.
 * ---------------------------------------------------------------------*/

int
G_sock_accept (int sockfd)
{
    struct sockaddr_un addr;
    int len = sizeof(addr);
    return accept (sockfd, (struct sockaddr *) &addr, &len);
}
 

/* ----------------------------------------------------------------------
 * G_sock_connect (char *name):  Tries to connect to the unix socket
 * specified by "name".  Returns the file descriptor if successful, or
 * -1 if unsuccessful.  Global errno is set by connect() if return is -1
 * (though you should zero errno first, since this function doesn't set
 * it for a couple conditions).
 * --------------------------------------------------------------------*/

int
G_sock_connect (char *name)
{
    int    sockfd;
    struct sockaddr_un addr;

    if (!G_sock_exists (name))
        return -1;

    /* must always zero socket structure */
    memset (&addr, 0, sizeof(addr));

    /* The path to the unix socket must fit in sun_path[] */
    if (sizeof (addr.sun_path) < strlen(name) + 1)
        return -1;
    
    strncpy (addr.sun_path, name, sizeof (addr.sun_path) - 1);
    
    addr.sun_family = AF_LOCAL;

    sockfd = socket (PF_LOCAL, SOCK_STREAM, 0);

    if (connect (sockfd, (struct sockaddr *) &addr, sizeof (addr)) != 0)
        return -1;
    else
        return sockfd;
}

/* vim: set softtabstop=4 shiftwidth=4 expandtab : */
