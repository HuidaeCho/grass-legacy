/***************************************************************************
*
*
*                         NCSA HDF version 3.2r3
*                            December 1, 1992
*
* NCSA HDF Version 3.2 source code and documentation are in the public
* domain.  Specifically, we give to the public domain all rights for future
* licensing of the source code, all resale rights, and all publishing rights.
*
* We ask, but do not require, that the following message be included in all
* derived works:
*
* Portions developed at the National Center for Supercomputing Applications at
* the University of Illinois at Urbana-Champaign, in collaboration with the
* Information Technology Institute of Singapore.
*
* THE UNIVERSITY OF ILLINOIS GIVES NO WARRANTY, EXPRESSED OR IMPLIED, FOR THE
* SOFTWARE AND/OR DOCUMENTATION PROVIDED, INCLUDING, WITHOUT LIMITATION,
* WARRANTY OF MERCHANTABILITY AND WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE
*
****************************************************************************
*/
/*
$Header$

$Log$
Revision 1.1  1999-12-29 15:12:33  markus
Initial revision

 * Revision 1.13  1992/10/23  19:51:58  koziol
 * Added _HUGE keyword for MS-Windows and MS-DOS compatibility.  On
 * all other machines it gets pre-processed out.  Please use this
 * in all future prototype declarations with pointers in them.
 *
 * Revision 1.12  1992/10/23  00:21:45  koziol
 * Fixed typo in WINIO declaration
 *
 * Revision 1.11  1992/10/23  00:14:11  koziol
 * Changed all DFIstr*() and DFImem*() calls to HDstr*() and HDmem*() calls
 * #ifdef'd out the macros Jason defined for Hopen, Hclose, etc. for Vsets
 * Replaced Vset VFREESPACE and VGETSPACE calls with actual calls to HDfreespace
 * and HDgetspace
 * Added a MS-Windows lower lower for file I/O (which may not be completely working
 *
 * Revision 1.9  1992/10/12  18:11:51  koziol
 * Updated for v3.2r2 release
 *
 * Revision 1.8  1992/09/28  17:05:50  koziol
 * Removed "#define BSD" from NeXT section
 *
 * Revision 1.7  1992/09/25  15:35:12  koziol
 * Added #define's for NeXT support (use -DNEXT on the compile line
 * for the machine type)
 *
 * Revision 1.6  1992/09/24  20:13:41  koziol
 * Put the "const" kludge in for HP9000 machines
 *
 * Revision 1.5  1992/09/18  21:52:57  koziol
 * Added the crazy __STDC__ define to the Convex section
 *
 * Revision 1.4  1992/09/17  19:43:59  koziol
 * Included Mike's changes to make Prototypes work on the Convex
 *
 * Revision 1.3  1992/09/11  14:15:04  koziol
 * Changed Fortran stubs' parameter passing to use a new typedef, intf,
 * which should be typed to the size of an INTEGER*4 in whatever Fortran
 * compiler the C functions need to be compatible with.  (This is mostly
 * for the PC and possibly for the Mac)
 *
 * Revision 1.2  1992/09/04  22:05:02  georgev
 * Fixed "const" problem for MIPSEL.
 *
 * Revision 1.1  1992/08/25  21:40:44  koziol
 * Initial revision
 *
*/
#ifndef HDFI_H
#define HDFI_H

#ifdef GOT_MACHINE
#undef GOT_MACHINE
#endif

/*--------------------------------------------------------------------------*/
/*                              MT/NT constants                             */
/*  four MT nibbles represent double, float, int, uchar (from most          */
/*  significant to least significant).                                      */
/*  The values for each nibble are:                                         */
/*      1 - Big Endian                                                      */
/*      2 - VAX                                                             */
/*      3 - Cray                                                            */
/*      4 - Little Endian                                                   */
/*--------------------------------------------------------------------------*/
#define     DFMT_SUN            0x1111
#define     DFMT_ALLIANT        0x1111
#define     DFMT_IRIS4          0x1111
#define     DFMT_APOLLO         0x1111
#define     DFMT_IBM6000        0x1111
#define     DFMT_HP9000         0x1111
#define     DFMT_CONVEXNATIVE   0x5511
#define     DFMT_CONVEX         0x1111
#define     DFMT_UNICOS         0x3331
#define     DFMT_CTSS           0x3331
#define     DFMT_VAX            0x2221
#define     DFMT_MIPSEL         0x4441
#define     DFMT_PC             0x4441
#define     DFMT_MAC            0x1111
#define     DFMT_SUN386         0x4441
#define     DFMT_NEXT           0x1111
#define     DFMT_MOTOROLA       0x1111

