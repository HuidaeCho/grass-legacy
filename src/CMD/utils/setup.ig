:
if test ! -d utils
then
    cd ..
fi
if test ! -d utils
then
    echo "$0: you are running me from the wrong directory" >&2
    exit 1
fi

CC=cc

utils/printf %s Compiler to use [$CC]: 
read x
if test "$x" != ""
then
  CC=$x
fi

if $CC -o /tmp/$$ utils/cc.c
then
utils/printf "%s\n" Compiler looks good.
rm -f /tmp/$$
else
utils/printf "%s\n" There were errors. Go find your compiler and come back...
exit 1
fi

(cd utils
 rm -f access access.o
 $CC access.c -o access >/dev/null 2>/dev/null
 rm -f access.o
)

# reset the path now
PATH=utils:$PATH
export PATH

# if there is an external pwd, use it.
# we have found that internal pwd on some machines doesn't work
# properly with NFS mounted file systems
pwd=`findcmd -p pwd`
if test "$pwd" = ""
then
    pwd=pwd
fi


# clear all variables

########## UNIX_BIN ##############################################
UNIX_BIN=/usr/local/bin

# setup the defaults

HASX=True
ansi=False

SRC=
if test -d ../../src
then
    SRC=`(cd ../../src; $pwd)`
fi

DEFAULT_DATABASE=
if test -d ../../data
then
    DEFAULT_DATABASE=`(cd ../../data;$pwd)`
fi

COMPILE_FLAGS=-O ;     check_cc_flags=yes
LDFLAGS=-s       ;     check_ld_flags=yes
CURSES=-lcurses  ;     check_curses=yes

if test "$ansi" = "False"
then
XCFLAGS=-D_NO_PROTO		;	check_x_c_flags=yes
else
XCFLAGS=      			;	check_x_c_flags=yes
fi
XLDFLAGS=      			;	check_x_ld_flags=yes
XINCPATH=/usr/include/X11	;	check_x_inc_path=yes
XMINCPATH=/usr/include/Xm	;	check_xm_inc_path=yes
XLIBPATH=/usr/lib		;	check_x_lib_path=yes
XTLIBPATH=/usr/lib		;	check_xt_lib_path=yes
XMLIBPATH=/usr/lib		;	check_xm_lib_path=yes
XEXTRALIBS=			;	check_x_xtra_libs=yes
MATHLIB=-lm      ;     check_mathlib=yes

TERMLIB=
if findlib termlib
then
    TERMLIB=-ltermlib
    check_termlib=yes
fi
CLEAR=
GISBASE=
TERMIO=
DIGITFLAGS=
VECTLIBFLAGS=
MTIO=

######new placement of platform-specific questions and settings

IGRAPH=no

echo ""
MASSCOMP=no
if agree Is this a SUN system
then
    if findcmd arch
    then
	ARCH=`arch`
    fi
    TERMIO=no
    CLEAR=yes
    check_mathlib=no
    check_cc_flags=no
    check_ld_flags=no
    check_x_ld_flags=no
    check_curses=no
    check_termlib=no
    check_x_xtra_libs=no
elif agree Is this a MASSCOMP system
then
    DIGITFLAGS="$DIGITFLAGS -DMASSCOMP"
    TERMIO=yes
    CLEAR=yes
    check_mathlib=no
    check_cc_flags=no
    check_ld_flags=no
    check_x_ld_flags=no
    check_termlib=no
    check_curses=yes
elif agree Is this an Intergraph system
then
    XCFLAGS=-"DINTERGRAPH -DX11R3"
    XINCPATH=/usr/ip32/xwinprog
    XLIBPATH=/usr/ip32/xwinprog/lib
    DIGITFLAGS="$DIGITFLAGS -DINTERPRO"
	IGRAPH=yes
fi

######end platform-specific settings

######check compiler for ANSI compliance

printf "\n%s\n\n"  Checking your compiler for ANSI compliance.
if $CC -o /tmp/$$ utils/ansi.c 1>/dev/null 2>&1
then
  printf "%s\n"  Your compiler seems to be an Ansi-C compiler
  printf "%s\n"  COMPILE_FLAGS must be defined to make it
  printf "%s\n\n\n"  "behave like a K&R C compiler."
  ansi=True
else
  printf "%s\n\n"  You are OK.
  ansi=False
fi
rm -f /tmp/$$

###intergraph/Apogee-Clipper-specific COMPILE_FLAG setting
if test "$ansi" = True &&  test "$IGRAPH" = yes
then
   if agree Is your compiler an Apogee or Clipper Advanced Optimizing Compiler
   then
		 printf "%s\n\n" "COMPILE_FLAGS are being set for K&R compatibility" 
		 printf "%s\n" "You will have an opportuinity to change or delete"
		 printf "%s\n\n\n" "these settings later."
         COMPILE_FLAGS="$COMPILE_FLAGS -w -knr" 
   fi
fi

