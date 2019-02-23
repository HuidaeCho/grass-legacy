/*%W%   %G%*/
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <curses.h>
#include <ctype.h>
#include <math.h>
/*==================================================================
 |		This file contains the Constants, Structures, and
 |	Variables referenced by the menu program for the CERL Fort
 |	Hood Information System Menu Driver. These symbols are used 
 |	by the program living in file mentst.c.
 */

#define NCOLS        80
#define NOERR_ERRNUM  0
#define FILSTA_ERRNUM 1
#define FILOPN_ERRNUM 2
#define FILRED_ERRNUM 3
#define	FILCLS_ERRNUM 4
#define	NOMEM_ERRNUM  5
#define FRKERR_ERRNUM 6
#define	IVLDCD_ERRNUM 7
 
#define MENWINHITE   7
#define CMDWINHITE   7
#define HELPWINHITE   14
#define MAXSHRTRESP   70
#define COMMANDFILE   "Mencommands"
#define TMPFILE       "./tempo"

#define NULLSTR "\0\n"   
#define	LF	'\n'
#define	CR	'\r'
#define	VERTCHAR	'|'
#define	HORZCHAR	'-'
#define	BKSPC	'\b'
#define	BLANK	' '
#define	KILLINE	 '\027'			/* Ctrl w */
#define TOPPAG   't'			/*      t */
#define BOTPAG   'e'			/*      e */
#define BAKPAG   'b'			/*      b */
#define FORPAG   'f'			/*      f */
#define BAKLIN   'd'			/*      d */
#define FORLIN   'u'			/*      u */
#define REFRESH  '\014' 		/* Ctrl l */ 
#define PRINTOUT '\001' 		/* Ctrl a */ 
#define HELP   	 '?'  
 
#define MICRORESP	1
#define MICROMESG	"Immediate response"
#define MINIRESP	2
#define MINIMESG	"One word response"
#define LINERESP	4
#define LINEMESG	"One line response"
#define LONGRESP	8
#define LONGMESG	"File edit response"
#define CHOOSERESP	16
#define CHOOSEMESG	"Choose response"
#define DEFLTRESP	MINIRESP
#define DEFLTMESG	MINIMESG

#define LEFTMARG	2
 
/*---------------------------------------------
 |		Global Data Structures
 */

WINDOW	*MenuW, *PlanetW, *PrmptW, *TablW;
WINDOW	*StatehelpW, *CommandhelpW, *ErrorW;
