# Nviz 1.1
# USACERL 3/11/96
# This file contains a palette of commonly used widgets.  It is
# expected that Nviz panels will be constructed using these widgets
# plus basic Tk functionality.

##########################################################################
# procedure to drag canvas item
##########################################################################
proc Nv_itemDrag {c info x y} {
    set lastx [St_get $info lastx]
    set lasty [St_get $info lasty]
    set item  [St_get $info item]
    set x [$c canvasx $x]
    set y [$c canvasy $y]
    $c move $item [expr $x-$lastx] [expr $y-$lasty]
    St_set $info lastx $x
    St_set $info lasty $y
}

##########################################################################
# procedure to get current x-y position of widget
# Returns position as [list x y] ratio
##########################################################################
proc Nv_getXYPos { iname } {
    global Nv_
    
    set x [St_get $Nv_($iname) lastx]
    set y [St_get $Nv_($iname) lasty]
    set w [St_get $Nv_($iname) width]
    set h [St_get $Nv_($iname) height]
    set x [expr $x/($w + 0.0) ]
    set y [expr $y/($h + 0.0) ]
    
    return [list $x $y]
}

##########################################################################
# procedure to make x-y position "widget" as seen in Nviz
##########################################################################
proc Nv_mkXYScale {C {type puck} {name null} {height 100} {width 100} {x 50} {y 50} {cmd null} {upcmd null}} {
    global Nv_

    canvas $C  -relief sunken -borderwidth 3 -height $height -width $width 
    set x1 [expr $x - 5]
    set x2 [expr $x + 5]
    set y1 [expr $y - 5]
    set y2 [expr $y + 5]
    if {[string compare $type puck] == 0} {
	$C create arc $x1 $y1 $x2 $y2 -style arc -width 1 -outline gray60 \
	    -start 210 -extent 210 -tags puck
	$C create arc $x1 $y1 $x2 $y2 -style arc -width 1 -outline white \
	    -start 60 -extent 150 -tags puck
    } else {
	$C create line [expr 0 -$width]  $x  [expr 2 * $width] $x \
	    -width 1 -tags cross
	$C create line $y [expr 0 - $height]  $y [expr 2*$height] \
	    -width 1 -tags cross
    }
    if {[string compare $name null] == 0} {set name $C.item}
    set Nv_($name) [St_create {item lastx lasty width height} $type $x $y $width $height]
    bind $C <1> "Nv_itemDrag $C $Nv_($name) %x %y; Nv_xyCallback $cmd $width $height %x %y "
    bind $C <B1-Motion> "Nv_itemDrag $C $Nv_($name) %x %y; Nv_xyCallback $cmd $width $height %x %y "
    
    bind $C <B1-ButtonRelease> "Nv_itemDrag $C $Nv_($name) %x %y; Nv_xyCallback $upcmd $width $height %x %y "
    return $C
}

proc Nv_xyCallback { cmd w h x y } {
    if [string compare $cmd null] {
	set x [expr (1.0*$x)/$w]
	set y [expr (1.0*$y)/$h]
	$cmd $x $y
    }
}

##########################################################################
# procedure to change scale setting
##########################################################################
proc Nv_changeScale {S {v 0}} {
    global Nv_
    $S set $v
}

proc Nv_setEntry {E V} {
    $E delete 0 end; $E insert 0 $V 
}
    
proc Nv_scaleCallback { S {who s} {decimal 0} {cmd null} {val 0} } {
    if {$who == "s"} {
	set val [expr $val/pow(10,$decimal)]
	Nv_setEntry $S.f.entry $val
    } elseif {$who == "e"} {
	set min [expr int([lindex [$S.scale configure -to] 4] / pow(10,$decimal))]
	set max [expr int([lindex [$S.scale configure -from] 4] / pow(10,$decimal))]
	set val [$S.f.entry get]
	if {$val < $min} then {
	    $S.scale configure -to [expr int($val*pow(10,$decimal))]
	}
	if {$val > $max} then {
	    $S.scale configure -from [expr int($val*pow(10,$decimal))]
	}
	Nv_changeScale  $S.scale [expr int($val*pow(10,$decimal))]
    } elseif {$who == "b"} {
	Nv_changeScale  $S.scale $val
	set tmpval [expr $val/pow(10,$decimal)]
	Nv_setEntry $S.f.entry $tmpval
    }
    
    $cmd $val
    
}