if test "$HASX" != "False"
then
  if test -d $XINCPATH
  then
	printf "%s\n\n" Found X11 include path -- $XINCPATH
  elif test -d /usr/local/include/X11
  then
    XINCPATH=/usr/local/include/X11
    printf "%s\n\n" Found X11 include path -- $XINCPATH
  else
    printf "%s\n" "I can't find your X11 include path."
    if agree Is X installed on your system
    then
      while true
      do
	printf "\n"
        printf %s "Path for X11 include files [RETURN for none] -->"
        read x
        if test "$x" != ""
        then
	  if access r $x/X11.h
          then
	    XINCPATH=$x
	    printf "%s\n\n" Found It.
	    break
          else
	    printf "%s\n\n" Invalid path.  Try again.
	  fi
	else
	  XINCPATH=
	  break
        fi
      done
    else
      HASX=False
    fi
  fi
fi
XINCPATH=`echo $XINCPATH | sed 's/\/X11$//'`
if test "$XINCPATH" = "/usr/include"
then
    XINCPATH=
fi
echo XINCPATH=$XINCPATH

if test "$HASX" != "False"
then
  if test -d $XMINCPATH
  then
    printf "%s\n\n" Found Motif Include path -- $XMINCPATH
  elif test -d $XINCPATH/Xm
  then
    XMINCPATH=$XINCPATH/Xm
    printf "%s\n\n" Found Motif Include path -- $XMINCPATH
    break
  else
    printf "%s\n" "I can't find your Motif include path."
    while true
    do
      printf "\n"
      printf %s "Path for Motif include files [RETURN for none] -->"
      read x
      if test "$x" != ""
      then
        if access r $x/Xm.h
        then
          XMINCPATH=$x
	  printf "%s\n\n" Found it.
          break
        else
          printf "%s\n\n" Invalid path.  Try again.
        fi
      else
	XMINCPATH=
	break
      fi
    done
  fi
fi
XMINCPATH=`echo $XMINCPATH | sed 's/\/Xm$//'`
if test "$XMINCPATH" = "/usr/include"
then
    XMINCPATH=
fi
echo XMINCPATH=$XMINCPATH

if test "$HASX" != "False"
then
  if access r $XLIBPATH/libX11.a
  then
	printf "%s\n\n" Found The X11 library -- $XLIBPATH
  else
    printf "%s\n" "I can't seem to find your X Library."
    while true
    do
      printf %s "Path to your X Library [RETURN for none] -->"
      read x
      if test "$x" != ""
      then
        if access r $x/libX11.a
        then
	  XLIBPATH=$x
   	  printf "%s\n\n" Found it.
	  break
        else
	  printf "%s\n" This is an invalid path.  Try again.
        fi
      else
	XLIBPATH=
	break;
      fi
    done
  fi
fi


if test "$HASX" != "False"
then
  if access r $XTLIBPATH/libXt.a
  then
    printf "%s\n\n" Found The  Intrinsics library -- $XTLIBPATH
  else
    printf "%s\n\n" "I can't seem to find your X Intrinsics Library."
    while true
    do
      printf %s "Path to your X Intrinsics Library [RETURN for none] -->"
      read x
      if test "$x" != ""
      then
        if access r $x/libXt.a
        then
	  XTLIBPATH=$x
	  printf "%s\n\n" Found it.
	  break
        else
	  printf "%s\n\n" This is an invalid path.  Try again.
        fi
      else
	XTLIBPATH=
	break
      fi
    done
  fi
fi

if test "$HASX" != "False"
then
  if access r $XMLIBPATH/libXm.a
  then
    printf "%s\n\n" Found The Motif library -- $XMLIBPATH
  else
    printf "%s\n\n" "I can't seem to find your X Motif Library."
    while true
    do
      printf %s "Path to your X Motif Library [RETURN for none] -->"
      read x
      if test "$x" != ""
      then
        if access r $x/libXm.a
        then
	  XMLIBPATH=$x
  	  printf "%s\n\n" Found it.
	  break
        else
	  printf "%s\n\n" This is an invalid path.  Try again.
        fi
      else
	XMLIBPATH=
	break
      fi
    done
  fi
fi

if test "$HASX" != "False"
then
  printf "%s\n" Now compiling a sample X program.  Watch for any errors.
  cd utils
  if $CC X.c -o /tmp/$$ -I$XINCPATH -L$XLIBPATH -lX11 -lbsd 
  then
    printf "%s\n" Looks good.
    rm -f /tmp/$$
  else
    printf "%s\n" There were errors.
  fi
  cd ..
fi



##################  Vector stuff ####################

cat <<  'EOF'

=============   Vector related setup



    GRASS 3.x forced digit (now v.digit) to run at high priority and
    as root.  If it was not, it would complain, but ask you if wished
    to go on.  You now have an option for GRASS 4.x:

EOF

if agreed YES 'Do you want v.digit to run at high priority as root [Y]'
then
    DIGITFLAGS="$DIGITFLAGS -DHIGHPRIORITY"
    echo '(YES)'
else
    echo '(NO)'
fi


cat << 'EOF'


       GRASS 4.x by default now supports a portable VECTOR data format. 

       GRASS 3.x only supported a portable data format for a few select
    installations which had an Intel 386 based machine (e.g. Sun 386i or
    Compaq 386) running networked with other GRASS machines.  Known places 
    where this was done include CERL, Southwest Division ACE, and possibly
    NPS Denver.  Just because you have a 386 machine does not mean you had 
    the GRASS 3.x portable code.
	The following question will determine the assumptions that GRASS 
    will make when dealing with 3.x format vector data on 386 machines.
	UNLESS you are compiling GRASS 4.x on a 386 based machine which 
    was ALREADY using the Portable code in GRASS 3.x, you should answer
    NO to the following question.

