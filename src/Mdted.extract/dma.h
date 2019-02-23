/* %W% %G% */
#include <stdio.h>

#ifdef MAIN
#define GLOBAL
#else
#define GLOBAL extern
#endif

GLOBAL int tapefd;		/* tape input file descriptor */
GLOBAL int outfd;		/* output file descriptor */
GLOBAL FILE *headfd;		/* header file descriptor */
GLOBAL unsigned char *tapebuf;	/* tape buffer */
GLOBAL int tapebuflen;
GLOBAL int nrows, ncols;	/* output rows, cols */

/* all lat/lon values are in 10ths of arc seconds */

GLOBAL long north, south;	/* user requested window */
GLOBAL long east, west;
GLOBAL long tnorth, tsouth;	/* current tape window */
GLOBAL long teast, twest;
GLOBAL int latres, lonres;	/* data resolution */
GLOBAL long cur_lon ;		/* lon of current tape swath */

GLOBAL char tapename[100];
GLOBAL char outname[100];
GLOBAL char headname[100];

GLOBAL char *PGM;
GLOBAL int verbose ;
GLOBAL int stopok ;

char *malloc() ;
char *realloc() ;

#define abs(x) ( (x) < 0 ? -(x) : (x) )
