:

if [ "$MAPSET" = "" ]
then
echo "You must be in GRASS to run this program."
exit
fi

if [ $# -lt 1 ]
then
    echo " "
    echo "USAGE: $0 Tiger.type1"
    echo "  Where Tiger.type1 is path name to Tiger Type 1 Data File"
    exit 2
fi

input1=$1

if [ -f $input1 ]
then
    continue
else
    echo " "
    echo "Cannot locate input file $input1"
    exit 3
fi

ETC=$GISBASE/etc/tiger_support

if [ -f $ETC/CFCC.Master ]
then
    continue
else
    echo " "
    echo "ERROR: Classification Code support file not found"
    exit 4
fi

TMP1=`g.tempfile pid=$$`
TMP2=`g.tempfile pid=$$`

$GISBASE/etc/tig.linefeed < $input1 > $TMP1

echo "Identified the following tract numbers:"
echo " "

cat $TMP1 | colrm 1 170 | colrm 5 > $TMP2
cat $TMP1 | colrm 1 176 | colrm 5 >> $TMP2
cat $TMP2 | sort -u

echo " "
echo "Found the following Classification Feature Codes:"
echo " "

cat $TMP1 | colrm 1 55 | colrm 4 | sort -u > $TMP2

for i in `cat $TMP2`
do
	fgrep $i $ETC/CFCC.Master
done

rm $TMP1 $TMP2