EOF

if agreed NO 'Did your system use the GRASS 3.x Portable Vector format [N]'
then
    VECTLIBFLAGS="$VECTLIBFLAGS -DPORTABLE_3"
    echo '(YES)'
else
    echo '(NO)'
fi

cat << 'EOF'
    
	On a similar subject, the portable vector data in GRASS 4.x will only 
    work on machines meeting the following criteria:

	    32 bit long integers
	    True IEEE floating point format

	Most machines that GRASS runs on fit these criteria, but some
    may not.  For example possibly CRAY computers, and VAX will not work
    correctly with GRASS 4.x portable vector code.  If you do not think
    your machine meets these criteria, you may want to answer NO to the
    following question.  If you answer NO, your binary vector files will 
    NOT be in the portable format, and will not be directly sharable with 
    other machines (e.g. if you have an NFS network).  Most people should 
    answer YES to the following question.

    To be sure, I am going to run a program, which will help in answering
    the question: 

    Press RETURN or ENTER to run the test program.
EOF
    rm -f portable.o
    $CC utils/portable.c -o /tmp/portable.$$ >/dev/null 2>/dev/null
    rm -f portable.o

# wait for return
    read x

if /tmp/portable.$$
then
    echo The program determined that you should answer the question YES.
else
    echo The program seems to have determined that you should answer the question NO.
fi

    rm -f /tmp/portable.$$
echo
echo

if agreed YES 'Do you want to use the Portable Vector format [Y]'
then
    echo '(YES)'
else
    VECTLIBFLAGS="$VECTLIBFLAGS -DNO_PORTABLE"
    echo '(NO)'
fi

##################  END New Vector stuff ####################

echo ""
echo "Checking out other things about your system. One moment"
echo ""

# termio.h vs sgtty.h
if test "$TERMIO" = ""
then
    rm -f termio.o sgtty.o
    rm -f /tmp/$$
    if $CC utils/termio.c -o /tmp/$$ >/dev/null 2>/dev/null 
    then
	TERMIO=yes
    fi

    rm -f /tmp/$$
    if $CC utils/sgtty.c -o /tmp/$$ >/dev/null 2>/dev/null && /tmp/$$
    then
	TERMIO=no
    fi
    rm -f termio.o sgtty.o
    rm -f /tmp/$$
fi
echo TERMIO=$TERMIO

# are setreuid(), seteuid(), setpriority() available?


rm -f seteuid.o setreuid.o setprior.o
rm -f /tmp/$$
if $CC utils/setreuid.c -o /tmp/$$ >/dev/null 2>/dev/null
then
    DIGITFLAGS="$DIGITFLAGS -DUSE_SETREUID"
fi
rm -f /tmp/$$
if $CC utils/seteuid.c -o /tmp/$$ >/dev/null 2>/dev/null
then
    DIGITFLAGS="$DIGITFLAGS -DUSE_SETEUID"
fi
rm -f /tmp/$$
if $CC utils/setprior.c -o /tmp/$$ >/dev/null 2>/dev/null
then
    DIGITFLAGS="$DIGITFLAGS -DUSE_SETPRIORITY"
fi
rm -f seteuid.o setreuid.o setprior.o
echo DIGITFLAGS="$DIGITFLAGS"

# mtio.h
if test "$MTIO" = ""
then
    rm -f /tmp/$$ mtio.o
    if $CC utils/mtio.c -o /tmp/$$ >/dev/null 2>/dev/null
    then
	MTIO=yes
    else
	MTIO=no
    fi
    rm -f /tmp/$$ mtio.o
fi
echo MTIO=$MTIO

# gethostname() or uname()
GETHOSTNAME=no
rm -f /tmp/$$
if $CC utils/gethost.c -o /tmp/$$ >/dev/null 2>/dev/null
then
	GETHOSTNAME=gethostname
fi
rm -f gethost.o
if test $GETHOSTNAME = no
then
    rm -f /tmp/$$
    if $CC utils/uname.c -o /tmp/$$ >/dev/null 2>/dev/null
    then
	GETHOSTNAME=uname
    fi
    rm -f uname.o
fi
echo GETHOSTNAME="$GETHOSTNAME"

cd utils
rm -f liba.o libb.o libmain.o

$CC -c liba.c
$CC -c libb.c
$CC -c libmain.c

rm -f lib.a a.out

ar ruv lib.a liba.o libb.o > /dev/null 2>/dev/null
if $CC libmain.o lib.a > /dev/null 2>/dev/null
then
	LIBRULE='ar ruv $@ $?'
elif findcmd ranlib
then
	rm -f a.out
	if ranlib lib.a >/dev/null 2>/dev/null
	then
	    if $CC libmain.o lib.a > /dev/null 2>/dev/null
	    then
		LIBRULE='ar ruv $@ $?; ranlib $@'
	    fi
	fi
