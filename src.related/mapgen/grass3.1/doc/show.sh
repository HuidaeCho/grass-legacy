

                                   APPENDIX I                    1
                              GRASS-MAPGEN User's Guide
            show.sh                                         show.sh            show.sh                                         show.sh




            
            
            NAME            NAME
            show.sh - Draws the contents of the map composition on the
            graphic screen.
            
            SYNOPSIS            SYNOPSIS
            show.sh [-m mapdef_file] [overlay1] [overlay2] ...            show.sh                                                                ______________   ________   ________     
            
            DESCRIPTION            DESCRIPTION
            show.sh draws the contents of the map composition, which are            _______                                                     
            stored in the ovm directory, in the selected GRASS window.                          ___                                         
            
            Mapdef_file is the map definition file to use; the default            ___________                                               
            is map.def.  overlay1 is a graphic overlay file to display;               _______   ________                                      
            the default is ovm/*, which shows all graphic overlay files                           _____                                       
            in the ovm directory.                   ___           
            
            Each successive command adds to the existing screen content.
            To clear the screen, the user must issue the GRASS command
            Derase or Dscreen.            ______    _______ 
            
            The graphic monitor must be started with the Dstart.mon                                                         __________
            command, and then be selected with Dselect.mon DVGA command                                               ________________        
            before running show.sh.                           _______ 
            
            A file named .colortable, in the user's current directory,
            will enable the user to revise the graphic screen colors.
            Colors are changed by increasing or deceasing the 0-255
            color levels for red, blue and/or green (see .colortable in                                                         ___________   
            APPENDIX I-A).
            
            EXAMPLES            EXAMPLES
            
                 show.sh ovm/*                      # all overlays
                 show.sh ovm/cornfld ovm/fields     # 2 overlays
                 show.sh ovm/* ../[map2]/ovm/*      # all overlays, 2
                                                       maps
                 show.sh -m test.def ovm/*          # all overlays when
                                                       other than
                                                       map.def is used
            
            SEE ALSO            SEE ALSO
                 .colortable         GRASS-MAPGEN User's Guide, APPENDIX                                                                                         ___________                                            
                                     1-A, USDA-SCS.
                 monitor commands    GRASS User's Manual, Version 3.1.
            
            AUTHOR            AUTHOR
            Marty Holko, Computer Systems Analyst, Cartography and
            Geographic Information Systems Division, USDA, Soil
            Conservation Service, Washington, DC. (October l990).