proc Nv_floatscaleCallback { S {who s} {decimal 0} {cmd null} {val 0} } {
    if {$who == "s"} {
	Nv_setEntry $S.f.entry $val
    } elseif {$who == "e"} {
	set min [lindex [$S.scale configure -to] 4]
	set max [lindex [$S.scale configure -from] 4]
	set val [$S.f.entry get]
	if {[expr $val < $min]} then {
	    $S.scale configure -to $val
	}
	if {[expr $val > $max]} then {
	    $S.scale configure -from $val
	}
	
	Nv_changeScale  $S.scale $val
    } elseif {$who == "b"} {
	set min [lindex [$S.scale configure -to] 4]
	set max [lindex [$S.scale configure -from] 4]
	if {[expr $val < $min]} then {
	    $S.scale configure -to $val
	}
	if {[expr $val > $max]} then {
	    $S.scale configure -from $val
	}

	Nv_changeScale  $S.scale $val
	Nv_setEntry $S.f.entry $val
    }
    
    $cmd $val
    
}

##########################################################################
# procedures to make sliders  
##########################################################################
proc Nv_mkScale { S {orient v} {name ---} {from 10000} {to 0} {curr 500} {cmd null} {decimal 0}} {
    frame $S 
    frame $S.f

    if { $orient == "v" } {
	set side left
	set text_side top
	set orient v
    } else {
	set side top
	set text_side left
	set orient h
    }

    scale $S.scale -from $from -length 140 -showvalue 0 -orient $orient \
		-tickinterval 0 -to $to -width 13 \
		-command "Nv_scaleCallback $S s $decimal $cmd " \
		-activebackground gray80 -background gray90

    label $S.f.label -text $name
    $S.scale set $curr
    entry $S.f.entry -width 5 -borderwidth 2 -relief sunken
    bind $S.f.entry <Return> "Nv_scaleCallback $S e $decimal $cmd " 
    pack $S.scale -side $side 
    pack $S.f -side $side 
    pack $S.f.label -side $text_side 
    pack $S.f.entry -side $text_side 

    return $S
}

proc Nv_mkFloatScale { S {orient v} {name ---} {from 10000} {to 0} {curr 500} {cmd null} {decimal 0}} {
    frame $S 
    frame $S.f
    
    if { $orient == "v" } {
	set side left
	set text_side top
	set orient v
    } else {
	set side top
	set text_side left
	set orient h
    }
    
    scale $S.scale -from $from -length 140 -showvalue 0 -orient $orient \
	-digits 0 -resolution [expr abs(($to - $from)/140.0)] \
	-tickinterval 0 -to $to -width 13 \
	-command "Nv_floatscaleCallback $S s 0 $cmd " \
	-activebackground gray80 -background gray90
    
    label $S.f.label -text $name
    entry $S.f.entry -width 5 -borderwidth 2 -relief sunken
    bind $S.f.entry <Return> "Nv_floatscaleCallback $S e 0 $cmd " 
    pack $S.scale -side $side 
    pack $S.f -side $side 
    pack $S.f.label -side $text_side 
    pack $S.f.entry -side $text_side 
    
    Nv_floatscaleCallback $S b 0 $cmd $curr
    
    return $S
}

