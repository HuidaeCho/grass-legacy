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
#ifdef RCSID
static char RcsId[] = "@(#)$Revision: 1.5 $";
#endif
/*
$Header: /hdf/hdf/v3.2r2/src/RCS/df24f.c,v 1.5 1992/10/30 18:36:52 koziol beta $

$Log: df24f.c,v $
 * Revision 1.5  1992/10/30  18:36:52  koziol
 * Fixed casts in a bunch of functions to make the SGI ANSI compiler happy
 *
 * Revision 1.4  1992/10/27  22:18:31  koziol
 * typo
 *
 * Revision 1.3  1992/10/12  18:11:51  koziol
 * Updated for v3.2r2 release
 *
 * Revision 1.2  1992/09/11  14:15:04  koziol
 * Changed Fortran stubs' parameter passing to use a new typedef, intf,
 * which should be typed to the size of an INTEGER*4 in whatever Fortran
 * compiler the C functions need to be compatible with.  (This is mostly
 * for the PC and possibly for the Mac)
 *
 * Revision 1.1  1992/08/25  21:40:44  koziol
 * Initial revision
 *
*/
/*-----------------------------------------------------------------------------
 * File:    df24F.c
 * Purpose: read and write 24-bit raster images
 * Invokes: dfgr.c df24.c
 * Contents: 
 *  d2reqil_: use this interlace when returning image
 *  df24reqil_: use this interlace when returning image
 *  d2sdims_: set dimensions of image
 *  df24setdims_: set dimensions of image
 *  d2setil_: set interlace for image
 *  df24setil_: set interlace for image
 *  d2first_: restart 24 bit raster
 *  df24restart_: restart 24 bit raster
 *  d2igdim_: get dimensions of image
 *  d2igimg_: read in image
 *  d2iaimg_: write out image
 *  d24lref_: last ref number
 *
 * Remarks:A RIG specifies attributes associated with an image - lookup table, 
 *          dimension, compression, color compensation etc.
 *---------------------------------------------------------------------------*/

#include "hdf.h"
#include "dfgr.h"

#ifndef DF24_FNAMES
#   define DF24_FNAMES
#ifdef DF_CAPFNAMES
#   define nd2reqil  FNAME(D2REQIL)
#   define ndf24reqil    FNAME(DF24REQIL)
#   define nd2sdims  FNAME(D2SDIMS)
#   define ndf24setdims  FNAME(DF24SETDIMS)
#   define nd2setil  FNAME(D2SETIL)
#   define ndf24setil    FNAME(DF24SETIL)
#   define nd2first  FNAME(D2FIRST)
#   define ndf24restart  FNAME(DF24RESTART)
#   define nd2igdim  FNAME(D2IGDIM)
#   define nd2igimg  FNAME(D2IGIMG)
#   define nd2iaimg  FNAME(D2IAIMG)
#   define nd2irref  FNAME(D2IRREF)
#else
#   define nd2reqil  FNAME(d2reqil)
#   define ndf24reqil    FNAME(df24reqil)
#   define nd2sdims  FNAME(d2sdims)
#   define ndf24setdims  FNAME(df24setdims)
#   define nd2setil  FNAME(d2setil)
#   define ndf24setil    FNAME(df24setil)
#   define nd2first  FNAME(d2first)
#   define ndf24restart  FNAME(df24restart)
#   define nd2igdim  FNAME(d2igdim)
#   define nd2igimg  FNAME(d2igimg)
#   define nd2iaimg  FNAME(d2iaimg)
#   define nd2irref  FNAME(d2irref)
#endif /* DF_CAPFNAMES */
#endif /* DF24_FNAMES */

#define LUT     0
#define IMAGE   1

static int dimsset = 0;


/*-----------------------------------------------------------------------------
 * Name:    d2reqil
 * Purpose: get next image with specified interlace
 * Inputs:  il: interlace to get next image with
 * Returns: 0 on success, -1 on failure with DFerror set
 * Users:   HDF HLL (high-level library) users, utilities, other routines
 * Invokes: DFGRIreqil
 * Remarks: none
 *---------------------------------------------------------------------------*/

    FRETVAL(intf)
#ifdef PROTOTYPE
nd2reqil(intf *il)
#else
nd2reqil(il)
    intf *il;
#endif /* PROTOTYPE */
{
    return(DFGRIreqil(*il, IMAGE));
}

/*-----------------------------------------------------------------------------
 * Name:    d2sdims
 * Purpose: set dimensions of image to write next
 * Inputs:  xdim, ydim: dimensions of image
 *          il: interlace of image
 * Returns: 0 on success, -1 on failure with DFerror set
 * Users:   HDF HLL (high-level library) users, utilities, other routines
 * Invokes: DFGRIsetdims
 * Remarks: none
 *---------------------------------------------------------------------------*/

    FRETVAL(intf)