fi
if test "$LIBRULE" = ""
then
	rm -f lib.a
	ar ruv lib.a liba.o libb.o > /dev/null 2>/dev/null
	if ar ts lib.a >/dev/null 2>/dev/null
	then
	    if $CC libmain.o lib.a > /dev/null 2>/dev/null
	    then
		LIBRULE='ar ruv $@ $?; ar ts $@'
	    fi
	else
	    LIBRULE='ar rc $@ `lorder $(OBJ) | tsort`'
	fi
fi
rm -f lib.a a.out liba.o libb.o libmain.o
cd ..

echo LIBRULE="$LIBRULE"

########## ARCH/HOST #############################################
MACHINE=
if findcmd hostname
then
    MACHINE=`hostname|awk -F. '{print $1}'`
fi

cat << 'EOF'

This release of GRASS supports compilation for multiple
machine/architectures from the same source directory.

EOF

if agree Do you want this feature enabled
then
#    echo ""
#    while true
#    do
#	echo "Enter a name for your machine"
#	echo ""
#	printf %s "MACHINE [$MACHINE] : "
#	read x
#	if test "$x" != ""
#	then
#	    MACHINE="$x"
#	fi
#	if test "$MACHINE" != ""
#	then
#	    break
#	fi
#    done
#    echo MACHINE = $MACHINE

    echo ""
    while true
    do
	echo "Enter the architecture type for this system"
	echo "What you enter will be used to name directories to"
	echo "hold .o and .a files, so enter a simple file name"
	echo ""
	printf %s "ARCH [$ARCH] : "
	read x
	if test "$x" != ""
	then
	    ARCH="$x"
	fi
	if test "$ARCH" != ""
	then
	    break
	fi
    done
    echo ARCH = $ARCH
else
    MACHINE=
    ARCH=
fi

########## -- loop -- ############################################
# this section gathers other info. user can repeat this section.
#
while true
do

########## UNIX_BIN ##############################################
cat << 'EOF'

Specify the Unix directory which will contain the GRASS startup
command. This should be a directory in every users' $PATH.

EOF

while true
do
    printf %s "UNIX_BIN [$UNIX_BIN] : "
    read x
    if test "$x" != ""
    then
	UNIX_BIN=$x
    fi
    if test ! -d $UNIX_BIN
    then
	echo ""
	echo "$UNIX_BIN - directory not found"
	if agree Create $UNIX_BIN
	then
	    makedir $UNIX_BIN
	    if test -d $UNIX_BIN
	    then
		echo $UNIX_BIN created
		echo ""
		break
	    fi
	fi
    elif access w $UNIX_BIN
    then
	break
    else
	echo ""
	echo "$UNIX_BIN - you do not have write permission"
	echo ""
    fi
done
echo UNIX_BIN = $UNIX_BIN

########## COMPILE_FLAGS #########################################

cat << 'EOF'

Specify the flags to be passed to the C-compiler.
Enter - if no flags are to be specified.

EOF


while true
do
    printf %s "COMPILE_FLAGS [$COMPILE_FLAGS] : "
    read x

    if test "$x" != ""
    then
	COMPILE_FLAGS="$x"
	check_cc_flags=yes
    fi

    if test "$COMPILE_FLAGS" = -
    then
	COMPILE_FLAGS=
	check_cc_flags=no
	break
    fi
    ok=yes
    x=
    for flag in $COMPILE_FLAGS
    do
	case "$flag" in
	    -*) ;;
	     *) flag=-$flag;ok=no;;
	esac
	if test "$x" = ""
	then
	    x=$flag
	else
	    x="$x $flag"
	fi
    done

    COMPILE_FLAGS="$x"
    if test $ok = no
    then
	check_cc_flags=yes
	echo ""
	echo "Please only specify -options"
	echo ""
	continue
    fi

    if test "$check_cc_flags" = yes
    then
	echo ""
	echo "I will check the compile flags now"
	echo "Please watch for error messages"
	rm -f /tmp/$$.c /tmp/$$.o
	cp utils/main.c /tmp/$$.c
	(cd /tmp; $CC $COMPILE_FLAGS -c $$.c)
	rm -f /tmp/$$.c /tmp/$$.o
	echo ""
	echo Check complete
	if agree Any problems
	then
	    continue
	fi
    fi

    break
done
echo COMPILE_FLAGS = "$COMPILE_FLAGS"
check_cc_flags=no

########## LDFLAGS ###############################################
cat << 'EOF'

Specify the flags to be passed to the loader (ld).
Enter - if no flags are to be specified.

EOF

while true
do
    printf %s "LDFLAGS [$LDFLAGS] : "
    read x
    if test "$x" != ""
    then
	LDFLAGS="$x"
	check_ld_flags=yes
    fi

    if test "$LDFLAGS" = -
    then
	LDFLAGS=
	check_ld_flags=no
	break
    fi
    ok=yes
    x=
    for flag in $LDFLAGS
    do
	case "$flag" in
	    -*) ;;
	     *) flag=-$flag;ok=no;;
	esac
	if test "$x" = ""
	then
	    x=$flag
	else
	    x="$x $flag"
	fi
    done
    LDFLAGS="$x"
    if test $ok = no
    then
	echo ""
	echo "Please only specify -options"
	echo ""
	check_ld_flags=yes
	continue
    fi

    if test "$check_ld_flags" = yes
    then
	echo ""
	echo "I will check the loader flags now"
	echo "Please watch for error messages"
	rm -f /tmp/$$.c /tmp/$$.o /tmp/$$
	cp utils/main.c /tmp/$$.c
	(cd /tmp; $CC $COMPILE_FLAGS -c $$.c; $CC $LDFLAGS $$.o -o $$)
	rm -f /tmp/$$.c /tmp/$$.o /tmp/$$
	echo ""
	echo Check complete
	if agree Any problems
	then
	    continue
	fi
    fi

    break
