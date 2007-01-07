#############################################################################
#
# $Id$
#
# MODULE:   	Grass Tcl/Tk Initialization
# AUTHOR(S):	Original author unknown - probably CERL
#   	    	Justin Hickey - Thailand - jhickey hpcc.nectec.or.th
#   	    	Markus Neteler - Germany - neteler geog.uni-hannover.de, itc.it
#				Michael Barton - USA - Arizona State University
#               Maris Nartiss - Latvia - maris.gis gmail.com
# PURPOSE:  	The source file for this shell script is in
#   	    	src/tcltkgrass/main/gis_set.tcl. It allows the user to choose
#   	    	the database, location, and mapset to use with grass by
#   	    	presenting a user interface window.
# COPYRIGHT:    (C) 2000,2006 by the GRASS Development Team
#
#               This program is free software under the GNU General Public
#   	    	License (>=v2). Read the file COPYING that comes with GRASS
#   	    	for details.
#
#############################################################################

if {[info exists env(OS)] && $env(OS) == "Windows_NT"} {
	set mingw "1"
} else {
	set mingw "0"
}

source $env(GISBASE)/etc/gtcltk/gmsg.tcl
#############################################################################
#
#   part regarding to the creation of a new location using proj and 
#   the EPSG codes    (routines epsgLocCom and infoEpsg)
#
#############################################################################
source $env(GISBASE)/etc/gtcltk/options.tcl
source $env(GISBASE)/etc/epsg_option.tcl
source $env(GISBASE)/etc/file_option.tcl

#fetch GRASS Version number:
set fp [open $env(GISBASE)/etc/VERSIONNUMBER r]
set GRASSVERSION [read -nonewline $fp]
close $fp


proc searchGISRC { filename } {
 
  global database
  global location
  global mapset
  global oldDb oldLoc oldMap

  global grassrc_list
  set grassrc_list ""

  set flag 0
  if { [file exists $filename] } {
      set ifp [open $filename "r"]
      set thisline [gets $ifp]
      while { [eof $ifp] == 0 } {

            lappend grassrc_list "$thisline"

	    if { [regexp -- {^GISDBASE: *(.*)$} $thisline dummy env_database] } {
                set database $env_database
            }
            if { [scan $thisline "LOCATION_NAME: %s" env_location] } {
                set location $env_location
            }
            if { [scan $thisline "MAPSET: %s" env_mapset] } {
                set mapset $env_mapset
            }
            set thisline [gets $ifp]
      }
      
      set oldDb $database
      set oldLoc $location
      set oldMap $mapset
      
      close $ifp
      if { $database != "" && $location != "" && $mapset != "" } {
         set flag 1
      }
  }
  return $flag
}

proc putGRASSRC { filename } {
 
  global database
  global location
  global mapset

  global grassrc_list

  set ofp [open $filename "w"]

  foreach i $grassrc_list {
      if { [regexp {^GISDBASE:} $i] } \
      {
	  puts $ofp "GISDBASE: $database"
      } \
      elseif { [regexp {^LOCATION_NAME:} $i] } \
      {
 	  puts $ofp "LOCATION_NAME: $location"
      } \
      elseif { [regexp {^MAPSET:} $i] } \
      {
          puts $ofp "MAPSET: $mapset"
      } \
      else \
      {
      	  puts $ofp $i
      }
  }

  close $ofp
}

