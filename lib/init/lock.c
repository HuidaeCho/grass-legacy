#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include "local_proto.h"
#include "gis.h"

/******************************************************************
*lock file pid
*
*   this programs "locks" the file for process pid:
*
*   1. if file exists, the pid is read out of the file. if this
*      process is still running, the file is considered locked.
*      exit(1)
*   2. if file does not exist, or if file exists but process is not
*      running (ie, lock was not removed), the file is locked for
*      process pid by writing pid into the file.
*      exit(0).
******************************************************************/

#include <errno.h>
extern int errno;

int main (int argc, char *argv[])
{
    int pid;
    int lockpid;
    int lock;
    int locked;

    if (argc != 3 || sscanf (argv[2],"%d",&lockpid) != 1)
        G_fatal_error("usage: %s file pid", argv[0]);

#define file argv[1]

    locked = 0;
    if ((lock = open (file, 0)) >= 0) /* file exists */
    {
	sleep(1); /* allow time for file creator to write its pid */
	if (read (lock, &pid, sizeof pid) == sizeof pid)
		locked = find_process (pid);
	close (lock);
    }
    if (locked)
	exit(1);
    umask (0);
    if ((lock = creat (file, 0666)) < 0)
    {
	perror (file);
	G_fatal_error("%s: ", argv[0]);
    }
    if (write(lock, &lockpid, sizeof lockpid) != sizeof lockpid)
        G_fatal_error("%s: can't write lockfile %s (disk full? Permissions?)", argv[0], file);
    close (lock);
    exit(0);
}

int 
find_process (int pid)
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
