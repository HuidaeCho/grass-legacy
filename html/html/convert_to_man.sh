#!/bin/sh
# Converting HTML file to MAN format, move into man1 directory
# Markus Neteler

#this will change in future: (testing purpose)
GISBASE=.

######### nothing to change below (hope so) ##############

TARGETDIR=$GISBASE/man/man_new

#create target directory:
if [ ! -d $TARGETDIR ]; then mkdir -p $TARGETDIR ; fi

# get list of files:
LIST_OF_HTMLS=`ls -1 *.html`

#do the conversion
for i in $LIST_OF_HTMLS ; do
  ../../src/scripts/contrib/g.html2man/g.html2man $i
  FILE=`echo $i | sed s/html=// | sed 's/\.html$//'`
  MANFILE=`echo $FILE | sed s/1=// | sed 's/\.1$//'`
  mv $MANFILE.1 $TARGETDIR/$MANFILE
done