/* I/O library constants */
#define UNIXUNBUFIO 1
#define UNIXBUFIO   2
#define MACIO       3
#define PCIO        4
#define WINIO       5

#include <stdio.h>

#ifdef __STDC__
#define PROTOTYPE
#include <stdlib.h>
#endif

#ifdef SUN

#ifdef GOT_MACHINE
If you get an error on this line more than one machine type has been defined.
Please check your Makefile.
#endif
#define GOT_MACHINE

#   define BSD
#include <string.h>
#ifndef __GNUC__
#include <memory.h>
#endif /* __GNUC__ */
#include <unistd.h>
#include <sys/file.h>               /* for unbuffered i/o stuff */
#define DF_MT             DFMT_SUN
typedef void              VOID;
typedef char              *VOIDP;
typedef char              *_fcd;
typedef int               bool;
typedef char              char8;
typedef unsigned char     uchar8;
typedef char              int8;
typedef unsigned char     uint8;
typedef short int         int16;
typedef unsigned short int uint16;
typedef long int          int32;
typedef unsigned long int uint32;
typedef int               intn;
typedef unsigned int      uintn;
typedef int               intf;     /* size of INTEGERs in Fortran compiler */
typedef float             float32;
typedef double            float64;
#define _HUGE              /* This should only be defined to a value on the PC */
#define DFmovmem(from, to, len) memcpy(to, from, len)
#define FNAME_POST_UNDERSCORE
#define _fcdtocp(desc) (desc)
#define FILELIB UNIXBUFIO
#ifndef __STDC__
#define const
#endif
#endif /* SUN */


#ifdef IBM6000

#ifdef GOT_MACHINE
If you get an error on this line more than one machine type has been defined.
Please check your Makefile.
#endif
#define GOT_MACHINE

#   define BSD
#include <string.h>
#ifndef __GNUC__
#include <memory.h>
#endif /* __GNUC__ */
#include <sys/file.h>               /* for unbuffered i/o stuff */
#define DF_MT             DFMT_IBM6000
typedef void              VOID;
typedef void              *VOIDP;
typedef char              *_fcd;
typedef int               bool;
typedef char              char8;
typedef unsigned char     uchar8;
typedef char              int8;
typedef unsigned char     uint8;
typedef short int         int16;
typedef unsigned short int uint16;
typedef long int          int32;
typedef unsigned long int uint32;
typedef int               intn;
typedef unsigned int      uintn;
typedef float             float32;
typedef double            float64;
typedef int               intf;     /* size of INTEGERs in Fortran compiler */
#define _HUGE              /* This should only be defined to a value on the PC */
#define DFmovmem(from, to, len) memcpy(to, from, len)
#define _fcdtocp(desc) (desc)
#define FILELIB UNIXBUFIO
#endif /* IBM6000 */

#ifdef HP9000

#ifdef GOT_MACHINE
If you get an error on this line more than one machine type has been defined.
Please check your Makefile.
#endif
#define GOT_MACHINE

#   define BSD
#include <string.h>
#ifndef __GNUC__
#include <memory.h>
#endif /* __GNUC__ */
#include <sys/file.h>               /* for unbuffered i/o stuff */
#define DF_MT             DFMT_HP9000
typedef void              VOID;
typedef void              *VOIDP;
typedef char              *_fcd;
typedef int               bool;
typedef char              char8;
typedef unsigned char     uchar8;
typedef char              int8;
typedef unsigned char     uint8;
typedef short int         int16;
typedef unsigned short int uint16;
typedef long int          int32;
typedef unsigned long int uint32;
typedef int               intn;
typedef unsigned int      uintn;
typedef float             float32;
typedef double            float64;
typedef int               intf;     /* size of INTEGERs in Fortran compiler */
#define _HUGE              /* This should only be defined to a value on the PC */
#define DFmovmem(from, to, len) memcpy(to, from, len)
#define _fcdtocp(desc) (desc)
#define FILELIB UNIXBUFIO
#ifndef __STDC__
#define const
#endif
#endif /* HP9000 */