done
echo LDFLAGS = "$LDFLAGS"
check_ld_flags=no

if test "$HASX" != "False"
then
########## XCFLAGS ###############################################
cat << 'EOF'

Specify the flags to be  passed  to  the  compiler  that  are
required  specifically  for  X Window-based programs. Some systems
require extra compiler flags to increase  the  symbol  table  size,
etc. when compiling for X Windows.  See the installation guide for
help if you are unsure. 
Enter - if no flags are to be specified.

EOF

while true
do
    printf %s "XCFLAGS [$XCFLAGS] : "
    read x
    if test "$x" != ""
    then
	XCFLAGS="$x"
	check_x_c_flags=yes
    fi

    if test "$XCFLAGS" = -
    then
	XCFLAGS=
	check_x_c_flags=no
	break
    fi

    if test "$check_x_c_flags" = yes
    then
	echo ""
	echo "I will check the loader flags now"
	echo "Please watch for error messages"
	rm -f /tmp/$$.c /tmp/$$.o /tmp/$$
	cp utils/main.c /tmp/$$.c
	(cd /tmp; $CC $COMPILE_FLAGS $XCFLAGS -c $$.c; $CC $$.o -o $$)
	rm -f /tmp/$$.c /tmp/$$.o /tmp/$$
	echo ""
	echo Check complete
	if agree Any problems
	then
	    continue
	fi
    fi

    break
done
echo XCFLAGS = "$XCFLAGS"
check_x_c_flags=no

########## XLDFLAGS ###############################################
cat << 'EOF'

Specify the flags to be  passed  to  the  loader  (ld)  that  are
required  specifically  for X Window-based programs. Some systems
require extra loader flags when compiling for X Windows.  
See the installation guide for help if you are unsure. 
Enter - if no flags are to be specified.

EOF

while true
do
    printf %s "XLDFLAGS [$XLDFLAGS] : "
    read x
    if test "$x" != ""
    then
	XLDFLAGS="$x"
	check_x_ld_flags=yes
    fi

    if test "$XLDFLAGS" = -
    then
	XLDFLAGS=
	check_x_ld_flags=no
	break
    fi

    if test "$check_x_ld_flags" = yes
    then
	echo ""
	echo "I will check the loader flags now"
	echo "Please watch for error messages"
	rm -f /tmp/$$.c /tmp/$$.o /tmp/$$
	cp utils/main.c /tmp/$$.c
	(cd /tmp; $CC $COMPILE_FLAGS -c $$.c; $CC $XLDFLAGS $$.o -o $$)
	rm -f /tmp/$$.c /tmp/$$.o /tmp/$$
	echo ""
	echo Check complete
	if agree Any problems
	then
	    continue
	fi
    fi

    break
done
echo XLDFLAGS = "$XLDFLAGS"
check_x_ld_flags=no

########## XEXTRALIBS ###############################################
cat << 'EOF'

Some system require additional libraries when compiling X Window programs.
Specify the additional libraries needed when compiling X Window programs.
Consult your installation guide if you are unsure of the need for these 
libraries. 
Enter - if there are no extra libraries needed.

EOF

while true
do
    printf %s "XEXTRALIBS [$XEXTRALIBS] : "
    read x
    if test "$x" != ""
    then
	XEXTRALIBS="$x"
	check_x_xtra_libs=yes
    fi

    if test "$XEXTRALIBS" = -
    then
        XEXTRALIBS=
        check_x_xtra_flags=no
        break
    fi

    ok=yes
    x=
    for flag in $XEXTRALIBS
    do
        case "$flag" in
            -) ;;
            -*) ;;
             *) flag=-$flag;ok=no;;
        esac
        if test "$x" = ""
        then
            x=$flag
        else
            x="$x $flag"
        fi
	case "$flag" in
	    -) flag=;ok=yes;;
	    -l*) ok=yes;;
	    -*) flag="`echo $flag|sed s/-/-l/`";;
	    *) flag=-l$flag;;
	esac
    done
    XEXTRALIBS="$x"
    if test $ok = no
    then
	echo ""
	echo "Please prefix the libraries with -l"
	echo ""
	check_x_xtra_libs=yes
	continue
    fi
    if test "$XEXTRALIBS" = ""
    then
	break
    fi

    if test "$check_x_xtra_libs" = yes
    then
	echo ""
	echo "Checking $XEXTRALIBS. Please watch for any errors"
	rm -f /tmp/$$ X.o
	$CC utils/X.c -o /tmp/$$ -I$XINCPATH -L$XLIBPATH -lX11 $XEXTRALIBS 
	rm -f /tmp/$$ X.o
	echo ""
	echo "Check complete"
	if agree Any problems
	then
	    continue
	fi
    fi
    break