proc GetDir {entWidget locList mapList} \
{
    global database
    
    toplevel .getDir
    wm title .getDir [G_msg "New GIS data directory"]
    wm resizable .getDir 0 0
    
    frame .getDir.base 
    pack .getDir.base -side top -fill both -expand 1
    
    listbox .getDir.base.list -selectmode single -height 10 -width 40\
    	-yscrollcommand ".getDir.base.vertScroll set" \
    	-xscrollcommand ".getDir.horzScroll set"
    pack .getDir.base.list -side left -fill x -expand 1
    scrollbar .getDir.base.vertScroll -width 12 \
    	-command ".getDir.base.list yview"
    scrollbar .getDir.horzScroll -width 12 -command ".getDir.base.list xview" \
    	-orient horizontal
    pack .getDir.base.vertScroll -side left -fill y
    pack .getDir.horzScroll -side top -fill x

    # Create a frame for the pushbuttons
    frame .getDir.sepFrame -height 4 -bd 2 -relief raised
    pack .getDir.sepFrame -side top -fill x
    frame .getDir.buttonFrame 
    pack .getDir.buttonFrame -side bottom -fill x
    
    # Create the pushbuttons
    button .getDir.buttonFrame.add -text [G_msg "OK"] -bd 1  -width 5 \
    	-command "SetDatabase .getDir.base.list .getDir $entWidget $locList $mapList"
    button .getDir.buttonFrame.quit -bd 1 -width 5 -text [G_msg "Cancel"] \
    	-command {destroy .getDir}
    pack .getDir.buttonFrame.add .getDir.buttonFrame.quit -side left \
    	-expand 1 -padx 10 -pady 5

    # Bind the double click to change directories
    bind .getDir.base.list <Double-Button-1> {ChangeDir %W %y}
    GetListItems .getDir.base.list [file dirname $database] \
    	[file tail $database]
}

proc GetListItems {widget dir default} \
{
    set currDir [pwd]
    if { [cdir $dir] == 0} {
    
    # Insert the parent directory in the list
    $widget delete 0 end
    set parent $dir
    append parent "/.."
    $widget insert end $parent
    
    # Set counter to 1 since parent is already in the list
    set i 1
    set index 0
    
    foreach filename [lsort [glob -nocomplain *]] {
		if {[file isdirectory $filename]} {
			if {[string compare $default $filename] == 0} {
				set index $i
			}
		
			set path $dir
			append path "/" $filename
			set filename $path
		
			$widget insert end $filename
	
			incr i
		}
    }
    
    $widget see $index
    $widget selection set $index
    $widget xview moveto 1
    
    cdir $currDir
    }
}

proc SetDatabase {widget top entryWidget locList mapList} \
{
    global database location
    
    set tmpdatabase [$widget get [$widget curselection]]
    
    if {[string compare [file tail $tmpdatabase] ".."] == 0} \
    {
    	set tmpdatabase [file dirname [file dirname $tmpdatabase]]
    }
    
    $entryWidget xview moveto 1
    
    if { [cdir $tmpdatabase] == 0} {
        set database $tmpdatabase
        set location ""
        refresh_loc
    
    .frame0.frameBUTTONS.ok configure -state disabled
    destroy $top
    }
}

proc ChangeDir {widget y} \
{
    set dir [$widget get [$widget nearest $y]]
    
    if {[string compare [file tail $dir] ".."] == 0} \
    {
    	set dir [file dirname [file dirname $dir]]
    }
    
    GetListItems $widget $dir ""
}

# Returns 0, if location is invalid, 1 othervise.
# Are hardcoded / in path's OK? They where here before me :) Maris.
proc CheckLocation {} {
    global database location
    
    set found 0
    set dir $database
    append dir "/$location"
    set currDir [pwd]

    # Special case - wrong GISDBASE
    if {[file isdirectory $dir] == 0} {
        DialogGen .wrnDlg [G_msg "WARNING: invalid location"] warning \
	[format [G_msg "Warning: location <%s> at GISDBASE <%s> is not a directory or does not exist."] \
	$location $database] \
        0 OK;
    } else {
        cdir $dir
        
        foreach filename [lsort [glob -nocomplain *]] \
        {
            if {[string compare $filename "PERMANENT"] == 0} \
            {
                # All good locations have valid PERMANENT mapset.
                if {[file exists "$dir/PERMANENT/WIND"] != 0} {
                    set found 1
                }
            }
        }
    }
    
    cdir $currDir
    return $found
}