############################################################################
# procedure to make  pulldown menus for menu buttons 
###########################################################################
proc Nv_mkMenu { P mname bnames underlines commands} {
    global Nv_

    menubutton $P -text $mname -menu $P.m -underline 0
    menu $P.m
    set j 0
    foreach i $bnames { 
	set cmd [concat [lindex $commands $j] \"$i\"]
	set underline [lindex $underlines $j]
	$P.m add command -label $i -underline $underline -command \
	    "inform [concat $i]; $cmd" 
	incr j
    }

    return $P
}

proc incrEntry { E } {
    set val [$E get]
    if {[catch {incr val}]} {set val 1}

    $E delete 0 end 
    $E insert 0 $val
}
proc decrEntry { E } {
    set val [$E get]
    if {[catch {incr val -1}]} {set val 1}
    if {$val < 1} {set val 1}

    $E delete 0 end 
    $E insert 0 $val
}


proc Nv_mkArrows {A {name ""} {cmd null} {val 1} {orient v} } {
    global bit_map_path
    frame $A 
    frame $A.f1
    frame $A.f2

    if { $orient == "v" } {
		set side left
		set text_side top
		set up up
		set down down
		set orient v
    } else {
		set side top
		set text_side left
		set up right
		set down left 
		set orient h
    }

    button $A.f1.up -bitmap @$bit_map_path/$up -command "incrEntry $A.f2.entry; $cmd $A.f2.entry"
    button $A.f1.down -bitmap @$bit_map_path/$down -command "decrEntry $A.f2.entry; $cmd $A.f2.entry"

    pack $A.f1.up $A.f1.down -side $text_side
    label $A.f2.label -text $name
    entry $A.f2.entry -width 5  -relief flat 
    $A.f2.entry delete 0 end 
    $A.f2.entry insert 0 $val
    pack $A.f1 -side $side 
    pack $A.f2.label -side $text_side 
    pack $A.f2.entry -side $text_side 
    pack $A.f2 -side $side 

    return $A
}
############################################################
proc Nv_mkPanelname {P name} {

    frame $P.name -relief groove -borderwidth 2
    label $P.name.label -text $name
    pack $P.name  -fill x -side top
    pack $P.name.label -expand 1 

    return $P.name
}

#########################################################
proc Nv_mkAttbutton {P name} {
    frame $P.$name
    button $P.$name.b -text "$name" -anchor nw -width 10 \
	-command "mkAttPopup $P.$name.pop $name 1"
    
    set txt [get_curr_status $name]
    
    label $P.$name.l -text $txt -anchor ne
    pack $P.$name.b -side left
    pack $P.$name.l -side right
    
    return $P.$name
}

###################################################################
# makes sunken frame with a checkbutton for each item in list L
###################################################################
proc Nv_mkSurfacelist { P L C} {

    frame $P -relief sunken
    set j 0
    foreach i $L {
	set name [Nget_map_name $i surf]
	checkbutton $P.$j  -relief flat -text $name -anchor w\
	    -command "change_surf_list $C $i" \
	    -variable "SL$P.$j"
	
	if {0 != $C} {
	    if {[$C surf_is_selected Nsurf$i]} then {
		$P.$j select
	    } else {
		$P.$j deselect
	    }
	}
	
	pack $P.$j -fill x -expand 1 -side top
	incr j
    }
    
    return $P
}

proc change_surf_list {C id} {

#puts "change_surf_list $C $id" 
    
    if {0 != [$C surf_is_selected Nsurf$id]} {
	$C unselect_surf Nsurf$id
    } else {
	$C select_surf Nsurf$id
    }
    
}

proc Nget_map_list { type } {
    set map_list ""

    switch $type {
	"surf"	{ set map_list [Nget_surf_list] }
	"vect"	{ set map_list [Nget_vect_list] }
	"site"	{ set map_list [Nget_site_list] }
	"vol"	{ set map_list [Nget_vol_list] }
    }
    
    return $map_list
}

proc Nget_map_name {id type} {
    switch $type {
	"surf" { 
	    set map_name [Nsurf$id get_att topo]
	    if {[lindex $map_name 0] == "map"} then {
		return [lindex $map_name 1]
	    } else {
		return "constant#$id"
	    }
	}
	"vect" { return [Nvect$id get_att map] }
	"site" { return [Nsite$id get_att map] }
    }
}

proc mkMapList { P type {cmd null}} {

    catch {destroy $P}
    set list [Nget_map_list $type]
    set name [Nget_current $type]

    if {[llength $list] == 0} {
	set name "None Loaded"
    } else {
	set n [lsearch $list $name]
	set list [lreplace $list $n $n]
	set name [Nget_map_name $name $type]
    }

    menubutton $P -text $name -menu $P.m
    menu $P.m -tearoff 0
    foreach i $list {
	set map_name [Nget_map_name $i $type]
	$P.m add command -label "$map_name" \
	    -command "inform Current $type: $i; set_new_curr $type $i; $cmd $i"
    }

    return $P
}

proc set_new_curr {type name} {
    global Nv_
    
    if { $name != 0 } then {
	set L [Nget_map_list $type]
	set n [lsearch -exact $L $name]
#puts "NAME = [Nget_map_name $name $type] LIST = $L INTERNAL NAME: $name"
    }

    Nset_current $type $name

    # reset panel
    set cmd mk$type\Panel
    set W $Nv_(P_AREA).$type
    set pos [Q_get_pos $Nv_(Q) $Nv_($W)]
    $cmd $W
    Nv_openPanel $type $pos
}


############################################################
# These two routines replace equivalent routines in C code # 
############################################################
proc Nget_current { type } {
    global Nv_

    switch $type {
	"surf" { return $Nv_(CurrSurf) }
	"vect" { return $Nv_(CurrVect) }
	"site" { return $Nv_(CurrSite) }
	"sdiff" { return $Nv_(CurrSdiff) }
    }
}

proc Nset_current { type id } {
    global Nv_

    switch $type {
	"surf" { set Nv_(CurrSurf) $id }
	"vect" { set Nv_(CurrVect) $id }
	"site" { set Nv_(CurrSite) $id }
	"sdiff" { set Nv_(CurrSdiff) $id }
    }
}

# Quick routine to make a separator widget
proc Nv_makeSeparator { name } {
    canvas $name -relief raised -height 2m -width 5m -bg \#111111
}



