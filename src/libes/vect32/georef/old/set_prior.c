/*  @(#)set_prior.c    2.1  6/26/87  */
/*
**  Written by Dave Gerdes  5/1988
**  US Army Construction Engineering Research Lab
*/

#include <stdio.h>

/*
**  	For this file, and this file only I have set it up to act on
**      special compiler defines.  Specifically if using a 3b2 or other
**	STRAIGHT ATT System V  that does not support such things as
**	setreuid ()  or seteuid ()  use  #define ATT
**          This will set priority once at the beginning and leave it set.
**	That is the worst case scenario. 
**
**	    Ideally we are trying to set it up so that priority is set
**	only when needed, (i.e. when collecting points in stream mode.)
**	To do this Collect_points calls set_priority()/unset_priority()
**	before and after talking w/ the digitizer.  (on 3B2 these have no 
**	effect)
**
**      If you have defined MASSCOMP in the DIGIT_OPTS in makehead,
**	digit will not run as root.  On Masscomps anyone can issue a
**	nice () request  so there is no need to run set-user as root.
**
**	Written by Dave Gerdes  4/1988
**
*/


#ifdef SYSV
    unsigned    short    getuid();
    unsigned    short    getgid();
#endif

/*
*  set_priority() - this functions sets the raises the priority of the program.
*    This is done to make sure no points are lost when digitizing in STREAM
*    mode.   This means that this program will get more CPU time then any
*    other program.  It will also take CPU time from graphics and ethernet.
*    If graphics or ethernet seem to be buffering output the priority may have
*    to be lessened.
*    Any other words the priority will differ from machine to machine.
*/

#define    PRIO_PROCESS    0

/*  WARNING  -18 gets the most CPU time, priority > -18 gets less CPU time  */
/*  If the graphics don't come out except after digitizing a line then your
*   running too fast and the CPU isn't giving the graphics monitor a chance
*   to update. -mh
*/

#define    NORMAL        0
#define    PRIORITY    -10
/*  read warning above  */

static  int  priority_set;


#ifdef ATT
set_priority_3b2 ()
{
    unsigned    short    getuid();
    unsigned    short    getgid();

    nice(PRIORITY);
    return (0);
}
#endif ATT

init_priority ()
{

#ifdef MASSCOMP
/* MASSCOMP does not require SU to set priorities so just turn off SU */
    if (getuid() == 0 || geteuid() == 0)
	if (getuid() != 0)
	    setuid (getuid());
	else
	    setuid (geteuid ());
#endif
    swap_re_uids ();
    priority_set = 0;

#ifdef ATT
    /* FOR 3b2 comment all the above out 
    ** and replace with the code below 
    *   And then ref
    */
    set_priority_3b2 ();
    
#endif ATT
}

/*  set_priority() returns 1 is already set and 0 if it had to set it.
*/

set_priority ()
{
    int     pid;


#ifndef ATT
#ifdef    SYSV
    unsigned    short    getuid();
    unsigned    short    getgid();

    if (priority_set)
    	return(priority_set);

    swap_re_uids ();	/* set to root */
    nice(PRIORITY);
#else

    if (priority_set)
    	return(priority_set);

    pid = getpid ();
    swap_re_uids ();	/* set to root */
    setpriority (PRIO_PROCESS, pid, PRIORITY);
#endif
    swap_re_uids ();	/* and back to user */
#endif !ATT

    priority_set = 1;
    return(0);
}

unset_priority ()
{
    int     pid;

#ifdef USE_SETUID
#ifdef    SYSV
    swap_re_uids ();	/* set to root */
    nice(-(PRIORITY));
#else
    pid = getpid ();
    swap_re_uids ();	/* set to root */
    setpriority (PRIO_PROCESS, pid, NORMAL);
#endif
    swap_re_uids ();	/* and back to user */

#endif !ATT

    priority_set = 0;
    return(0);
}

swap_re_uids ()
{
#ifdef  USE_SETUID
#ifndef USE_SETRUID
    int hold;
    static flipflop;

    /* should we be turning off interupts here? */

    /* first time thru  Effective will be 0 */
    if (! flipflop)
    {
	/* set Real to 0 */
	hold = getuid ();
	setruid (0);
	seteuid (hold);

	flipflop = 1;
    }
    else
    {
	/* set Effective to 0 */
	hold = geteuid ();
	seteuid (0);
	setruid (hold);
	flipflop = 0;
    }
#else

    setreuid ((int)geteuid(), (int)getuid());

#endif
#endif
}

/* returns -1 if cannot create a user other than root */
set_uid_to_user ()
{
    int user;

    user =  geteuid ();
    if (!user)
	user = getuid ();
    if (!user)
    {
	fprintf (stderr, "Set_uid_to_user () failed!\n");
	return (-1);
    }

    setuid (user);
    return (0);
}


/* leaving this around to point out that the masscomp
**  getuid and geteuid were backwards
*/
/*
#ifdef XMASSCOMP
    user =  getuid ();
    if (!user)
	user = geteuid ();
#else
    user =  geteuid ();
    if (!user)
	user = getuid ();
#endif
*/