#ifdef IRIS4

#ifdef GOT_MACHINE
If you get an error on this line more than one machine type has been defined.
Please check your Makefile.
#endif
#define GOT_MACHINE 1

#   define BSD
#include <string.h>
#ifndef __GNUC__
#include <memory.h>
#endif /* __GNUC__ */
#include <sys/file.h>               /* for unbuffered i/o stuff */
#define DF_MT              DFMT_IRIS4
typedef void               VOID;
typedef char               *VOIDP;
typedef char               *_fcd;
typedef int                bool;
typedef char               char8;
typedef unsigned char      uchar8;
typedef char               int8;
typedef unsigned char      uint8;
typedef short int          int16;
typedef unsigned short int uint16;
typedef long int           int32;
typedef unsigned long int  uint32;
typedef int                intn;
typedef unsigned int       uintn;
typedef float              float32;
typedef double             float64;
typedef int                intf;     /* size of INTEGERs in Fortran compiler */
#define _HUGE              /* This should only be defined to a value on the PC */
#define DFmovmem(from, to, len) memcpy(to, from, len)
#define FNAME_POST_UNDERSCORE
#define _fcdtocp(desc) (desc)
#define FILELIB UNIXBUFIO
#endif /* IRIS4 */


#ifdef UNICOS

#ifdef GOT_MACHINE
If you get an error on this line more than one machine type has been defined.
Please check your Makefile.
#endif
#define GOT_MACHINE 1

#include <string.h>
#include <memory.h>
#include <fortran.h>
#ifndef O_RDONLY
#include <fcntl.h>              /* for unbuffered i/o stuff */
#define L_INCR  1
#endif /*O_RDONLY*/

#define DF_MT   DFMT_UNICOS
typedef void            VOID;
typedef char            *VOIDP;
typedef char            *_fcd;
typedef int             bool;
typedef char            char8;
typedef unsigned char   uchar8;
typedef char            int8;
typedef unsigned char   uint8;
typedef int             int16;
typedef unsigned int    uint16;
typedef int             int32;
typedef unsigned int    uint32;
typedef int             intn;
typedef unsigned int    uintn;
typedef float           float32;
typedef double          float64;
typedef int             intf;     /* size of INTEGERs in Fortran compiler */
#define _HUGE              /* This should only be defined to a value on the PC */

#define DFmovmem(from, to, len) memcpy(to, from, len)
#define DF_CAPFNAMES            /* fortran names are in all caps */
#define FILELIB UNIXBUFIO

#endif /* UNICOS */

#ifdef VMS

#ifdef GOT_MACHINE
If you get an error on this line more than one machine type has been defined.
Please check your Makefile.
#endif
#define GOT_MACHINE 1
#include <file.h>               /* for unbuffered i/o stuff */

#define DF_MT              DFMT_VAX
typedef int                VOID;
typedef char               *VOIDP;
typedef char               *_fcd;
typedef int                bool;
typedef char               char8;
typedef unsigned char      uchar8;
typedef char               int8;
typedef unsigned char      uint8;
typedef short int          int16;
typedef unsigned short int uint16;
typedef long int           int32;
typedef unsigned int       uint32;
typedef int                intn;
typedef unsigned int       uintn;
typedef float              float32;
typedef double             float64;
typedef int                intf;     /* size of INTEGERs in Fortran compiler */
#define _HUGE              /* This should only be defined to a value on the PC */
#define DFmovmem(from, to, len) memcpy(to, from, len)
#define _fcdtocp(desc)  ((char *) *((char **) &desc[4]))

/* 
  Redef a couple of C routine names to avoid conflicts
  since the VMS link command is case-insensitive
*/
#define FILELIB UNIXBUFIO
#include "dfivms.h"

#endif /* VMS */

/* For Convex machines with native format floats */
#ifdef CONVEXNATIVE
#   define CONVEX
#endif

#ifdef CONVEX

#ifdef GOT_MACHINE
If you get an error on this line more than one machine type has been defined.
Please check your Makefile.
#endif
#define GOT_MACHINE

