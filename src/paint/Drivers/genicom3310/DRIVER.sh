: ${PAINTER?} ${PAINT_DRIVER?}

# for networked machines, set the host which has the printer, and uncomment
# printer_host=. printer_host_alias=.
# case `hostname` in
#     $printer_host|$printer_host_alias);;
#     *) exec rsh $printer_host $GISBASE/etc/paint/driver.rsh $PAINTER n
#        exit 0;;
# esac

MAPLP=${MAPLP-/dev/$PAINTER}

BAUD=9600

HRES=36.0
VRES=36.0
NPIXELS=480
NCHARS=120

TEXTSCALE=0.75
TEXTFUDGE=0
TEXTSPACE=2
BLOCKSIZE=10
BLOCKSPACE=8
NBLOCKS=20

QUALITY=${QUALITY-3}
if test $QUALITY = 3
then
    HRES=72.0
    VRES=72.0
    NPIXELS=960
    TEXTSCALE=1.0
    BLOCKSIZE=20
    BLOCKSPACE=16
fi

export MAPLP BAUD
export HRES VRES NPIXELS NCHARS
export TEXTSCALE TEXTSPACE TEXTFUDGE BLOCKSIZE BLOCKSPACE NBLOCKS
export QUALITY

exec ${PAINT_DRIVER?}
