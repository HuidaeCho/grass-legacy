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

HRES=101.6
VRES=100.0
NCHARS=132

TEXTSCALE=1.0
TEXTFUDGE=0
TEXTSPACE=3
BLOCKSIZE=25
BLOCKSPACE=23
NBLOCKS=20

export MAPLP BAUD
export HRES VRES NCHARS
export TEXTSCALE TEXTSPACE TEXTFUDGE BLOCKSIZE BLOCKSPACE NBLOCKS

exec ${PAINT_DRIVER?}