#include <sys/file.h>               /* for unbuffered i/o stuff */
#include <stdlib.h>

#define __STDC__					/* To invoke ANSI compilation */
#ifndef PROTOTYPE
#define PROTOTYPE 					/* to invoke ANSI compilation */
#endif /* PROTOTYPE */

#ifdef CONVEXNATIVE
#define DF_MT             DFMT_CONVEXNATIVE
#else
#define DF_MT             DFMT_CONVEX
#endif
typedef void              VOID;
typedef char              *VOIDP;
typedef char              *_fcd;
typedef int               bool;
typedef char              char8;
typedef unsigned char     uchar8;
typedef char              int8;
typedef unsigned char     uint8;
typedef short int         int16;
typedef unsigned short int uint16;
typedef long int          int32;
typedef unsigned long int uint32;
typedef int               intn;
typedef unsigned int      uintn;
typedef float             float32;
typedef double            float64;
typedef int               intf;     /* size of INTEGERs in Fortran compiler */
#define _HUGE              /* This should only be defined to a value on the PC */
#define DFmovmem(from, to, len) bcopy(from, to, len)
#define FNAME_POST_UNDERSCORE
#define _fcdtocp(desc) (desc)
#define FILELIB UNIXBUFIO
#endif /* CONVEX */


#ifdef MIPSEL

#ifdef GOT_MACHINE
If you get an error on this line more than one machine type has been defined.
Please check your Makefile.
#endif
#define GOT_MACHINE 1

#include <sys/types.h>
#include <sys/file.h>               /* for unbuffered i/o stuff */
#define DF_MT   DFMT_MIPSEL
typedef void            VOID;
typedef char            *VOIDP;
typedef char            *_fcd;
typedef int             bool;
typedef char            char8;
typedef unsigned char   uchar8;
typedef char            int8;
typedef unsigned char   uint8;
typedef short           int16;
typedef unsigned short  uint16;
typedef int             int32;
typedef unsigned int    uint32;
typedef int             intn;
typedef unsigned int    uintn;
typedef float           float32;
typedef double          float64;
typedef int             intf;     /* size of INTEGERs in Fortran compiler */
#define _HUGE              /* This should only be defined to a value on the PC */
#define DFmovmem(from, to, len) memcpy(to, from, len)
#define _fcdtocp(desc) (desc)
#define FNAME_POST_UNDERSCORE
#define FILELIB UNIXBUFIO
#ifndef __STDC__
#define const
#endif /* __STDC__ */
#endif /* MIPSEL */

#ifdef MAC

#ifdef GOT_MACHINE
If you get an error on this line more than one machine type has been defined.
Please check your Makefile.
#endif
#define GOT_MACHINE 1
#define PROTOTYPE

#include <memory.h>             /* malloc stuff for MPW 3.0 */
#include <fcntl.h>              /* unbuffered IO stuff for MPW 3.0 */
#include <String.h>
#ifdef THINK_C                  /* for LightSpeed C */
#include <unix.h>
#define isascii(c)  (isprint(c) || iscntrl(c))
#else /*THINK_C                   MPW, possibly others */
#include <Files.h>              /* for unbuffered i/o stuff */
#endif /*THINK_C*/
#define	DF_CAPFNAMES            /* fortran names are in all caps */
#define DF_DYNAMIC				/* use dynamic allocation */
#define DFmovmem(from, to, len) memcpy(to, from, len)
#define malloc(x)   NewPtr((Size)   (x))    /* don't use malloc on the Mac */
#define free(x)     DisposPtr((Ptr) (x))    /* don't use free on the Mac   */
#define DF_MT   DFMT_MAC

typedef void              VOID;
typedef char              *VOIDP;
typedef char              *_fcd;
typedef int               bool;
typedef char              char8;
typedef unsigned char     uchar8;
typedef char              int8;
typedef unsigned char     uint8;
typedef short int         int16;
typedef unsigned short int uint16;
typedef long int          int32;
typedef unsigned long int uint32;
typedef int               intn;
typedef unsigned int      uintn;
typedef float             float32;
typedef double            float64;
typedef int               intf;     /* size of INTEGERs in Fortran compiler */
#define _HUGE              /* This should only be defined to a value on the PC */
#define _fcdtocp(desc) (desc)
void exit(int status);

