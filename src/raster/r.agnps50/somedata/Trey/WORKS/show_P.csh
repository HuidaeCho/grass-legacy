#!/bin/csh
#
#  This C shell calls GRASS function r.reclass
#  to generate maps using the rules generated by 
#  critical area analysis.
#
#  This shell requires 2 input parameters.  The first
#  is the path name for the selected simulation.  The
#  second is the map groups the user wants to display.
#  $2=2  shows the field averaged contamination 
#  severeness and an highlighted cell
#  map overlaying over the field averaged contamination
#  severeness map.     
#
#
#              Zhian Li, 4/15/95
####################################################
#
# MODIFIED BY MIKE FOSTER 2-21-96 to show only
#
# AGNPS output contaminant load, not critical
#
# area
# Parameters: $1 is mapflag (map type)
#             $2 is the cell size desired for
#                purposes of naming the cell_num.map
#                file, e.g. cell_num.map.100
#####################################################

#  Switch to the correct map set
   
#   cd $1

#  Set correct GIS environment 

   setenv GISRC $HOME/.grassrc 


# 
# 
#  Generate maps 
#
#  FIRST, SET THE RESOLUTION IN THE GRASS MONITOR ITSELF
#
   g.region res=$2
   d.frame -e
#
   
   r.reclass input=cell_num.map.$2 output=Sed_P.map <P_sed.rules
   r.reclass input=cell_num.map.$2 output=Sol_P.map <P_sol.rules

# The next statement is no longer needed in the modified version
# MIKE FOSTER 2-23-96
#
#       d.mon select=${MONITOR}
        d.frame -e
        d.frame -c frame=f1 at=15,100,0,100
        d.frame -c frame=f2 at=0,15,50,100
        d.frame -c frame=f3 at=0,15,0,50

#  Make a temporary directory if it does not exist

        if ( ! -d $HOME/tmp ) mkdir $HOME/tmp

   switch ( $1 )

     # Show the map of sediment attatched Phosphorus  
      case 3: 
        d.frame -s frame=f3
        echo "Sediment Phosphorus" |d.text size=20 color=red line=2
        echo "Load" |d.text size=20 color=red line=3
        echo "in lbs/acre " |d.text size=20 color=yellow line=4
        cat P.title |d.text size=20 color=yellow line=5
        psu_agnps_max_cat.sh Sed_P.map
        d.frame -s frame=f1
        d.rast map=Sed_P.map.reclass
        d.frame -s frame=f2
        d.legend map=Sed_P.map.reclass lines=4
       breaksw

     # Show the map of soluble Phosphorus  
      case 4:
        d.frame -s frame=f3
        echo "Soluble Phosphorus" |d.text size=20 color=red line=2
        echo "Load" |d.text size=20 color=red line=3
        echo "lbs/acre " |d.text size=20 color=yellow line=4
        cat P.title |d.text size=20 color=yellow line=5
        psu_agnps_max_cat.sh Sol_P.map
        d.frame -s frame=f1
        d.rast map=Sol_P.map.reclass
        d.frame -s frame=f2
        d.legend map=Sol_P.map.reclass lines=4
      breaksw
      endsw
    
if ( -f $HOME/tmp/tmp.rule ) rm $HOME/tmp/tmp.rule
#Show the numbers of the cell map.  ADDED BY MIKE FOSTER
# 3-31-96.
 
     d.frame -s frame=f1 
     d.rast.num map=cell_num.map.$2 grid=black

