
##################################################################
# DO NOT EDIT BELOW
me=$0
if test "$SRC" = ""
then
    echo `basename $me` - SRC variable NULL or not set
    exit 1
fi
if test "$CMD" = ""
then
    echo `basename $me` - CMD variable NULL or not set
    exit 1
fi
if test "$HEADER" = ""
then
    echo `basename $me` - HEADER variable NULL or not set
    exit 1
fi

all=no
parseonly=no
while test $# != 0
do
    case "$1" in
	-sh) parseonly=sh;shift;;
	-csh) parseonly=csh;shift;;
	-v) GMAKE_VERBOSE=no;export GMAKE_VERBOSE;shift;;
	-all) all=yes;shift;;
	 *) break
    esac
done

HEAD=$CMD/head/$HEADER
MID=$CMD/generic/make.mid
TAIL=$CMD/generic/make.tail

if test ! -d "$SRC"
then
    echo "SRC=$SRC - directory not found" >&2
    exit 1
fi
if test ! -r "$HEAD"
then
    echo "$HEAD - file not found (or not readable)" >&2
    exit 1
fi
if test ! -r "$MID"
then
    echo "$MID - file not found (or not readable)" >&2
    exit 1
fi

GISBASE="`sed 's/=/ /' $HEAD | awk '$1 ~ /^GISBASE$/ {if(NF>1)print $2}'`"
VERSION_NUMBER=
VERSION_DATE=
if test -r $CMD/VERSION
then
    VERSION_NUMBER="`awk '{print;exit}' $CMD/VERSION`"
    VERSION_DATE="`awk '{if(hit){print;exit}{hit=1}}' $CMD/VERSION`"
fi

if test "$GISBASE" = ""
then
    echo "GISBASE not set in header file $HEAD" >&2
    exit 1
fi

ARCH="`sed 's/=/ /' $HEAD | awk '$1 ~ /^ARCH$/ {if(NF>1)print $2}'`"


if test $parseonly = sh
then
    echo GISBASE=$GISBASE
    echo SRC=$SRC
    echo CMD=$CMD
    echo HEADER=$HEADER
    echo ARCH=$ARCH
    exit 0
fi

if test $parseonly = csh
then
    echo setenv GISBASE $GISBASE
    echo setenv SRC $SRC
    echo setenv CMD $CMD
    echo setenv HEADER $HEADER
    echo setenv ARCH $ARCH
    exit 0
fi

if test "$ARCH" = ""
then
    OBJARCH=OBJ  # force an object directory
    LIBARCH=LIB
else
    OBJARCH=OBJ.$ARCH
    LIBARCH=LIB.$ARCH
fi

if test $# -gt 0
then
    if test ! -d $1
    then
	echo "$1 - directory not found"
	exit 1
    fi
    cd $1
    shift
fi

if test ! "$GMAKE_VERBOSE" = no
then
    echo "  SRC     = $SRC"
    echo "  CMD     = $CMD"
    echo "  HEADER  = $HEADER"
    echo "  ARCH    = $ARCH"
    echo "  GISBASE = $GISBASE"
    echo "  VERSION = $VERSION_NUMBER $VERSION_DATE"
fi

if test $all = yes
then
    for dir in *
    do
	if test -r $dir/Gmakefile
	then
	    $me -v $dir || exit 1
	fi
    done
exit 0
fi

echo "#################################################################"
pwd
if test ! -r Gmakefile
then
    echo "Gmakefile not found (or not readable)"
    exit 1
fi

if grep -s '[({]GIS[)}]' Gmakefile
then
    echo "Old style Gmakefile - contains reference to GIS"
    echo "Modify it to use GISBASE or SRC"
    exit 1
fi

if grep -s '[({]GISDBASE[)}]' Gmakefile
then
    echo "Old style Gmakefile - contains reference to GISDBASE"
    echo "Modify it to use DEFAULT_DATABASE"
    exit 1
fi

# reject any explicit .o rules
badline="`awk '
	    /^ *[0-9a-zA-Z_\.\-]*\.o[ \	]*:/{line=NR;next}
	    /^\t./{if(line) {print line+1; exit}}
	    {line=0}' Gmakefile`"

if test ! "$badline" = ""
then
    echo Gmakefile line $badline - contains explicit .o action
    echo "Modify it to eliminate the action"
    exit 1
fi

if test ! -r "$TAIL"
then
    TAIL=/dev/null
fi

makefile=$OBJARCH/make.rules
if test ! -d $OBJARCH
then
    echo "  mkdir $OBJARCH"
    mkdir $OBJARCH || exit 1
fi

(
# build the make.rules file
# 
# define VERSION, SRC, OBJARCH for .o files, LIBARCH for .a files
echo VERSION_NUMBER="$VERSION_NUMBER"
echo VERSION_DATE="$VERSION_DATE"
echo SRC=$SRC
echo OBJARCH=$OBJARCH
echo LIBARCH=$LIBARCH
echo ""

# remove ARCH from $HEAD
awk '$1 !~ /^ARCH$/ {print}' $HEAD

# define gmake
echo GMAKE = $me

# prepend all .o with $(OBJARCH) and .a files with $(LIBARCH)
sed \
    -e 's#[\*0-9a-zA-Z_\.\-]*\.o[ \	]#$(OBJARCH)/&#g' \
    -e 's#[\*0-9a-zA-Z_\.\-]*\.o:[ \	]#$(OBJARCH)/&#g' \
    -e 's#[\*0-9a-zA-Z_\.\-]*\.o$#$(OBJARCH)/&#g' \
    -e 's#[\*0-9a-zA-Z_\.\-]*\.a[ \	]#$(LIBARCH)/&#g' \
    -e 's#[\*0-9a-zA-Z_\.\-]*\.a:[ \	]#$(LIBARCH)/&#g' \
    -e 's#[\*0-9a-zA-Z_\.\-]*\.a$#$(LIBARCH)/&#g' \
	$MID Gmakefile


echo ""
# extract .o files to be built from .c and .f files in the current directory
# and build explict make rules

sed -e 's/=/ /' -e 's/\\//' Gmakefile |\
 awk '{for(i=1;i<=NF;i++) \
    if ($i ~ /^[a-zA-Z0-9_\.\-]*\.o$/) print substr($i,1,length($i)-2)} ' |\
 sort -u |\
 (
    while read file
    do
	if test -f $file.f
	then
	    echo '$(OBJARCH)/'${file}.o: ${file}.f
	    echo '	rm -f $@'
	    echo '	$(FC) $(FFLAGS) -c' ${file}.f
	    echo '	mv' ${file}.o '$@'
	else
	    echo '$(OBJARCH)/'${file}.o: ${file}.c
	    echo '	rm -f $@'
	    echo '	$(CC) $(CFLAGS) -c' ${file}.c
	    echo '	mv' ${file}.o '$@'
	fi
    done
 )

cat $TAIL
) > $makefile

echo "  make -f $makefile $*"
echo ""
make -f $makefile $*
exit $?