#ifdef PROTOTYPE
nd2sdims(intf *xdim, intf *ydim)
#else
nd2sdims(xdim, ydim)
    intf *xdim, *ydim;
#endif /* PROTOTYPE */
{
    dimsset = 1;
    return(DFGRIsetdims(*xdim, *ydim, 3, IMAGE));
}


/*-----------------------------------------------------------------------------
 * Name:    d2igdim
 * Purpose: get dimensions of next image RIG
 * Inputs:  filename: name of HDF file
 *          pxdim, pydim: pointer to locations for returning x,y dimensions
 *          pil: location for returning interlace of image in file
 *          fnlen: length of filename
 * Returns: 0 on success, -1 on failure with DFerror set
 *          *pxdim, *pydim, *pil set on success
 * Users:   HDF HLL (high-level library) users, utilities, other routines
 * Invokes: DF24getdims
 * Remarks: none
 *---------------------------------------------------------------------------*/

    FRETVAL(intf)
#ifdef PROTOTYPE
nd2igdim(_fcd filename, intf *pxdim, intf *pydim, intf *pil, intf *fnlen)
#else
nd2igdim(filename, pxdim, pydim, pil, fnlen)
    _fcd filename;
    intf *pxdim, *pydim;
    intf *pil, *fnlen;
#endif /* PROTOTYPE */
{
    char *fn;
    intf ret;

    fn = HDf2cstring(filename, *fnlen);
    ret =  DF24getdims(fn, (int32 *)pxdim, (int32 *)pydim, (intn *)pil);
    HDfreespace(fn);
    return(ret);
}


/*-----------------------------------------------------------------------------
 * Name:    d2igimg
 * Purpose: get image from next RIG
 * Inputs:  filename: name of HDF file
 *          image: pointer to space to return image
 *          xdim, ydim: dimensions of space to return image
 *          fnlen: length of filename
 * Returns: 0 on success, -1 on failure with DFerror set
 * Users:   HDF HLL (high-level library) users, utilities, other routines
 * Invokes: DFGRIgetimlut
 * Remarks: space is assumed to be xdim * ydim * 3 bytes
 *---------------------------------------------------------------------------*/

    FRETVAL(intf)
#ifdef PROTOTYPE
nd2igimg(_fcd filename, _fcd image, intf *xdim, intf *ydim, intf *fnlen)
#else
nd2igimg(filename, image, xdim, ydim, fnlen)
    _fcd filename;
    _fcd image;
    intf *xdim, *ydim;
    intf *fnlen;
#endif /* PROTOTYPE */
{
    char *fn;
    intf ret;

    fn = HDf2cstring(filename, *fnlen);
    ret =  DF24getimage(fn, (VOIDP)_fcdtocp(image), *xdim, *ydim);
    HDfreespace(fn);
    return(ret);
}


/*-----------------------------------------------------------------------------
 * Name:    d2iaimg
 * Purpose: Write out image
 * Inputs:  filename: name of HDF file
 *          image: image to write
 *          xdim, ydim: dimensions of array image
 *          fnlen: length of filename
 * Returns: 0 on success, -1 on failure with DFerror set
 * Users:   HDF HLL (high-level library) users, utilities, other routines
 * Invokes: DFGRIaddimlut
 * Remarks: array image is assumed to be xdim * ydim * ncomps bytes
 *---------------------------------------------------------------------------*/

    FRETVAL(intf)
#ifdef PROTOTYPE
nd2iaimg(_fcd filename, _fcd image, intf *xdim, intf *ydim, intf *fnlen,
    intf *newfile)
#else
nd2iaimg(filename, image, xdim, ydim, fnlen, newfile)
    _fcd filename;
    _fcd image;
    intf *xdim, *ydim;
    intf *fnlen, *newfile;
#endif /* PROTOTYPE */
{
    char *fn;
    intf ret;

    if (!dimsset)
        if (DFGRIsetdims(*xdim, *ydim, 3, IMAGE)<0) return(-1);

    fn = HDf2cstring(filename, *fnlen);
    ret = DFGRIaddimlut(fn, (VOIDP)_fcdtocp(image), *xdim, *ydim,
            IMAGE, 1, *newfile);
    HDfreespace(fn);
    return(ret);
}


/*-----------------------------------------------------------------------------
 * Name:    d2setil
 * Purpose: set interlace store with following images
 * Inputs:  il: interlace to set
 * Returns: 0 on success, -1 on failure with DFerror set
 * Users:   HDF HLL (high-level library) users, utilities, other routines
 * Invokes: DFGRIsetil
 * Remarks: none
 *---------------------------------------------------------------------------*/

    FRETVAL(intf)
#ifdef PROTOTYPE
nd2setil(intf *il)
#else
nd2setil(il)
    intf *il;