#############################################################################
proc gisSetWindow {} {
# create main GRASS startup panel
    global GRASSVERSION
    global database
    global location
    global mymapset
    global mapset
    global oldDb oldLoc oldMap
    global env
    global grassrc_list
    global gisrc_name
    global refresh
    
    set refresh 0

    global mingw

    # Window manager configurations

    wm title . [format [G_msg "GRASS %s Startup"] $GRASSVERSION]

    # ---------------------------
    # build .frame0
    # ---------------------------
    set mainfr [frame .frame0 \
    	-borderwidth {2} \
    	-relief {raised}]

    set titlefrm [frame .frame0.intro -borderwidth 2 ]
    set introimg  [label $titlefrm.img -image [image create photo -file \
    	"$env(GISBASE)/etc/gintro.gif"]]
    set introtitle [text $titlefrm.msg -height 5 \
    	-relief flat -fg darkgreen \
    	-bg #dddddd \
    	-font introfont \
    	-width 50 ]

    pack $titlefrm -side top
	pack $introimg -side top
    pack $introtitle -side top

    .frame0.intro.msg tag configure all -justify center
    .frame0.intro.msg insert end [G_msg "Welcome to GRASS GIS Version $GRASSVERSION\n"]
    .frame0.intro.msg insert end [G_msg "The world's leading open source GIS\n\n"]
    .frame0.intro.msg insert end [G_msg "Select an existing project location and mapset\n"]
    .frame0.intro.msg insert end [G_msg "or define a new location\n"]
    .frame0.intro.msg tag add all 1.0 end
    .frame0.intro.msg configure -state disabled

    # -----------------------------------
    # build .frame0.frameDB
    # -----------------------------------

    frame .frame0.frameDB \
    	-borderwidth {2}

    frame .frame0.frameDB.left \
    	-borderwidth {2}

    frame .frame0.frameDB.mid \
    	-borderwidth {2}

    frame .frame0.frameDB.right \
    	-borderwidth {2}

    label .frame0.frameDB.left.label \
    	-anchor {n} \
    	-justify right \
    	-text [G_msg "GIS Data Directory: "]

    entry .frame0.frameDB.mid.entry \
    	-relief {sunken} \
    	-textvariable database \
		-width 40 \
    	-xscrollcommand { .frame0.frameDB.mid.hscrollbar set}
    
    scrollbar .frame0.frameDB.mid.hscrollbar \
    	-command { .frame0.frameDB.mid.entry xview} \
    	-relief {sunken} \
    	-width 12 \
    	-orient {horizontal}
 
    if { $mingw == "1" } {
       # We cannot use Double-Button-1 (change dir) and Button-1 (select dir)
       # events at the same time because of MS-Windows TclTk's event bug.
       button .frame0.frameDB.right.button \
    	   -text [G_msg "Browse..."] -bd 1 \
    	   -command {set database [tk_chooseDirectory -initialdir $database \
	   	-parent .frame0 -title [G_msg "New GIS data directory"] -mustexist true]
		refresh_loc
		.frame0.frameBUTTONS.ok configure -state disabled}
    } else {
       button .frame0.frameDB.right.button \
    	   -text [G_msg "Browse..."] -bd 1 \
    	   -command {GetDir .frame0.frameDB.mid.entry .frame0.frameLOC.listbox \
    	       .frame0.frameMS.listbox}
    }


    pack .frame0.frameDB.left.label -side top
    pack .frame0.frameDB.mid.entry -side top -fill x
    pack .frame0.frameDB.mid.hscrollbar -side bottom -fill x
    pack .frame0.frameDB.right.button -side left -fill x
    pack .frame0.frameDB.left -side left  -anchor n -fill x
    pack .frame0.frameDB.mid -side left -fill x
    pack .frame0.frameDB.right -side left -anchor n -fill x

    # -----------------------------------
    # build .frame0.frameLOC
    # -----------------------------------
    frame .frame0.frameLOC \
    	-borderwidth {2}

    label .frame0.frameLOC.label \
    	-anchor {w} \
    	-text [G_msg "Project Location\n(projection/coordinate system)"] 

    listbox .frame0.frameLOC.listbox \
    	-relief {sunken} \
    	-exportselection false \
    	-yscrollcommand {.frame0.frameLOC.vscrollbar set} \
    	-xscrollcommand {.frame0.frameLOC.hscrollbar set}

    scrollbar .frame0.frameLOC.vscrollbar -width 12 \
    	-command {.frame0.frameLOC.listbox yview} \
    	-relief {sunken}

    scrollbar .frame0.frameLOC.hscrollbar -width 12 \
    	-command {.frame0.frameLOC.listbox xview} \
    	-orient {horizontal} \
    	-relief {sunken}

    pack append .frame0.frameLOC \
    	.frame0.frameLOC.label { top fill } \
    	.frame0.frameLOC.vscrollbar { right filly } \
    	.frame0.frameLOC.hscrollbar { bottom fillx } \
    	.frame0.frameLOC.listbox { left expand fill }


    # -----------------------------------
    # build .frame0.frameMS
    # -----------------------------------
    frame .frame0.frameMS \
    	-borderwidth {2}

    label .frame0.frameMS.label \
    	-anchor {w} \
    	-text [G_msg "Accessible Mapsets\n(directories of GIS files)"] 

    listbox .frame0.frameMS.listbox \
    	-relief {sunken} \
    	-yscrollcommand {.frame0.frameMS.vscrollbar set} \
    	-xscrollcommand {.frame0.frameMS.hscrollbar set}

    scrollbar .frame0.frameMS.vscrollbar -width 12 \
    	-command {.frame0.frameMS.listbox yview} \
    	-relief {sunken}

    scrollbar .frame0.frameMS.hscrollbar -width 12 \
    	-command {.frame0.frameMS.listbox xview} \
    	-orient {horizontal} \
    	-relief {sunken}

    pack append .frame0.frameMS \
    	.frame0.frameMS.label { top fill } \
    	.frame0.frameMS.vscrollbar { right filly } \
    	.frame0.frameMS.hscrollbar { bottom fillx } \
    	.frame0.frameMS.listbox { left expand fill }

    # -----------------------------------
    # build .frame0.frameNMS
    # -----------------------------------
    frame .frame0.frameNMS \
    	-borderwidth {2}

    frame .frame0.frameNMS.first \
    	-borderwidth {2}

    frame .frame0.frameNMS.second \
    	-borderwidth {2}

    frame .frame0.frameNMS.third \
    	-borderwidth {2}

    frame .frame0.frameNMS.fourth \
    	-borderwidth {2}

    frame .frame0.frameNMS.fifth \
    	-borderwidth {2}

    frame .frame0.frameNMS.sixth \
    	-borderwidth {2}

    frame .frame0.frameNMS.seventh \
    	-borderwidth {2}

    label .frame0.frameNMS.first.label \
    	-anchor {n} \
    	-text [G_msg "Create new mapset\nin selected location"]

    entry .frame0.frameNMS.second.entry \
    	-relief {sunken} \
    	-textvariable mymapset \
    	-width 22 
	
    button .frame0.frameNMS.third.button \
    	-text [G_msg "Create new mapset"] \
    	-width 20 -bd 1 \
     	-command { 
            .frame0.frameNMS.third.button configure -state disabled
	    if { $mymapset != "" } {
            	if {[CheckLocation] == 0} {
            	    DialogGen .wrnDlg [G_msg "WARNING: invalid location"] warning \
		    [format [G_msg "Warning: selected location <%s> is not valid. \n New mapset is NOT created. \n Select valid location and try again."] $location] \
                    0 OK;
            	} else {
                    cdir $database
                    cdir $location
                    file mkdir $mymapset
                    #generate default DB definition, create dbf subdirectory:
                    set varfp [open $mymapset/VAR "w"]
                    puts $varfp "DB_DRIVER: dbf"
                    puts $varfp "DB_DATABASE: \$GISDBASE/\$LOCATION_NAME/\$MAPSET/dbf/"
                    close $varfp
                    catch {file attributes $mymapset/VAR -permissions u+rw,go+r}
                    file mkdir $mymapset/dbf
                    #copy over the WIND definition:
                                    catch {file copy $mymapset/../PERMANENT/WIND $mymapset}
                    catch {file attributes $mymapset/WIND -permissions u+rw,go+r}
                    .frame0.frameMS.listbox insert end $mymapset
                    #TODO: select new MAPSET
                }
            }
	}

    label .frame0.frameNMS.fourth.label \
    	-anchor {n} \
    	-text [G_msg "Define new location with..."]


    button .frame0.frameNMS.fifth.button \
    	-text [G_msg "Georeferenced file"] \
    	-width 20 -bd 1\
    	-relief raised \
    	-command "fileOpt::fileLocCom
    		tkwait window .fileloc
    		refresh_loc"

    button .frame0.frameNMS.sixth.button \
    	-text [G_msg "EPSG codes"] \
    	-width 20 -bd 1\
    	-relief raised \
    	-command "epsgOpt::epsgLocCom
    		tkwait window .optPopup
    		refresh_loc"
    	    			
    button .frame0.frameNMS.seventh.button \
    	-text [G_msg "Projection values"] \
    	-width 20 -bd 1\
    	-relief raised \
    	-command {
           if { $mingw == "1" } {
               exec -- cmd.exe /c start $env(GISBASE)/etc/set_data
           } else {
	       exec -- $env(GISBASE)/etc/grass-xterm-wrapper -name xterm-grass -e $env(GISBASE)/etc/grass-run.sh $env(GISBASE)/etc/set_data
           }
           # Now we should refresh the list of locations!
           refresh_loc ;# Could it look like this? Maris.
        }

    pack append .frame0.frameNMS
    pack .frame0.frameNMS.first.label -side top
    pack .frame0.frameNMS.second.entry -side top -fill x
    pack .frame0.frameNMS.third.button -side top -fill x
    pack .frame0.frameNMS.fifth.button -side bottom -fill x
    pack .frame0.frameNMS.sixth.button -side bottom -fill x
    pack .frame0.frameNMS.seventh.button -side bottom -fill x
    pack .frame0.frameNMS.fourth.label -side bottom
    pack .frame0.frameNMS.first -side top  -anchor n
    pack .frame0.frameNMS.second -side top -expand yes
    pack .frame0.frameNMS.third -side top -anchor n -expand yes
    pack .frame0.frameNMS.seventh -side bottom -anchor s -expand yes
    pack .frame0.frameNMS.sixth -side bottom -anchor s -expand yes
    pack .frame0.frameNMS.fifth -side bottom -anchor s -expand yes
    pack .frame0.frameNMS.fourth -side bottom -anchor s -expand yes

    # ----------------------------------
    # build .frame0.frameBUTTONS
    # ----------------------------------
    frame .frame0.frameBUTTONS \
    	-borderwidth {2}
    
    
    button .frame0.frameBUTTONS.ok \
     	-text [G_msg "Enter GRASS"] \
    	-width 10 -bd 1 -fg green4 -default active \
     	-command {
            if {[CheckLocation] == 0} {
            	    DialogGen .wrnDlg [G_msg "WARNING: invalid location"] warning \
		    [format [G_msg "Warning: selected location <%s> is not valid. \n Select valid location and try again."] $location] \
                    0 OK;
            } else {
                if {[file exists "$database/$location/$mapset/WIND"] == 0} {
                    DialogGen .wrnDlg [G_msg "WARNING: invalid mapset"] warning \
                    [format [G_msg "Warning: <%s> is not a valid mapset"] $mapset] \
                    0 OK;
                }
                if { $mapset != "" && [file exists "$database/$location/$mapset/WIND"] != 0} {
                    puts stdout "GISDBASE='$database';"
                    puts stdout "LOCATION_NAME='$location';"
                    puts stdout "MAPSET='$mapset';"
                    putGRASSRC $gisrc_name
                    destroy .
                }
            } 
        }
        
    bind . <Return> {.frame0.frameBUTTONS.ok invoke}

    button .frame0.frameBUTTONS.help \
    	-text [G_msg "Help"] \
    	-width 10 -bd 1\
    	-bg honeydew2 \
		-command {
			if { [winfo exists .help] } {
				 puts [G_msg "Help already opened"]
				 wm deiconify .help
				 raise .help
				 return
			}
			if { $mingw == "1" } {
				exec -- $env(GRASS_HTML_BROWSER) file://$env(GISBASE)/docs/html/helptext.html &;
			} else {
				exec -- $env(GRASS_HTML_BROWSER) file://$env(GISBASE)/docs/html/helptext.html >@stdout 2>@stderr &;
			}
        }

    button .frame0.frameBUTTONS.cancel \
    	-text [G_msg "Exit"] \
    	-width 10 -bd 1 \
    	-command { 
            puts stdout "exit" 
            destroy . 
        }

    #############################################################################

    pack append .frame0.frameBUTTONS \
    	.frame0.frameBUTTONS.ok { left  } \
    	.frame0.frameBUTTONS.cancel { left  } \
    	.frame0.frameBUTTONS.help { right  } 



    # ----------------------------------
    # packed it all
    # ----------------------------------

    # pack widget .frame0
    pack append .frame0 \
    	.frame0.frameDB { top expand fill } \
    	.frame0.frameBUTTONS { bottom expand fill } \
    	.frame0.frameLOC { left expand  } \
    	.frame0.frameMS { left expand  } \
     	.frame0.frameNMS { left expand fill }

    .frame0.frameNMS.third.button configure -state disabled

    pack append . \
    	.frame0 { top frame center expand fill }

    .frame0.frameDB.mid.entry xview moveto 1
    
    if { ! [file exists $database] } \
    {
      	DialogGen .wrnDlg [G_msg "WARNING: Invalid Database"] warning \
	    [G_msg "WARNING: Invalid database. Finding first valid directory in parent tree"] \
	    0 OK
      
      	while { ! [file exists $database] } \
      	{
	    set database [file dirname $database]
      	}
    }
    
    # setting list of locations
    refresh_loc
        
    set i 0
    set curSelected 0
    set length [.frame0.frameLOC.listbox size]
    while { $i <  $length } {
    	if { $location == [.frame0.frameLOC.listbox get $i] } {
            set curSelected $i
            break
      	}
        
	incr i 1
    }
    
    .frame0.frameLOC.listbox select set $curSelected

	# setting list of owned mapsets
    cdir $database
    if { [file exists $location] } \
    {
	cdir $location
	foreach i [lsort [glob -directory [pwd] *]] {
     	    if {[file isdirectory $i] && [file owned $i] } {
        	.frame0.frameMS.listbox insert end [file tail $i]
      	    }
	}

	set i 0
	set curSelected 0
	set length [.frame0.frameMS.listbox size]
	while { $i <  $length } {
    	    if { $mapset == [.frame0.frameMS.listbox get $i] } {
        	set curSelected $i
        	break
            }

	    incr i 1
	}

	.frame0.frameMS.listbox yview $curSelected
	.frame0.frameMS.listbox select set $curSelected
    }

  bind .frame0.frameDB.mid.entry <Return> {
        set new_path [%W get]
        if { "$new_path" != "" \
             && [file exists $new_path] && [file isdirectory $new_path] } {
           %W delete 0 end
           %W insert 0 $new_path
           cdir $new_path
           set location ""
           refresh_loc
           set database [pwd]
        }
	.frame0.frameBUTTONS.ok configure -state disabled
  }

  bind .frame0.frameLOC.listbox <Double-ButtonPress-1> {
        # Do something only if there IS atleast one location
        if {[%W size] > 0} {
            %W select set [%W nearest %y]
            cdir $database
            set location [%W get [%W nearest %y]]
            if {[CheckLocation] == 0} {
                    # Notice - %%s prevents %s capturing by bind
            	    DialogGen .wrnDlg [G_msg "WARNING: invalid location"] warning \
		    [format [G_msg "Warning: selected location <%%s> is not valid. \n Select valid location and try again."] $location] \
                    0 OK;
            } else {
                    cdir $location
            .frame0.frameMS.listbox delete 0 end
            foreach i [lsort [glob -directory [pwd] *]] {
              if { [file isdirectory $i] && [file owned $i] } { 
                    .frame0.frameMS.listbox insert end [file tail $i]
              }
            }
            set mapset ""
            .frame0.frameBUTTONS.ok configure -state disabled
	}
  }
  }

  bind .frame0.frameLOC.listbox <ButtonPress-1> {
        # Do something only if there IS atleast one location
        if {[%W size] > 0} {
            %W select set [%W nearest %y]
            cdir $database
            set location [%W get [%W nearest %y]]
            if {[CheckLocation] == 0} {
                    # Notice - %%s prevents %s capturing by bind
            	    DialogGen .wrnDlg [G_msg "WARNING: invalid location"] warning \
		    [format [G_msg "Warning: selected location <%%s> is not valid. \n Select valid location and try again."] $location] \
                    0 OK;
            } else {
                    cdir $location
            .frame0.frameMS.listbox delete 0 end
            foreach i [lsort [glob -directory [pwd] *]] {
              if { [file isdirectory $i] && [file owned $i] } {
                    .frame0.frameMS.listbox insert end [file tail $i]
              }
            }
            set mapset ""
            .frame0.frameBUTTONS.ok configure -state disabled
        }
  }
  }

  bind .frame0.frameMS.listbox <Double-ButtonPress-1> {
        # Do something only if there IS atleast one mapset
        if {[%W size] > 0} {
            %W select set [%W nearest %y]
            set mapset [%W get [%W nearest %y]]
            .frame0.frameBUTTONS.ok configure -state normal
            if {[CheckLocation] == 0} {
                    # Notice - %%s prevents %s capturing by bind
            	    DialogGen .wrnDlg [G_msg "WARNING: invalid location"] warning \
		    [format [G_msg "Warning: selected location <%%s> is not valid. \n Select valid location and try again."] $location] \
                    0 OK;
            } else {
                if {[file exists "$database/$location/$mapset/WIND"] == 0} {
                    DialogGen .wrnDlg [G_msg "WARNING: invalid mapset"] warning \
                    [format [G_msg "Warning: <%%s> is not a valid mapset"] $mapset] \
                    0 OK;
                }
                if { $mapset != "" && [file exists "$database/$location/$mapset/WIND"] != 0} {
                    puts stdout "GISDBASE='$database';"
                    puts stdout "LOCATION_NAME='$location';"
                    puts stdout "MAPSET='$mapset';"
                    putGRASSRC $gisrc_name
                    destroy .
                }
            }
        }
  }

  bind .frame0.frameMS.listbox <ButtonPress-1> {
        # Do something only if there IS atleast one mapset
        if {[%W size] > 0} {
            %W select set [%W nearest %y]
            set mapset [%W get [%W nearest %y]]
            .frame0.frameBUTTONS.ok configure -state normal
            if {[CheckLocation] == 0} {
            	    DialogGen .wrnDlg [G_msg "WARNING: invalid location"] warning \
		    [format [G_msg "Warning: selected location <%%s> is not valid. \n Select valid location and try again."] $location] \
                    0 OK;
            }
        }
  }

  bind .frame0.frameNMS.second.entry <KeyRelease> {
	.frame0.frameNMS.third.button configure -state active
  }
  
  grab .
  tkwait window . 

}