#define register   /* don't mess with the MAC compiler's register allocation */
#define FILELIB MACIO
#endif /*MAC*/


#ifdef WIN3
#define PC
#endif  /* WIN3 */

#ifdef PC

#ifdef GOT_MACHINE
If you get an error on this line more than one machine type has been defined.
Please check your Makefile.
#endif
#define GOT_MACHINE 1

#include <fcntl.h>
#include <sys\types.h>      /* for unbuffered file I/O */
#include <sys\stat.h>
#include <io.h>
#include <malloc.h>
#include <conio.h>          /* for debugging getch() calls */
#include <string.h>         /* for vaious string functions */
#include <limits.h>         /* for UINT_MAX used in various places */
#ifdef WIN3
#ifndef GMEM_MOVEABLE       /* check if windows header is already included */
#include <windows.h>        /* include the windows headers */
#endif
#endif /* WIN3 */

#define DF_MT             DFMT_PC

#ifndef PROTOTYPE
/* Want prototypes for the PC even if we're not completely ANSI C compliant */
#define PROTOTYPE
#endif  /* !PROTOTYPE */

#ifndef VOID    /* The stupid windows.h header file uses a #define instead of a typedef */
typedef void              VOID;
#endif  /* end VOID */
typedef void              huge *VOIDP;
typedef char              huge *_fcd;
typedef int               bool;
typedef char              char8;
typedef unsigned char     uchar8;
typedef char              int8;
typedef unsigned char     uint8;
typedef short int         int16;
typedef unsigned short int uint16;
typedef long int          int32;
typedef unsigned long int uint32;
typedef int               intn;
typedef unsigned int      uintn;
typedef float             float32;
typedef double            float64;
typedef long              intf;     /* size of INTEGERs in Fortran compiler */
/* used to force the prototypes in hproto.h to use huge pointers */
#define _HUGE              huge      

#ifdef WIN3
#define DFmovmem(from, to, len) _fmemmove(to, from, len)
#else   /* !WIN3 */
#define DFmovmem(from, to, len) memcpy(to, from, len)
#endif  /* WIN3 */

#define _fcdtocp(desc) (desc)

#define register    /* don't mess with the PC compiler's register allocation */
#ifdef WIN3
#define FILELIB WINIO
#else /* !WIN3 */
#define FILELIB PCIO
#endif /* WIN3 */
#endif /* PC */

#ifdef NEXT

#ifdef GOT_MACHINE
If you get an error on this line more than one machine type has been defined.
Please check your Makefile.
#endif
#define GOT_MACHINE

#include <string.h>
#ifndef __GNUC__
#include <memory.h>
#endif /* __GNUC__ */
#include <sys/file.h>               /* for unbuffered i/o stuff */
#define DF_MT             DFMT_NEXT
typedef void              VOID;
typedef char              *VOIDP;
typedef char              *_fcd;
typedef int               bool;
typedef char              char8;
typedef unsigned char     uchar8;
typedef char              int8;
typedef unsigned char     uint8;
typedef short int         int16;
typedef unsigned short int uint16;
typedef long int          int32;
typedef unsigned long int uint32;
typedef int               intn;
typedef unsigned int      uintn;
typedef int               intf;     /* size of INTEGERs in Fortran compiler */
typedef float             float32;
typedef double            float64;
#define _HUGE              /* This should only be defined to a value on the PC */
#define DFmovmem(from, to, len) memcpy(to, from, len)
#define FNAME_POST_UNDERSCORE
#define _fcdtocp(desc) (desc)
#define FILELIB UNIXBUFIO
#endif /* NEXT */


#ifndef GOT_MACHINE
No machine type has been defined.  Your Makefile needs to have someing like
-DSUN or -DUNICOS in order for the HDF internal structures to be defined
correctly.
#endif

#ifdef MOTOROLA

#ifdef GOT_MACHINE
If you get an error on this line more than one machine type has been defined.
Please check your Makefile.
#endif
#define GOT_MACHINE