done
echo XEXTRALIBS = "$XEXTRALIBS"
check_x_xtra_libs=no

fi

########## MATHLIB ###############################################
cat << 'EOF'

Specify the math library.
Enter - if there is no math library, otherwise enter -lm.

EOF

while true
do
    printf %s "MATHLIB [$MATHLIB] : "
    read x
    if test "$x" != ""
    then
	MATHLIB="$x"
	check_mathlib=yes
    fi

    ok=no
    case "$MATHLIB" in
	-) MATHLIB=;ok=yes;;
	-l*) ok=yes;;
	-l)
	    MATHLIB=
	    if findlib m
	    then
		MATHLIB=-lm
	    fi
	    ;;
	-*) MATHLIB="`echo $MATHLIB|sed s/-/-l/`";;
	*) MATHLIB=-l$MATHLIB;;
    esac
    if test $ok = no
    then
	echo ""
	echo "Please prefix the library with -l"
	echo ""
	check_mathlib=yes
	continue
    fi
    if test "$MATHLIB" = ""
    then
	break
    fi

    if test "$check_mathlib" = yes
    then
	echo ""
	echo "Checking $MATHLIB. Please watch for any errors"
	rm -f /tmp/$$ math.o
	$CC utils/math.c $MATHLIB -o /tmp/$$
	rm -f /tmp/$$ math.o
	echo ""
	echo "Check complete"
	if agree Any problems
	then
	    continue
	fi
    fi
    break
done
echo MATHLIB = "$MATHLIB"
check_mathlib=no

########## TERMLIB ###############################################
cat << 'EOF'

Specify the termlib library.
Enter - if there is no termlib library, otherwise enter -ltermlib.

EOF

while true
do
    printf %s "TERMLIB [$TERMLIB] : "
    read x
    if test "$x" != ""
    then
	TERMLIB="$x"
	check_termlib=yes
    fi

    ok=no
    case "$TERMLIB" in
	-) TERMLIB=;ok=yes;;
	-l*) ok=yes;;
	-l)
	    TERMLIB=
	    if findlib termlib
	    then
		TERMLIB=-ltermlib
	    fi
	    ;;
	-*) TERMLIB="`echo $TERMLIB|sed s/-/-l/`";;
	*) TERMLIB=-l$TERMLIB;;
    esac
    if test $ok = no
    then
	echo ""
	echo "Please prefix the library with -l"
	echo ""
	check_termlib=yes
	continue
    fi
    if test "$TERMLIB" = ""
    then
	break
    fi

    if test "$check_termlib" = yes
    then
	echo ""
	echo "Checking $TERMLIB. Please watch for any errors"
	rm -f /tmp/$$ clear.o
	$CC utils/clear.c $TERMLIB -o /tmp/$$
	rm -f /tmp/$$ clear.o
	echo ""
	echo "Check complete"
	if agree Any problems
	then
	    continue
	fi
    fi
    break

done
echo TERMLIB = "$TERMLIB"
check_termlib=no

########## CURSES ##############################################
cat << 'EOF'

Specify the curses library.
EOF
if test "$MASSCOMP" = yes && findlib oldcurses
then
    echo For a masscomp system, you may have to specify -loldcurses.
fi
echo ""

while true
do
    printf %s "CURSES [$CURSES] : "
    read x
    if test "$x" != ""
    then
	CURSES="$x"
	check_curses=yes
    fi

    ok=no
    case "$CURSES" in
	-l*) ok=yes;;
	-l) CURSES=-lcurses;;
	-*) CURSES="`echo $CURSES|sed s/-/-l/`";;
	*) CURSES=-l$CURSES;;
    esac
    if test $ok = no
    then
	echo ""
	echo "Please prefix the library with -l"
	echo ""
	check_curses=yes
	continue
    fi

    if test "$check_curses" = yes
    then
	echo ""
	echo "Checking $CURSES. Please watch for any errors"
	rm -f /tmp/$$ curses.o
	$CC utils/curses.c $CURSES $TERMLIB -o /tmp/$$
	rm -f /tmp/$$ curses.o
	echo ""
	echo "Check complete"
	if agree Any problems
	then
	    continue
	fi
    fi
    break

done
echo CURSES = "$CURSES"
check_curses=no



########## GISBASE ###############################################
if test "$GISBASE" = ""
then
    GISBASE=`(cd ../..;$pwd)`
    if test "$ARCH" != ""
    then
	if test "$GISBASE" = "/"
	then
	    GISBASE=/$ARCH
	else
	    GISBASE=$GISBASE/$ARCH
	fi
    fi
fi

cat << 'EOF'

Select the Unix directory where the GRASS programs, manual and help
files, etc., will reside.
Note: if this is an NFS mount, please enter a directory name which
is common to all machines using this mount point.

EOF

