#!/bin/sh

###################################################
#
# DO NOT USE THIS SCRIPT!
#
# (unless you know what you are doing)
###################################################
# it is intented to tag GRASS stable releases
#
#
# There are three ways to include files: 
# a) two variables 
#       FURTHER_FILES=
#       FURTHER_DIRECTORIES=
#     which catch files and dirs not in
#       src/CMD/lists/GRASS
#
#  b) the 
#       MODULE_LIST= 
#     which covers the uncommented contents of
#      src/CMD/lists/GRASS
#
# Means: the src/CMD/lists/GRASS may only contain stable modules now!
#
###################################################
# Markus Neteler
# $Id$
###################################################

#enter branch tag here later:
# (note: no $,. allowed)
TAG="releasebranch_500"
BRANCH="releasebranch_500"

#catch all active modules from compile list:
MODULE_LIST=`cat src/CMD/lists/GRASS | grep -v '#'`

#list of extra files not in compile list:
FURTHER_FILES="config.guess \
	config.sub GPL.TXT testgrass.sh COPYING README SUBMITTING INSTALL \
	WARNING.html install-sh ONGOING REQUIREMENTS README.xdriver \
	binaryInstall.src README.cygwin Makefile.in AUTHORS BUGS TODO.txt \
	configure.in configure NEWS.html \
	src/libes/README"

#list of extra directories not in compile list:
FURTHER_DIRECTORIES="documents ALPHA64 testsuite tools unused \
	src/include src/CMD src/fonts src/gui src/sites/cdhc \
	src/libes/libimage src/libes/vect32_64/shapelib-1.2.8 \
	src/libes/dbmi/drivers/odbc src.garden/grass.postgresql\
	src/libes/gmath src/libes/ogsf"


#tag it...
cvs tag $TAG  $FURTHER_FILES $FURTHER_DIRECTORIES $MODULE_LIST

# branch it....