#include <string.h>
#ifndef __GNUC__
#include <memory.h>
#endif /* __GNUC__ */
#include <unistd.h>
#include <sys/file.h>               /* for unbuffered i/o stuff */
#ifndef O_RDONLY
#include <fcntl.h>              /* for unbuffered i/o stuff */
#endif /*O_RDONLY*/
#define DF_MT             DFMT_MOTOROLA
typedef void              VOID;
typedef char              *VOIDP;
typedef char              *_fcd;
typedef int               bool;
typedef char              char8;
typedef unsigned char     uchar8;
typedef char              int8;
typedef unsigned char     uint8;
typedef short int         int16;
typedef unsigned short int uint16;
typedef long int          int32;
typedef unsigned long int uint32;
typedef int               intn;
typedef unsigned int      uintn;
typedef int               intf;     /* size of INTEGERs in Fortran compiler */
typedef float             float32;
typedef double            float64;
#define _HUGE              /* This should only be defined to a value on the PC */
#define DFmovmem(from, to, len) memcpy(to, from, len)
#define FNAME_POST_UNDERSCORE
#define _fcdtocp(desc) (desc)
#define FILELIB UNIXBUFIO
#ifndef __STDC__
#define const
#endif
#endif /* MOTOROLA */


/*-----------------------------------------------------*/
/*              encode and decode macros               */
/*-----------------------------------------------------*/

#   define INT16ENCODE(p, i) \
{ *(p) = (uint8)(((uint16)(i) >> 8) & 0xff); (p)++; \
        *(p) = (uint8)((uint16)(i) & 0xff); (p)++; }

#   define UINT16ENCODE(p, i) \
{ *(p) = (uint8)(((uint16)(i) >> 8) & 0xff); (p)++; *(p) = (uint8)((i) & 0xff); (p)++; }

#   define INT32ENCODE(p, i) \
{ *(p) = (uint8)(((uint32)(i) >> 24) & 0xff); (p)++; \
        *(p) = (uint8)(((uint32)(i) >> 16) & 0xff); (p)++; \
        *(p) = (uint8)(((uint32)(i) >> 8) & 0xff); (p)++; \
        *(p) = (uint8)((uint32)(i) & 0xff); (p)++; }

#   define UINT32ENCODE(p, i) \
{ *(p) = (uint8)(((i) >> 24) & 0xff); (p)++; \
        *(p) = (uint8)(((i) >> 16) & 0xff); (p)++; \
        *(p) = (uint8)(((i) >> 8) & 0xff); (p)++; \
        *(p) = (uint8)((i) & 0xff); (p)++; }

#   define INT16DECODE(p, i) \
{ (i) = (*(p) & 0xff) << 8; (p)++; (i) |= (*(p) & 0xff); (p)++; }

#   define UINT16DECODE(p, i) \
{ (i) = ((*(p) & 0xff) << 8); (p)++; (i) |= (*(p) & 0xff); (p)++; }

#   define INT32DECODE(p, i) \
{ (i) = ((int32)(*(p) & 0xff) << 24); (p)++; \
        (i) |= ((int32)(*(p) & 0xff) << 16); (p)++; \
        (i) |= ((int32)(*(p) & 0xff) << 8); (p)++; \
        (i) |= (*(p) & 0xff); (p)++; }

#   define UINT32DECODE(p, i) \
{ (i) = ((uint32)(*(p) & 0xff) << 24); (p)++; \
        (i) |= ((uint32)(*(p) & 0xff) << 16); (p)++; \
        (i) |= ((uint32)(*(p) & 0xff) << 8); (p)++; \
        (i) |= (*(p) & 0xff); (p)++; }

/**************************************************************************
*                   Conversion Routine Pointers
***************************************************************************/
#    ifndef DFKMASTER
#if defined __STDC__ || defined PC
extern int (*DFKnumin)(void _HUGE * source, void _HUGE * dest, uint32 num_elm,
            uint32 source_stride,uint32 dest_stride);
extern int (*DFKnumout)(void _HUGE * source, void _HUGE * dest, uint32 num_elm,
            uint32 source_stride,uint32 dest_stride);
#else
extern int (*DFKnumin)();
extern int (*DFKnumout)();
#endif
#     endif /* DFKMASTER */