#############################################################################

proc refresh_loc {} {
# refresh location listbox entries 
	global database

	set locList .frame0.frameLOC.listbox
	set mapList .frame0.frameMS.listbox
 

	if { "$database" != "" \
		 && [file exists $database] && [file isdirectory $database] } {
	   cdir $database
	   $locList delete 0 end
	   foreach i [lsort [glob -nocomplain -directory [pwd] *]] {
		   if { [file isdirectory $i] } {
			   $locList insert end [file tail $i]
		   }
	   }
	   $mapList delete 0 end
	}
	.frame0.frameBUTTONS.ok configure -state disabled
	update idletasks
}

# cd wrapper
proc cdir { dir } {
    if { [catch { cd $dir }] } {
        DialogGen .wrnDlg [G_msg "WARNING: change directory failed"] warning \
          [format [G_msg "Warning: could not change directory to <%s>.\nCheck directory permissions."] $dir ]\
          0 OK;
          return 1
    } else {
        return 0
    }
}

#############################################################################
#
# proc DialogGen {widget title bitmap text default buttons}
#
# PURPOSE:  	This function simply pops up a dialog box with a given message.
#   	    	Note that it is similar to tk_dialog but has a slightly
#   	    	different look to the dialog.
#   	    	Example call:
#   	    	    set val [DialogGen .warnDlg "WARNING: List Changed" \
#   	    	    	warning "WARNING: You have changed the current list.\
#   	    	    	Do you want to discard the changes and open a new \
#   	    	    	file?" 0 OK Cancel]
#    	    	    if { $val == 0 } { puts stderr "OK button pressed" }
#   	    	    if { $val == 1 } { puts stderr "Cancel button pressed" }
# INPUT VARS:	widget	=>  name of the dialog box starting with . eg .errDlg
#   	    	title	=>  title to display in window border
#   	    	bitmap	=>  bitmap icon to display - must be one of
#   	    	    	    	error	    	gray12
#   	    	    	    	gray50 	    	hourglass
#   	    	    	    	info   	    	questhead
#   	    	    	    	question    	warning
#   	    	text	=>  text of the message to be displayed
#   	    	default =>  index of default button (0, 1, 2...) must be less
#   	    	    	    than number of buttons
#   	    	buttons =>  text to be used for each button eg OK Cancel
# RETURN VAL:	index of button that was clicked - can be ignored if only one
#   	    	button is defined
#
#############################################################################

