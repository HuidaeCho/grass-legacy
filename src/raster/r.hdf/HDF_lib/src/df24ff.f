c***************************************************************************
c
c
c                         NCSA HDF version 3.2r3
c                            December 1, 1992
c
c NCSA HDF Version 3.2 source code and documentation are in the public
c domain.  Specifically, we give to the public domain all rights for future
c licensing of the source code, all resale rights, and all publishing rights.
c
c We ask, but do not require, that the following message be included in all
c derived works:
c
c Portions developed at the National Center for Supercomputing Applications at
c the University of Illinois at Urbana-Champaign, in collaboration with the
c Information Technology Institute of Singapore.
c
c THE UNIVERSITY OF ILLINOIS GIVES NO WARRANTY, EXPRESSED OR IMPLIED, FOR THE
c SOFTWARE AND/OR DOCUMENTATION PROVIDED, INCLUDING, WITHOUT LIMITATION,
c WARRANTY OF MERCHANTABILITY AND WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE
c
c***************************************************************************
C
C $Header: /hdf/hdf/v3.2r2/src/RCS/df24ff.f,v 1.1 1992/08/25 21:40:44 koziol beta $
C
C $Log: df24ff.f,v $
c Revision 1.1  1992/08/25  21:40:44  koziol
c Initial revision
c
C
C------------------------------------------------------------------------------
C File:     df24Ff.f
C Purpose:  read and write 24-bit raster images
C Invokes:  df24F.c
C Contents: 
C   d2gdims: get dimensions of image
C   d2gimg: read in image
C   d2aimg: write out image
C   d2rref: set reference number to be used for next get
C   DF24getdims:  get dimensions of image
C   DF24getimage: read in image
C   DF24getimage: write out image
C   DF24readref:  set reference number to be used for next get
C
C Remarks:  A RIG specifies attributes associated with an image - lookup table, 
C           dimension, compression, color compensation etc.
C----------------------------------------------------------------------------*/



C------------------------------------------------------------------------------
C Name: d2gdims
C Purpose:  get dimensions of next image RIG
C Inputs:   name: name of HDF file
C           xdim, ydim: locations for returning x,y dimensions
C           il: location for returning interlace of image in file
C Returns: 0 on success, -1 on failure with DFerror set
C           *pxdim, *pydim, *pil set on success
C Users:    HDF HLL (high-level library) users, utilities, other routines
C Invokes: d2igdim
C Remarks: none
C----------------------------------------------------------------------------*/

      integer function d2gdims(name, xdim, ydim, il)
      character*(*) name
      integer xdim, ydim, il, d2igdim

      d2gdims = d2igdim(name, xdim, ydim, il, len(name))
      return
      end


C------------------------------------------------------------------------------
C Name: d2gimg
C Purpose:  get image from next RIG
C Inputs:   name: name of HDF file
C           image: pointer to space to return image
C           xdim, ydim: dimensions of space to return image
C Returns: 0 on success, -1 on failure with DFerror set
C Users:    HDF HLL (high-level library) users, utilities, other routines
C Invokes: d2igimg
C Remarks: space is assumed to be xdim * ydim * 3 bytes
C----------------------------------------------------------------------------*/

      integer function d2gimg(name, image, xdim, ydim)
      character*(*) name, image
      integer xdim, ydim, d2igimg

      d2gimg = d2igimg(name, image, xdim, ydim, len(name))
      return
      end


C------------------------------------------------------------------------------
C Name: d2aimg
C Purpose:  Write out image
C Inputs:   filename: name of HDF file
C           image: image to write
C           xdim, ydim: dimensions of array image
C Returns: 0 on success, -1 on failure with DFerror set
C Users:    HDF HLL (high-level library) users, utilities, other routines
C Invokes: d2iaimg
C Remarks: array image is assumed to be xdim * ydim * ncomps bytes
C----------------------------------------------------------------------------*/

      integer function d2aimg(name, image, xdim, ydim)
      character*(*) name, image
      integer xdim, ydim, d2iaimg

      d2aimg = d2iaimg(name, image, xdim, ydim, len(name), 0)
      return
      end

      integer function d2pimg(name, image, xdim, ydim)
      character*(*) name, image
      integer xdim, ydim, d2iaimg

      d2pimg = d2iaimg(name, image, xdim, ydim, len(name), 1)
      return
      end