#endif /* PROTOTYPE */
{
    return (DFGRIsetil(*il, IMAGE));
}

/*-----------------------------------------------------------------------------
 * Name:    df24first
 * Purpose: restart 24 bit raster file
 * Inputs:  
 * Returns: 0 on success, -1 on failure with DFerror set
 * Users:   HDF HLL (high-level library) users, utilities, other routines
 * Invokes: DFGRIrestart
 * Remarks: none
 *---------------------------------------------------------------------------*/

    FRETVAL(intf)
#ifdef PROTOTYPE
nd2first(void)
#else
nd2first()
#endif /* PROTOTYPE */
{
    return (DFGRIrestart());
}


/*-----------------------------------------------------------------------------
 * Name:    d24lref
 * Purpose: return last reference number 
 * Inputs:  
 * Returns: last ref number
 * Users:   HDF HLL (high-level library) users, utilities, other routines
 * Invokes: DFGRIrestart
 * Remarks: none
 *---------------------------------------------------------------------------*/

    FRETVAL(intf)
#ifdef PROTOTYPE
nd24lref(void)
#else
nd24lref()
#endif /* PROTOTYPE */
{
    return (DFGRIlastref());
}

/*-----------------------------------------------------------------------------
 * Name:    df24reqil
 * Purpose: get next image with specified interlace
 * Inputs:  il: interlace to get next image with
 * Returns: 0 on success, -1 on failure with DFerror set
 * Users:   HDF HLL (high-level library) users, utilities, other routines
 * Invokes: DFGRIreqil
 * Remarks: none
 *---------------------------------------------------------------------------*/

    FRETVAL(intf)
#ifdef PROTOTYPE
ndf24reqil(intf *il)
#else
ndf24reqil(il)
    intf *il;
#endif /* PROTOTYPE */
{
    return(DFGRIreqil(*il, IMAGE));
}

/*-----------------------------------------------------------------------------
 * Name:    df24setdims
 * Purpose: set dimensions of image to write next
 * Inputs:  xdim, ydim: dimensions of image
 *          il: interlace of image
 * Returns: 0 on success, -1 on failure with DFerror set
 * Users:   HDF HLL (high-level library) users, utilities, other routines
 * Invokes: DFGRIsetdims
 * Remarks: none
 *---------------------------------------------------------------------------*/

    FRETVAL(intf)
#ifdef PROTOTYPE
ndf24setdims(intf *xdim, intf *ydim)
#else
ndf24setdims(xdim, ydim)
    intf *xdim, *ydim;
#endif /* PROTOTYPE */
{
    dimsset = 1;
    return(DFGRIsetdims(*xdim, *ydim, 3, IMAGE));
}

/*-----------------------------------------------------------------------------
 * Name:    df24setil
 * Purpose: set interlace store with following images
 * Inputs:  il: interlace to set
 * Returns: 0 on success, -1 on failure with DFerror set
 * Users:   HDF HLL (high-level library) users, utilities, other routines
 * Invokes: DFGRIsetil
 * Remarks: none
 *---------------------------------------------------------------------------*/

    FRETVAL(intf)
#ifdef PROTOTYPE
ndf24setil(intf *il)
#else
ndf24setil(il)
    intf *il;
#endif /* PROTOTYPE */
{
    return (DFGRIsetil(*il, IMAGE));
}

/*-----------------------------------------------------------------------------
 * Name:    df24restart
 * Purpose: restart 24 bit raster file
 * Inputs:  
 * Returns: 0 on success, -1 on failure with DFerror set
 * Users:   HDF HLL (high-level library) users, utilities, other routines
 * Invokes: DFGRIrestart
 * Remarks: none
 *---------------------------------------------------------------------------*/

    FRETVAL(intf)
#ifdef PROTOTYPE
ndf24restart(void)
#else
ndf24restart()
#endif /* PROTOTYPE */
{
    return (DFGRIrestart());
}

/*-----------------------------------------------------------------------------
 * Name:    d2irref
 * Purpose: Internal stub for setting ref of rig to read next
 * Inputs:  filename: name of HDF file
 *          ref: reference
 *          fnlen: length of filename
 * Returns: 0 on success, -1 on failure with DFerror set
 * Users:   HDF HLL (high-level library) users, utilities, other routines
 * Invokes: DFGRreadref
 * Remarks: 
 *---------------------------------------------------------------------------*/

    FRETVAL(intf)
#ifdef PROTOTYPE
nd2irref(_fcd filename, intf *ref, intf *fnlen)
#else
nd2irref(filename, ref, fnlen)
    _fcd filename;
    intf *ref;
    intf *fnlen;
#endif /* PROTOTYPE */
{
    char *fn;
    intf ret;

    fn = HDf2cstring(filename, *fnlen);
    ret = DFGRreadref(fn, (uint16)*ref);
    HDfreespace(fn);
    return(ret);
}