# Procedure to generate the dialog box
proc DialogGen {widget title bitmap text default buttons} \
{
    global buttonNum
    
    # Create a popup window to warn the user
    toplevel $widget
    wm title $widget $title
    wm resizable $widget 0 0
    wm protocol $widget WM_DELETE_WINDOW "CancelDialog $widget"

    # Create a label for the bitmap and a message for the text
    frame $widget.dlgFrame
    pack $widget.dlgFrame -side top -fill both
    label $widget.dlgFrame.icon -bitmap $bitmap
    message $widget.dlgFrame.text -text $text -width 10c
    pack $widget.dlgFrame.icon $widget.dlgFrame.text -side left -fill x \
	-padx 10
    
    # Create a frame for the pushbuttons
    frame $widget.sepFrame -height 4 -bd 2 -relief raised
    frame $widget.buttonFrame 
    pack $widget.buttonFrame $widget.sepFrame -side bottom -fill x

    # Create the pushbuttons
    set i 0
    foreach buttonLabel $buttons \
    {
	button $widget.buttonFrame.$i -text $buttonLabel \
	    -command "set buttonNum $i"
	pack $widget.buttonFrame.$i -side left -expand 1 -padx 10 -pady 5
	incr i
    }
    
    # Position the top left corner of the window over the root window
    wm withdraw $widget
    update idletasks
    wm geometry $widget +[expr [winfo rootx .] + ([winfo width .] \
	-[winfo width $widget]) / 2]+[expr [winfo rooty .] + ([winfo \
	height .] - [winfo height $widget]) / 2]
    wm deiconify $widget

    # Grab the pointer to make sure this window is closed before continuing
    grab set $widget

    if {$default >= 0} \
    {
	focus $widget.buttonFrame.$default
    }
    
    tkwait variable buttonNum
    
    # Destroy the popup window
    destroy $widget
    
    # Return the number of the button that was pushed
    return "$buttonNum"
}

# Procedure to cancel the dialog
proc CancelDialog {widget} \
{
    global buttonNum

    # Set the wait variable so that the dialog box can cancel properly
    set buttonNum 999
}

global database
global location
global mapset

global grassrc_list
global gisrc_name

set ver [info tclversion]

if { [string compare $ver "8.0"] < 0} \
{
    puts stderr "Sorry your version of the Tcl/Tk libraries is $ver and is too"
    puts stderr "old for GRASS which requires a Tcl/Tk library version of 8.0 or later."
    puts stderr "Reverting default settings back to GRASS text mode interface."
    exit 1
}

set database ""
set location ""
set mapset ""

set gisrc_name ""
if { [info exists env(GISRC)] } {
   set gisrc_name $env(GISRC)
}

if { [searchGISRC $gisrc_name] } {
   gisSetWindow
}







