# some support functions for the 
# GRASS testing framework
# (fetched from some metadata extraction utility from the same author)
# c) Andreas Lange, andreas.lange@rhein-main.de
#


###############################################################################
# check if grass is running
proc grass_running { } {
    global env 
    global mglobal
    

    if { ! [info exists env(GISRC)] } {
	puts stderr "You must be running GRASS to execute $mglobal(progname)"
	return 0
    } else {
	if [catch {exec g.gisenv} result] {
	    return 0
	} else {
	    return 1
	}
    }
}


###############################################################################
# init global array for metadata 
proc meta_init { } {
    global env
    global mglobal


    # set global variables
    set mglobal(out) "index.html"
    set mglobal(css) "grass.css"
    set mglobal(style) "on"
    set mglobal(timefmt) "%d/%b/%Y %H:%M:%S"
    set mglobal(gisdbase) $env(GISDBASE)
    set mglobal(location) $env(LOCATION)
    if { [info exists env(LOCATION_NAME)] } {
	set mglobal(location_name) $env(LOCATION_NAME) 
    } else {
	set nloc [llength [file split $env(LOCATION)]]
	incr nloc -2
	set mglobal(location_name) [lindex [file split $env(LOCATION)] $nloc]
    }
    set mglobal(mapset) $env(MAPSET)
    if { [string match "PERMANENT" "$mglobal(location_name)"] } {
	set mglobal(permanent) $mglobal(location)
    } else {
	set mglobal(permanent) [file join $mglobal(gisdbase) $mglobal(location_name) "PERMANENT"]
    }
    set mglobal(gisbase) $env(GISBASE)
    set mglobal(user) $env(USER)
    set mglobal(gisrc) $env(GISRC)
    set mglobal(fd) "stdout"
    if { [catch {exec g.version | head -1 2>/dev/null} result ] } {
	set mglobal(grassver) "unknown (GRASS 4.x?)"
    } else {
	set mglobal(grassver) [exec g.version | head -1 2>/dev/null ]
    }
    return 1
}


###############################################################################
# checks if the line is empty, i. e. commented or empty
proc empty_line { line } {
    if { [regexp "^\[\ \t;\#\]+" $line] || [regexp {^$} $line] } {
	return 1
    } else {
	return 0
    }
    return
}


###############################################################################
# grass_create_location 
# quick hack to create a new location on the fly,
# directory must exist, use grass_switch_location to switch to it.
# database location proj zone north south east west nwres nsres
proc grass_create_location { db loc proj zone north south east west ewres nsres } {
    set path [file join $db $loc "PERMANENT"]
    if { ! [file isdirectory $path] } {
	exec mkdir -p $path
    } 
    set fn [file join $path "DEFAULT_WIND"]
    # create DEFAULT_WIND file
    if [catch {open $fn w 0600} fd] {
	perror "error opening $path/DEFAULT_WIND"
    }
    puts $fd "proj: $proj"
    puts $fd "zone: $zone"
    puts $fd "north: $north"
    puts $fd "south: $south"
    puts $fd "east: $east"
    puts $fd "west: $west"
    puts $fd "e-w res: $ewres"
    puts $fd "n-s res: $nsres"
    close $fd
    # copy to WIND file
    file copy -force [file join $path "DEFAULT_WIND"] [file join $path "WIND"]
    return
}


###############################################################################
# grass_rm_location
# quick hack to remove a temporary location
# use grass_switch_location to switch to new
# location before removing the old one!
proc grass_rm_location { loc } {
    exec rm -fR $loc
    return
}


###############################################################################
# grass_switch_location
proc grass_switch_location { loc } {
    global env

    # loc: /dir/dir../dir../location/mapset
    #                          -2      -1
    set nloc [llength [file split $loc]]
    setenv MAPSET [lindex [file split $loc] end]
    setenv LOCATION_NAME [lindex [file split $loc] end-1]
    setenv LOCATION [file join [lrange [file split $loc] 0 end-2]]
    # GISDBASE stays same
    # $env(GISDBASE)
    return
}

