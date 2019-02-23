
/*
*  geo_read_control() - reads the digitizer info from file and the control
*    point information.
*
*  geo_save_control() - writes the digitizer info to the file and the control
*    point information.
*
*  The ascii data file's format that is being written and read to is:
*   1st line -  'digitizer_name  tty  lockfile_name'
*   2nd and following lines   'utm_x  utm_y  raw_x  raw_y'
*
*/

#include    <stdio.h>
#include    "map.h"

geo_read_control (fp, digitizer_name, tty, lock_name, pid)
    FILE    *fp;
    char  *digitizer_name ;
    char  *tty ;
    char  *lock_name ;
    int   *pid ;
{

    int   i;
    int   num_read ;
    char  buf[128];

/*  first line is the digitizer info  */
    if ( fgets (buf, (sizeof buf)-1, fp) == NULL)
	return(-1) ;

    num_read = sscanf( buf, "%s %s %s %d", digitizer_name, tty, lock_name, pid) ;
    if (  num_read != 4)
	return(-1) ;

/*  all lines following are control_points  */
    for ( i=0;  i<MAX_COOR; i++  )
     {
	if ( fgets (buf, sizeof (buf), fp) == NULL)
	    break;

	num_read = sscanf (buf, "%lf %lf %lf %lf", &bx[i], &by[i],
		&ax[i], &ay[i]) ;
	if ( num_read !=  4)
		return(-1) ;

	use[i] = C_FILE ;

     }	/*  for (i)  */

    return (i);

}



geo_save_control (fp, digitizer_name, tty, lock_name, pid)
    FILE  *fp;
    char  *digitizer_name ;
    char  *tty ;
    char  *lock_name ;
    int   pid ;
{
    register    int    i;

    fseek (fp, 0L, 0) ;	/*  seek to top  */
    fprintf (fp, "%s  %s  %s  %d\n", digitizer_name, tty, lock_name, pid) ;

    for ( i=0;  i<MAX_COOR;  i++)
	if (use[i])
	    fprintf (fp, "%lf %lf %lf %lf\n", bx[i], by[i], ax[i], ay[i]);

    return(0) ;

}	
