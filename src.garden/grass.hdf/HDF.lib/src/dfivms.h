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
$Header: /hdf/hdf/v3.2r2/src/RCS/dfivms.h,v 1.4 1992/10/27 22:18:31 koziol beta $

$Log: dfivms.h,v $
 * Revision 1.4  1992/10/27  22:18:31  koziol
 * deleted reference to DFtransarray
 *
 * Revision 1.3  1992/10/23  00:14:11  koziol
 * Changed all DFIstr*() and DFImem*() calls to HDstr*() and HDmem*() calls
 * #ifdef'd out the macros Jason defined for Hopen, Hclose, etc. for Vsets
 * Replaced Vset VFREESPACE and VGETSPACE calls with actual calls to HDfreespace
 * and HDgetspace
 * Added a MS-Windows lower lower for file I/O (which may not be completely working
 *
 * Revision 1.1  1992/08/25  21:40:44  koziol
 * Initial revision
 *
*/
#ifndef DFIVMS_H
#define DFIVMS_H

#define DFANIaddentry _DFANIaddentry
#define DFANIgetann _DFANIgetann
#define DFANIgetannlen _DFANIgetannlen
#define DFANIlablist _DFANIlablist
#define DFANIlocate _DFANIlocate
#define DFANIopen _DFANIopen
#define DFANIputann _DFANIputann
#define DFANgetdesc _DFANgetdesc
#define DFANgetdesclen _DFANgetdesclen
#define DFANgetlabel _DFANgetlabel
#define DFANgetlablen _DFANgetlablen
#define DFANlablist _DFANlablist
#define DFANlastref _DFANlastref
#define DFANputdesc _DFANputdesc
#define DFANputlabel _DFANputlabel
#define DFANaddfid _DFANaddfid
#define DFANgetfidlen _DFANgetfidlen
#define DFANgetfid _DFANgetfid
#define DFANgetfdslen _DFANgetfdslen
#define DFANgetfds _DFANgetfds
#define DFANaddfds _DFANaddfds
#define DFCimcomp _DFCimcomp
#define DFCrle _DFCrle
#define DFCunimcomp _DFCunimcomp
#define DFCunrle _DFCunrle
#define DFDIgetgroup _DFDIgetgroup
#define DFDIputgroup _DFDIputgroup
#define DFIc2fstr _DFIc2fstr
#define DFIcheck _DFIcheck
#define DFIemptyDD _DFIemptyDD
#define DFIerr _DFIerr
#define DFIf2cstring _DFIf2cstring
#define DFIfind _DFIfind
#define DFIfreespace _DFIfreespace
#define DFIgetspace _DFIgetspace
#define DFImemcopy _DFImemcopy
#define DFIseedDDs _DFIseedDDs
#define DFIstrncpy _DFIstrncpy
#define DFPIopen _DFPIopen
#define DFPgetpal _DFPgetpal
#define DFPlastref _DFPlastref
#define DFPnpals _DFPnpals
#define DFPputpal _DFPputpal
#define DFPaddpal _DFPaddpal
#define DFPreadref _DFPreadref
#define DFPrestart _DFPrestart
#define DFPwriteref _DFPwriteref
#define DFR8Iopen _DFR8Iopen
#define DFR8Iputimage _DFR8Iputimage
#define DFR8Iriginfo _DFR8Iriginfo
#define DFR8addimage _DFR8addimage
#define DFR8getdims _DFR8getdims
#define DFR8getimage _DFR8getimage
#define DFR8getrig _DFR8getrig
#define DFR8lastref _DFR8lastref
#define DFR8nimages _DFR8nimages
#define DFR8putimage _DFR8putimage
#define DFR8putrig _DFR8putrig
#define DFR8readref _DFR8readref
#define DFR8restart _DFR8restart
#define DFR8setpalette _DFR8setpalette
#define DFR8writeref _DFR8writeref
#define DFSDIclear _DFSDIclear
#define DFSDIgetdata _DFSDIgetdata
#define DFSDIgetslice _DFSDIgetslice
#define DFSDIopen _DFSDIopen
#define DFSDIputdata _DFSDIputdata
#define DFSDIputslice _DFSDIputslice
#define DFSDIsdginfo _DFSDIsdginfo
#define DFSDadddata _DFSDadddata
#define DFSDclear _DFSDclear
#define DFSDendslice _DFSDendslice
#define DFSDgetNT _DFSDgetNT
#define DFSDgetdata _DFSDgetdata
#define DFSDgetdatalen _DFSDgetdatalen
#define DFSDgetdatastrs _DFSDgetdatastrs
#define DFSDgetdimlen _DFSDgetdimlen
#define DFSDgetdims _DFSDgetdims
#define DFSDgetdimscale _DFSDgetdimscale
#define DFSDgetdimstrs _DFSDgetdimstrs
#define DFSDgetrange _DFSDgetrange
#define DFSDgetsdg _DFSDgetsdg
#define DFSDgetslice _DFSDgetslice
#define DFSDlastref _DFSDlastref
#define DFSDnumber _DFSDnumber
#define DFSDpre32 _DFSDpre32
#define DFSDputdata _DFSDputdata
#define DFSDputsdg _DFSDputsdg
#define DFSDputslice _DFSDputslice
#define DFSDrestart _DFSDrestart
#define DFSDsetNT _DFSDsetNT
#define DFSDsetdatastrs _DFSDsetdatastrs
#define DFSDsetdims _DFSDsetdims
#define DFSDsetdimscale _DFSDsetdimscale
#define DFSDsetdimstrs _DFSDsetdimstrs
#define DFSDsetlengths _DFSDsetlengths
#define DFSDsetorder _DFSDsetorder
#define DFSDsetrange _DFSDsetrange
#define DFSDsettype _DFSDsettype
#define DFSDstartslice _DFSDstartslice
#define DFSDreadref _DFSDreadref
#define DFaccess _DFaccess
#define DFIaccess _DFIaccess
#define DFclose _DFclose
#define DFdel _DFdel
#define DFdescriptors _DFdescriptors
#define DFdiget _DFdiget
#define DFdiput _DFdiput
#define DFdiread _DFdiread
#define DFdisetup _DFdisetup
#define DFdiwrite _DFdiwrite
#define DFdup _DFdup
#define DFerrno _DFerrno
#define DFfind _DFfind
#define DFgetcomp _DFgetcomp
#define DFgetelement _DFgetelement
#define DFishdf _DFishdf
#define DFnewref _DFnewref
#define DFnumber _DFnumber
#define DFopen _DFopen
#define DFputcomp _DFputcomp
#define DFputelement _DFputelement
#define DFread _DFread
#define DFseek _DFseek
#define DFsetfind _DFsetfind
#define DFstart _DFstart
#define DFstat _DFstat
#define DFupdate _DFupdate
#define DFwrite _DFwrite
#define DFfindnextref _DFfindnextref
#define DF24putimage _DF24putimage
#define DF24addimage _DF24addimage
#define DF24getdims _DF24getdims
#define DF24getimage _DF24getimage
#define DF24reqil _DF24reqil
#define DF24setdims _DF24setdims
#define DF24setil _DF24setil
#define DF24restart _DF24restart
#define DF24readref _DF24readref
#define DFGRIgetdims _DFGRIgetdims
#define DFGRIgetimlut _DFGRIgetimlut
#define DFGRIopen _DFGRIopen
#define DFGRIreqil _DFGRIreqil
#define DFGRIriginfo _DFGRIriginfo
#define DFGRaddimage _DFGRaddimage
#define DFGRaddlut _DFGRaddlut
#define DFGRaddrig _DFGRaddrig
#define DFGRgetimage _DFGRgetimage
#define DFGRgetimdims _DFGRgetimdims
#define DFGRgetlut _DFGRgetlut
#define DFGRgetlutdims _DFGRgetlutdims
#define DFGRgetrig _DFGRgetrig
#define DFGRreqimil _DFGRreqimil
#define DFGRreqlutil _DFGRreqlutil
#define DFGRsetcompress _DFGRsetcompress
#define DFGRsetimdims _DFGRsetimdims
#define DFGRsetlut _DFGRsetlut
#define DFGRsetlutdims _DFGRsetlutdims
#define DFUfptoimage _DFUfptoimage
#define Hclose _Hclose
#define Hopen _Hopen

#endif  /* DFIVMS_H */