/*****************************************************************
*  Create DFtbuf: a temporary buffer used when a small amount of
*  memory is needed, as when small conversions are done
******************************************************************/
#define DF_TBUFSZ       512     /* buffer size can be smaller */
#ifdef  HMASTER
    int    int_DFtbuf[DF_TBUFSZ]; /* int declaration to force word boundary */
    uint8  *DFtbuf = (uint8 *) int_DFtbuf;
#else /* !HMASTER */
extern uint8 *DFtbuf;
#endif /*HMASTER*/

/*----------------------------------------------------------------
** MACRO FCALLKEYW for any special fortran-C stub keyword
**
** MacIntosh MPW LS-fortran needs pascal since it can interface
**  best with pascal functions.
** Microsoft C and Fortran need _fortran for Fortran callable C
**  routines.
**
** MACRO FRETVAL for any return value from a fortran-C stub function
**  Replaces the older FCALLKEYW macro.
**---------------------------------------------------------------*/
#ifdef FRETVAL
#undef FRETVAL
#endif

#if defined(MAC)                /* with LS FORTRAN */
#   define FCALLKEYW    pascal
#   define FRETVAL(x)   pascal x
#endif

#if defined(PC)   /* with MS Fortran */
#   define FCALLKEYW    _fortran
#   define FRETVAL(x)   x _fortran
#endif

#ifndef FRETVAL /* !MAC && !PC */
#   define FCALLKEYW    /*NONE*/
#   define FRETVAL(x)   x
#endif


/*----------------------------------------------------------------
** MACRO FNAME for any fortran callable routine name.
**
**  This macro prepends, appends, or does not modify a name
**  passed as a macro parameter to it based on the FNAME_PRE_UNDERSCORE,
**  FNAME_POST_UNDERSCORE macros set for a specific system.
**
**---------------------------------------------------------------*/
#if defined(FNAME_PRE_UNDERSCORE) && defined(FNAME_POST_UNDERSCORE)
#ifdef __STDC__
#   define FNAME(x)     _##x##_
#else
#   define FNAME(x)     _/**/x/**/_
#endif
#endif
#if defined(FNAME_PRE_UNDERSCORE) && !defined(FNAME_POST_UNDERSCORE)
#ifdef __STDC__
#   define FNAME(x)     _##x
#else
#   define FNAME(x)     _/**/x
#endif
#endif
#if !defined(FNAME_PRE_UNDERSCORE) && defined(FNAME_POST_UNDERSCORE)
#ifdef __STDC__
#   define FNAME(x)     x##_
#else
#   define FNAME(x)     x/**/_
#endif
#endif
#if !defined(FNAME_PRE_UNDERSCORE) && !defined(FNAME_POST_UNDERSCORE)
#   define FNAME(x)     x
#endif

/**************************************************************************
*  String functions defined differently under MS Windows
**************************************************************************/

#ifdef WIN3
#  define HDstrcat(s1,s2)   (_fstrcat((s1),(s2)))
#  define HDstrcmp(s,t)     (_fstrcmp((s),(t)))
#  define HDstrcpy(s,d)     (_fstrcpy((s),(d)))
#  define HDstrlen(s)       (_fstrlen(s))
#  define HDstrncmp(s1,s2,n)    (_fstrncmp((s1),(s2),(n)))
#  define HDstrncpy(s1,s2,n)    (_fstrncpy((s1),(s2),(n)))
#else
#  define HDstrcat(s1,s2)   (strcat((s1),(s2)))
#  define HDstrcmp(s,t)     (strcmp((s),(t)))
#  define HDstrcpy(s,d)     (strcpy((s),(d)))
#  define HDstrlen(s)       (strlen(s))
#  define HDstrncmp(s1,s2,n)    (strncmp((s1),(s2),(n)))
#  define HDstrncpy(s1,s2,n)    (strncpy((s1),(s2),(n)))
#endif /* WIN3 */


/**************************************************************************
*  Memory functions defined differently under MS Windows
**************************************************************************/

#ifdef WIN3
#  define HDmemcpy(dst,src,n) (_fmemcpy((dst),(src),(n)))
#else
#  define HDmemcpy(dst,src,n) (memcpy((dst),(src),(n)))
#endif /* WIN3 */

#endif /* HDFI_H */