while true
do
    printf %s "GISBASE [$GISBASE] : "
    read x
    if test "$x" != ""
    then
	GISBASE=$x
    fi
    case "$GISBASE" in
	/*) ;;
	 *) echo Must begin with a /; continue ;;
    esac
    if test ! -d $GISBASE
    then
	echo ""
	echo "$GISBASE - directory not found"
	echo ""
	if agree Create $GISBASE
	then
	    makedir $GISBASE
	    if test -d $GISBASE
	    then
		echo $GISBASE created
		echo ""
		break
	    fi
	fi
    elif access w $GISBASE
    then
	break
    else
	echo ""
	echo "$GISBASE - you do not have write permission"
    fi
    echo ""
done
echo GISBASE = $GISBASE

########## SRC ###################################################

cat << 'EOF'

Please enter the Unix directory where the GRASS source code lives.
Note: if this is an NFS mount, please enter a directory name which
is common to all machines using this mount point.

EOF

while true
do
    printf %s "SRC [$SRC] : "
    read x
    if test "$x" != ""
    then
	SRC="$x"
    fi
    case "$SRC" in
	"") continue;;
	/*) ;;
	 *) echo Must begin with a /; continue ;;
    esac
    if test ! -d $SRC
    then
	echo ""
	echo "$SRC - directory not found"
	echo ""
	continue
    fi

    if test -d $SRC/libes
    then
	break
    fi
    echo ""
    echo "$SRC - does not appear to be the GRASS source directory"
    if agree Accept $SRC anyway
    then
	break
    fi
    echo ""
done
echo SRC = $SRC
CMD=$SRC/CMD

########## DEFAULT_DATABASE ######################################
cat << 'EOF'

When GRASS starts, it will ask the user to select a database  and
a  location  within that database. It stores this information and
presents it to the user each time they run GRASS. New users  will
not have a previous database or location.

Please specify the Unix directory which contains GRASS  databases
that can be presented to first time users.

EOF

while true
do

repeat=no

while true
do
    printf %s "DEFAULT_DATABASE [$DEFAULT_DATABASE] : "
    read x
    if test "$x" != ""
    then
	DEFAULT_DATABASE="$x"
    fi
    case "$DEFAULT_DATABASE" in
	"") continue;;
	/*) ;;
	 *) echo Must begin with a /; continue ;;
    esac
    if test ! -d $DEFAULT_DATABASE
    then
	echo ""
	echo "$DEFAULT_DATABASE - directory not found"
	echo ""
	continue
    fi
    break
done
echo DEFAULT_DATABASE = $DEFAULT_DATABASE

########## DEFAULT_LOCATION ######################################
echo ""
echo Now select the default location in $DEFAULT_DATABASE
echo The available locations are:
echo ""
ls -C $DEFAULT_DATABASE
echo ""
echo "Enter - if you want to change the DEFAULT_DATABASE"
echo ""

if test "$DEFAULT_LOCATION" = ""
then
    if test -d $DEFAULT_DATABASE/spearfish
    then
	DEFAULT_LOCATION=spearfish
    else
	DEFAULT_LOCATION="`ls $DEFAULT_DATABASE | awk '{print;exit}'`"
    fi
fi

while true
do
    printf %s "DEFAULT_LOCATION [$DEFAULT_LOCATION] : "
    read x

    if test "$x" != ""
    then
	DEFAULT_LOCATION="$x"
    fi
    if test "$DEFAULT_LOCATION" = -
    then
	DEFAULT_LOCATION=
	repeat=yes
	break
    fi
    case "$DEFAULT_LOCATION" in
	.|..|.*)
	    echo ""
	    echo "$DEFAULT_LOCATION" - must not start with a period
	    echo ""
	    continue
	    ;;
	/|/*|*/)
	    echo ""
	    echo "$DEFAULT_LOCATION" - must not contain a /
	    echo ""
	    continue
	    ;;
    esac
    if test -d $DEFAULT_DATABASE/$DEFAULT_LOCATION
    then
	break
    fi
    echo ""
    echo $DEFAULT_LOCATION - location not found in $DEFAULT_DATABASE
    echo ""
    if agree Accept anyway
    then
	break
    fi
    echo The available locations in $DEFAULT_DATABASE are:
    echo ""
    ls -C $DEFAULT_DATABASE
    echo ""
done
if test $repeat = yes
then
    echo ""
    continue
fi
echo DEFAULT_LOCATION = $DEFAULT_LOCATION
break

done
########## VERIFY ################################################

echo ""
echo "============================================================"
cat << EOF | lineup
CC = $CC
COMPILE_FLAGS = $COMPILE_FLAGS
LDFLAGS = $LDFLAGS
MATHLIB = $MATHLIB
TERMLIB = $TERMLIB
CURSES = $CURSES

XCFLAGS = $XCFLAGS
XLDFLAGS = $XLDFLAGS
XINCPATH = $XINCPATH
XMINCPATH = $XMINCPATH
XLIBPATH = $XLIBPATH
XTLIBPATH = $XTLIBPATH
XMLIBPATH = $XMLIBPATH
XEXTRALIBS = $XEXTRALIBS

UNIX_BIN = $UNIX_BIN
GISBASE = $GISBASE
SRC = $SRC

DEFAULT_DATABASE = $DEFAULT_DATABASE
DEFAULT_LOCATION = $DEFAULT_LOCATION
EOF
echo "============================================================"
echo ""
if agree Look ok
then
    break
fi
done

