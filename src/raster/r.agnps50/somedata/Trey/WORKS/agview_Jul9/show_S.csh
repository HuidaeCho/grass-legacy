#! /bin/csh
#
#  This C shell calls GRASS function r.reclass
#  to generate maps using the rules generated by 
#  critical area analysis.
#
#  This shell requires 2 input parameters.  The first
#  is the path name for the selected simulation.  The
#  second is the map groups the user wants
#  to display.  $2=2  shows The field averaged
#  contamination severeness and an highlighted cell
#  map overlaying over the field averaged contamination
#  severeness map.     
#
#
#              Zhian Li, 4/15/95
#
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
####################################################



#  Switch to the correct directory 

#   echo "The directory is $1"
   

#   cd $1

#  set the GIS environment 

   setenv GISRC $HOME/.grassrc

# 
# 
#  Generate maps 
#
#   FIRST SET THE RESOLUTION IN THE GRASS MONITOR ITSLEF
#
   g.region res=$2
   d.frame -e
   

   r.reclass input=cell_num.map.$2 output=Sedyield.map <sedyield.rules
   r.reclass input=cell_num.map.$2 output=Cellerosion.map <cellerosion.rules


#  Make the monitor into 2 frames and show three maps for
#  for Sediment.

#  The next statement is no longer needed in the modified version.
#  MIKE FOSTER 2-23-96
#
#  d.mon select=${MONITOR}

   d.frame -e

        d.frame -c frame=f1 at=0,100,0,60 
        d.frame -c frame=f2 at=0,80,60,100
        d.frame -c frame=f3 at=80,100,60,100
     
#  Make a temporary directory if it does not exist 

        if ( ! -d $HOME/tmp ) mkdir $HOME/tmp



     switch ($1)
      case 6:
        d.frame -s frame=f3
        echo "Cell Erosion" |d.text size=20 color=red line=2
        echo "in tons/acre "|d.text size=20 color=green line=3
        cat cellerosion.title |d.text  size=20 color=yellow line=4
        psu_agnps_max_cat.sh Cellerosion.map
        d.frame -s frame=f1
        d.rast map=Cellerosion.map.reclass
        d.frame -s frame=f2
        d.legend map=Cellerosion.map.reclass lines=20
     breaksw
 
     case 7: 
        d.frame -s frame=f3
        echo "Sediment Yield" |d.text size=20 color=red line=2
        echo "in tons"|d.text size=20 color=green line=3
        cat sedyield.title |d.text  size=20 color=yellow line=4
        psu_agnps_max_cat.sh Sedyield.map
        d.frame -s frame=f1
        d.rast map=Sedyield.map.reclass
        d.frame -s frame=f2
        d.legend map=Sedyield.map.reclass lines=20
     breaksw 
     endsw

#        rm $HOME/tmp/tmp.rule
#        rm $HOME/tmp/tmp.rules
#Show the numbers of the cell map.  ADDED BY MIKE FOSTER
     # 3-31-96.
 
     d.frame -s frame=f1  
     d.rast.num map=cell_num.map.$2 grid=black