C------------------------------------------------------------------------------
C Name: d2rref
C Purpose:  Write out image
C Inputs:   filename: name of HDF file
C           ref:      reference number to be used for next get
C Returns: 0 on success, -1 on failure with DFerror set
C Users:    HDF HLL (high-level library) users, utilities, other routines
C Invokes: d2irref
C Remarks: array image is assumed to be xdim * ydim * ncomps bytes
C----------------------------------------------------------------------------*/

      integer function d2rref(name, ref)
      character*(*) name
      integer ref

      d2rref = d2irref(name, ref, len(name))
      return
      end


CEND7MAX


C------------------------------------------------------------------------------
C Name: DF24getdims
C Purpose:  get dimensions of next image RIG
C Inputs:   name: name of HDF file
C           xdim, ydim: locations for returning x,y dimensions
C           il: location for returning interlace of image in file
C Returns: 0 on success, -1 on failure with DFerror set
C           *pxdim, *pydim, *pil set on success
C Users:    HDF HLL (high-level library) users, utilities, other routines
C Invokes: d2igdim
C Remarks: none
C----------------------------------------------------------------------------*/

      integer function DF24getdims(name, xdim, ydim, il)
      character*(*) name
      integer xdim, ydim, il, d2igdim

      DF24getdims = d2igdim(name, xdim, ydim, il, len(name))
      return
      end


C------------------------------------------------------------------------------
C Name: DF24getimage
C Purpose:  get image from next RIG
C Inputs:   name: name of HDF file
C           image: pointer to space to return image
C           xdim, ydim: dimensions of space to return image
C Returns: 0 on success, -1 on failure with DFerror set
C Users:    HDF HLL (high-level library) users, utilities, other routines
C Invokes: d2igimg
C Remarks: space is assumed to be xdim * ydim * 3 bytes
C----------------------------------------------------------------------------*/

      integer function DF24getimage(name, image, xdim, ydim)
      character*(*) name, image
      integer xdim, ydim, d2igimg

      DF24getimage = d2igimg(name, image, xdim, ydim, len(name))
      return
      end


C------------------------------------------------------------------------------
C Name: DF24addimage
C Purpose:  Write out image
C Inputs:   filename: name of HDF file
C           image: image to write
C           xdim, ydim: dimensions of array image
C Returns: 0 on success, -1 on failure with DFerror set
C Users:    HDF HLL (high-level library) users, utilities, other routines
C Invokes: d2iaimg
C Remarks: array image is assumed to be xdim * ydim * ncomps bytes
C----------------------------------------------------------------------------*/

      integer function DF24addimage(name, image, xdim, ydim)
      character*(*) name, image
      integer xdim, ydim, d2iaimg

      DF24addimage = d2iaimg(name, image, xdim, ydim, len(name), 0)
      return
      end

      integer function DF24putimage(name, image, xdim, ydim)
      character*(*) name, image
      integer xdim, ydim, d2iaimg

      DF24putimage = d2iaimg(name, image, xdim, ydim, len(name), 1)
      return
      end


C------------------------------------------------------------------------------
C Name: df24readref
C Purpose:  Write out image
C Inputs:   filename: name of HDF file
C           ref:      reference number to be used for next get
C Returns: 0 on success, -1 on failure with DFerror set
C Users:    HDF HLL (high-level library) users, utilities, other routines
C Invokes: d2irref
C Remarks: array image is assumed to be xdim * ydim * ncomps bytes
C----------------------------------------------------------------------------*/

      integer function df24readref(name, ref)
      character*(*) name
      integer ref

      df24readref = d2irref(name, ref, len(name))
      return
      end