# if test "$CLEAR" = "" -a "$TERMLIB" != ""
# then
#     rm -f /tmp/$$ clear.o
#     if $CC utils/clear.c $TERMLIB -o /tmp/$$ >/dev/null 2>/dev/null
#     then
# 	printf %s Will try to clear the screen. Hit RETURN :
# 	read x
# 	/tmp/$$
# 	if agree Did the screen clear
# 	then
# 	    CLEAR=yes
# 	fi
# 	echo "Thank you"
#     fi
#     rm -f /tmp/$$ clear.o
# fi

if test "$ARCH" = ""
then
    HEADER=head
else
    HEADER=$ARCH
fi
cat << 'EOF'

Now that you have defined everything, please name the file
to store this information in.

EOF
while true
do
    printf %s "HEADER [$HEADER] : "
    read x
    if test "$x" != ""
    then
	HEADER="$x"
    fi
    case "$HEADER" in
    /|/*|*/|*/*)
	echo ""
	echo "Must not contain a /"
	echo ""
	continue
    esac
    HEAD=$CMD/head/$HEADER
    if test -f $HEAD
    then
	echo ""
	echo "$HEADER - file exists"
	if agree Ok to overwrite
	then
	    break
	fi
    else
	break
    fi
done
echo HEADER = $HEADER

rm -f $HEAD
echo "Creating header file"
(
echo CC = $CC
echo ARCH = $ARCH
echo ""
echo GISBASE = $GISBASE
echo UNIX_BIN = $UNIX_BIN
echo ""
echo DEFAULT_DATABASE = $DEFAULT_DATABASE
echo DEFAULT_LOCATION = $DEFAULT_LOCATION
echo ""
echo COMPILE_FLAGS = $COMPILE_FLAGS
echo LDFLAGS = $LDFLAGS
echo ""
echo XCFLAGS = $XCFLAGS
echo XLDFLAGS = $XLDFLAGS
if test "$XINCPATH" != ""
then
    echo XINCPATH = -I$XINCPATH
else
    echo XINCPATH =
fi
if test "$XMINCPATH" != ""
then
    echo XMINCPATH = -I$XMINCPATH
else
    echo XMINCPATH =
fi
if test "$XLIBPATH" != ""
then
    echo XLIBPATH = -L$XLIBPATH
else
    echo XLIBPATH = 
fi
if test "$XTLIBPATH" != ""
then
    echo XTLIBPATH = -L$XTLIBPATH
else
    echo XTLIBPATH = 
fi
if test "$XMLIBPATH" != ""
then
    echo XMLIBPATH = -L$XMLIBPATH
else
    echo XMLIBPATH = 
fi
echo XEXTRALIBS = $XEXTRALIBS
echo ""
echo TERMLIB = $TERMLIB
echo CURSES = $CURSES '$(TERMLIB)'
echo MATHLIB = $MATHLIB
echo ""
echo '# LIBRULE = ar ruv $@ $?'
echo '# LIBRULE = ar ruv $@ $?; ranlib $@'
echo '# LIBRULE = ar ruv $@ $?; ar ts $@'
echo '# LIBRULE = ar rc $@ `lorder $(OBJ) | tsort`'
echo LIBRULE = "$LIBRULE"
echo ""
if test "$TERMIO" = yes
then
    echo USE_TERMIO = -DUSE_TERMIO
else
    echo USE_TERMIO =
fi
if test "$MTIO" = yes
then
    echo USE_MTIO = -DUSE_MTIO
else
    echo USE_MTIO =
fi
echo DIGITFLAGS = $DIGITFLAGS
echo VECTLIBFLAGS = $VECTLIBFLAGS
# if test "$CLEAR" = yes
# then
#     echo CAN_CLEAR = -DCAN_CLEAR
# else
#     echo CAN_CLEAR =
# fi
if test "$GETHOSTNAME" = gethostname
then
    echo GETHOSTNAME = -DGETHOSTNAME_OK
elif test "$GETHOSTNAME" = uname
then
    echo GETHOSTNAME = -DGETHOSTNAME_UNAME
else
    echo GETHOSTNAME =
fi
) | lineup > $HEAD
echo "Header file $HEAD created"

VERSION=`awk 'NR==1{print $0}' $CMD/VERSION`
gmake=gmake${VERSION}
GMAKE=$UNIX_BIN/$gmake
echo ""
echo "Installing $GMAKE"

rm -f $GMAKE
(
echo ':'
echo SRC=$SRC
echo CMD=$CMD
echo HEADER=$HEADER
if test "$HASX" != "False"
then
    echo HASX=yes
else
    echo HASX=no
fi
echo '. $CMD/generic/gmake.sh'
) > $GMAKE
chmod +x $GMAKE

GISGEN=GISGEN.$HEADER
MAKELINKS=MAKELINKS.$HEADER
echo ""
echo "Creating $GISGEN"
rm -f $CMD/$GISGEN
(
echo ':'
echo "GMAKE=$GMAKE"
echo ". generic/GISGEN.sh"
) > $CMD/$GISGEN
chmod +x $CMD/$GISGEN

echo ""
echo "Creating $MAKELINKS"
rm -f $CMD/$MAKELINKS
(
echo ':'
echo "GMAKE=$GMAKE"
echo ". generic/MAKELINKS.sh"
) > $CMD/$MAKELINKS
chmod +x $CMD/$MAKELINKS

echo ""
echo Setup completed
echo ""


exit 0
