/* %G% %W% */
/* 
    +---------------------------------------------------------+
    |            ANSWERS on GRASS Integration Project         |
    |  Developed in the Agriculture Engineering Department    |
    |                at Purdue University                     |
    |                        by                               |
    |           Chris Rewerts and Bernard Engel               |
    |                                                         |
    |   (c)Copyright, 1992 Purdue Research Foundation, West   |
    |   Lafayette, Indiana 47907. Permission to use, copy,    |
    |   modify, and distribute this software and its          |
    |   documentation for any purpose and without fee is      |
    |   hereby granted, provided that the above copyright     |
    |   notice appear in all copies.  This software is        |
    |   provided "as is" without express or implied warranty. |
    +---------------------------------------------------------+

   */
#include <stdio.h>

hit_return()
{
    char buf[100];
/* sustituyo temporalmente lo siguiente a ve su funciona */
/*    extern char *gets();   */

    fprintf (stderr,"hit RETURN to continue -->");
    if (!fgets(buf,sizeof(buf),stdin)) exit(0);

/*    if (!gets(buf)) exit(0);  */
    G_strip (buf);
    if (strcmp (buf, "exit") == 0) exit(0);
        return(0); 
}
