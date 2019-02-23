/*  @(#)main.c    2.1  6/26/87  */
/*
**  Last modified by Dave Gerdes  5/1988
**  US Army Construction Engineering Research Lab
*/

#include <signal.h>
#include "gis.h"

#define MAIN
#include "dig_head.h"
#include "digit.h"

#define DEBUG

#ifdef DEBUG
#include <sys/types.h>
#include <time.h>
#endif


static char *N_dig_file;
static char *N_plus_file;
static char *N_att_file;
static char *N_coor_file;
static char *N_digitizer;
static char *N_path;
static char *N_name;
static char *N_PPID;

/*
**  calling sequence
**  digit file_name path_to_mapset parent_pid digitizer_tty
*/
main(argc, argv)
    int argc;
    char **argv;
{
    FILE *digit, *fopen(), *attr, *plus;
    char buf[1024];
    int have_old;
    int have_plus;
    int have_attr;
    int    close_down();
    int ret;
    unsigned	short	getuid() ;
    unsigned	short	getgid() ;
    char *memptr;	/* for load_file */

#ifndef MASSCOMP
    if (geteuid () != 0 && getuid () != 0)
    {
	/* if running w/ no digitizer this is not important */
	if (strcmp (argv[4], "nodig"))
	{
	printf ( "Warning!  Digit not running as root.\n");
	printf ( "Please consult GRASS installation manual on how to set up DIGIT\n");
	if (!G_yes ("Do you want to continue any way?", 0))
	    exit (0);
	}
    }
#endif
    init_priority ();	/* set up permissions and stuff for higher nice value */

#ifdef DEBUG
    init_debug (argv[2]);
#endif

    /*
    Get_new_tty ();	
    */
    Data_Loaded = 0;
    Files_Open = 0;
    signal(SIGFPE, close_down) ;    /*  in case of floating point error  */

    /* Couldn't call G_gisinit () because of UID stuff */
    G_no_gisinit ("DIGIT3.0");

    putenv ("DPG_LOCK", "LOCKED");

    if (argc != 5)
    {
	sprintf(buf, "Usage: %s  path/mapset  file_name  PPID [ digitizer_tty | 'nodig' ]\n", argv[0]);
	G_fatal_error (buf);
    }

    CM = &Map1;

    /* store the original file names */
    {
	N_name = argv[2];
	N_path = argv[1];
	N_PPID = argv[3];

	sprintf (buf, "%s/%s/%s", N_path, "dig", N_name);
	N_dig_file= G_store (buf);

	sprintf (buf, "%s/%s/%s", N_path, "dig_plus", N_name);
	N_plus_file = G_store (buf);

	sprintf (buf, "%s/%s/%s", N_path, "dig_att", N_name);
	N_att_file = G_store (buf);

	sprintf (buf, "%s/%s/%s", N_path, "reg", N_name);
	N_coor_file = G_store (buf);

	N_digitizer = argv[4];
    }

    have_old = have_plus = have_attr = 0;
    if ( (digit = fopen(N_dig_file, "r+")) != NULL )
	have_old = 1;
    else
    {
	if ( (digit = fopen(N_dig_file, "w+") ) == NULL )
	{
	    sprintf (buf, "Not able to open <%s>\n", N_dig_file);
	    G_fatal_error (buf);
	}
	printf ( "\nCreating a new vector file\n");
	have_old = 0;
    }

    if ((plus = fopen (N_plus_file, "r+")) != NULL)
    {
	fclose (plus);
	have_plus = 1;
    }
    else
    {
	if ( (plus = fopen(N_plus_file, "w+") ) == NULL )
	{
	    if (have_old)
	    {
		G_fatal_error ("No dig_plus file exists. You must run support.vect\n");
	    }
	    sprintf (buf, "Not able to open <%s>\n", N_plus_file);
	    G_fatal_error  (buf);
	}
	fclose (plus);
	unlink (N_plus_file);
	have_plus = 0;
    }


    if ((attr = fopen (N_att_file, "r+")) != NULL)
	have_attr = 1;
    else
    {
	if ( (attr = fopen(N_att_file, "w+") ) == NULL )
	{
	    sprintf (buf, "Not able to open <%s>\n", N_att_file);
	    G_fatal_error (buf);
	}
	have_attr = 0;
    }

    if (have_old)
    {
	ret = dig_do_file_checks (N_plus_file, N_dig_file, N_att_file);
	if (ret < 0)
	{
	    fprintf (stderr, "Could not open dig_plus file\n");
	    fprintf (stderr, "You must first run support.vect\n");
	    sleep (4);
	    exit (-1);
	}
    }

    if (have_old)
	dig_read_head_binary(digit, &head);

    get_head_info(have_old);

    dig_write_head_binary(digit, &head);

    initialize (digit, attr, N_plus_file, N_coor_file);
    CM->digit_file = N_dig_file;
    CM->att_file = N_att_file;
    Files_Open = 1;

    /* if we have created a new dig file, then create new dig_plus file */
    if (!have_old)
	if ( 0 > write_out (0))
	{
	    fprintf (stderr, "\nError creating 'dig_plus' file!\n");
	    close_down (0);
	}

    if (! leave())
        goto clean_up;

    /* what follows is a big chunk of unorganized code to set states */
    /* correctly when there is no digitizer enabled. */
    /* this mostly involves turning off commands that only make sense */
    /* with a digitizer */
    if (strcmp ("nodig", N_digitizer) == 0)
    {
	register int i;
/*DEBUG*/ debugf ("Setting DIG_ENABLED = OFF\n");

/*MDIG 	Set_G_Mask (MG_DIGIT, OFF); 	Disable Digitize Menu */
	Dig_Enabled = 0;		/* Disable Digitizer */
	Window_Device = MOUSE; 		/* Setup Mouse as windowing device */
	Digtiz_Device = MOUSE;

	/* disable Point Marker */
	/* could have used ME_MARK as index but dont trust 
	** it the way I have been changing menus  
	*/
	for (i = 0 ; M_edit.item[i].text != NULL ; i++)
	    if (M_edit.item[i].command == MEC_MARK)
	    {
		M_edit.item[i].enabled = 0;
		break;
	    }
	/* Disable Toggle Pointing/Windowing/Digitizing Device */
	for (i = 0 ; M_custom.item[i].text != NULL ; i++)
	{
	    switch (M_custom.item[i].command) {
		case MCC_WINDOW:
		case MCC_DIGTIZ:
		case MCC_POINT:
		    M_custom.item[i].enabled = 0;
		    break;
		default:
		    break;
	    }
	}
	/* Disable Digitizing in stream mode */
	for (i = 0 ; M_digit.item[i].text != NULL ; i++)
	{
	    switch (M_digit.item[i].command) {
		case MDC_MODE:
		    M_digit.item[i].enabled = 0;
		    break;
		default:
		    break;
	    }
	}
	/* Disable Re-registering Map / Create Neat Line */
	/* I would like to be able to do this eventually */
	for (i = 0 ; M_tool.item[i].text != NULL ; i++)
	{
	    switch (M_tool.item[i].command) {
		case MTC_NEAT:
		case MTC_REGIST:
		    M_tool.item[i].enabled = 0;
		    break;
		default:
		    break;
	    }
	}
    }

    /* if we have a digit file, but no plus file, complain and exit */
    if (have_old && !have_plus)
    {
	BEEP;
	fprintf (stderr, "\n");
	fprintf(stderr, "No 'dig_plus' file exists. Run import.vect first\n\n");
	close_down (0);

	Extended_Edit = 0;
    }

    if (Dig_Enabled)
    {
	if (D_setup_driver(N_digitizer) <0)
	    close_down() ;

	if ( reset_map(digit, N_coor_file) <0)
	    close_down() ;
    }
    else
    {
	if (head.orig_scale == 0)
	{
	    BEEP;
	    fprintf (stderr, "\r\n Original Scale is not set!\n");
	    sleep (3);
	    close_down(0);
	}
    }
    /* set_thresh() is called after load_plus () */
    

/*DEBUG*/ debugf ("entering write_head_binary ()\n");
    dig_write_head_binary(digit, &head);

    if (do_graphics()) 
    {
	Write_info (2, " Graphics set.");

/*DEBUG*/ debugf ("entering init_window ()\n");
	init_window();
	R_standard_color( dcolors[CLR_ERASE]);
	erase_window();
	outline_window();
    }

    if (have_old)
    {
/*DEBUG*/ debugf ("HAVE_OLD\n");
	dig_read_head_binary(digit, &head);
/*DEBUG*/ debugf ("   Read head\n");
	if (!Extended_Edit)
	{
	    fprintf (stderr, "\nFIRST_PLOT REACHED.  EXITTING\n"), close_down (-1);
	    first_plot(digit);
	}
	else
	{
	    dig_load_plus (CM, digit, 0);
/*DEBUG*/ debugf ("Entering REPLOT\n");
	    replot (CM);
/*DEBUG*/ debugf ("Exit from REPLOT\n");
	}
    }
    else
    {
/*DEBUG*/ debugf ("Creating new PLUS file\n");
	if (Extended_Edit)
	    init_plus (CM);	/* initialize arrays */
    }
    set_thresh();

    Data_Loaded = 1;

/*DEBUG*/ debugf ("Entering INTERACT\n");
    interact();

    if (do_graphics())
	R_close_driver();

    _Clear_info ();
    if (Changes_Made)
    {
	    if ( 0 > write_out (1))
	    {
		ret = -1;
		goto clean_up;
	    }
#ifdef FOO
	if (curses_yes_no (2, "  Changes Made: Do you want to save this session? "))
	{
	    if ( 0 > write_out (1))
	    {
		ret = -1;
		goto clean_up;
	    }
	}
	else
	{
	    if (curses_yes_no (2, "   Changes will be lost!  Do you want to save the session? "))
		if (0 > write_out (1))
		{
		    ret = -1;
		    goto clean_up;
		}
	}
#endif
    }

    ret = 55;	/* normal termination code */
clean_up:
    last_words (CM, ret);
    exit (ret);  /*redundant */
}

last_words (map, ret)
    struct Map_info *map;
{
    register int att;
    P_ATT *Att;

    fclose (map->digit);
    if (Extended_Edit)
    {
	fclose (map->att);
	if (Changes_Made)
	{
	    if (curses_yes_no_default (3, "Do you want to compress the Atts file? ", 0))
	    {
		Write_info (3, "Updating Att file...");
		/* update the dig_att file to agree with the digit atts */
		if (0 > unlink (map->att_file))
		{
		    Write_info (3, "Error trying to unlink att file");
		}
		if (NULL == (map->att = fopen (map->att_file, "w")))
		{
		    Write_info (3, "Error openning att file. Attributes may be lost");
		}
		else
		{
		    for (att = 1 ; att <= map->n_atts ; att++)
		    {
			Att = &(map->Att[att]);
			if (ATT_ALIVE (Att))
			{
			    write_att (map->att, (char) dig_new_to_old_type (Att->type), Att->x, Att->y, Att->cat);
			}
		    }
		    fclose (map->att);
		    Write_info (3, "Updating Att file... DONE.");
		}
	    }
	}
    }

    if (Curses_state ())
	Close_curses();
#ifdef DEBUG
    close_debug();
#endif

    do_file_checks (map);

    exit (ret);
}

do_file_checks (map)
    struct Map_info *map;
{
    FILE *fp;
    struct Plus_head Plus;

    if ((fp = fopen (map->plus_file, "r+")) == NULL)
    {
	G_fatal_error ("Can't open Plus file for final write\n");
    }
    dig_Rd_Plus_head (&Plus, fp);
    rewind (fp);
    dig_write_file_checks (fp, map, &Plus);
    fclose (fp);
}

/* exit status code needs to be finished */
/* EXIT (-5)  abnormal, but data should be ok */
/* EXIT (-1)  abnormal, Data should not be trusted */
close_down()
{
	int ret;

	Close_curses() ;

	if (Data_Loaded)
	{
	    Write_info(1, "Program exitting abnormally! Attempting to save data ");
		    if ( 0 > write_out (1))
		    {
			BEEP;
			Write_info (2, "Write FAILED!!!");
			sleep (4);
			ret = -1 ;
		    }
		    else
		    {
			Changes_Made = 0;
			Write_info (2, "File updated...");
			sleep (2);
			ret = -5 ;
		    }

	}
	    
	ret = -5 ;
end:

    last_words (CM, ret);
    exit (ret); /* redundant */
}


#ifdef DEBUG

static FILE *debugfp;
static int debug_on;
init_debug (file)
     char *file;
{

    debug_on = 0;
    if (!getenv ("DEBUG"))
	return ;
    if (strcmp ("wy50", getenv ("DEBUG")) == 0)
    {
	/* set 43 lines/page */
	fprintf (stderr, "%ce+", 27);
	/* split  24/19 */
	fprintf (stderr, "%cx18", 27);
	sleep (1);
	flush_keyboard ();
	debug_on = 1;
    }
    else
    {
	if ((debugfp = fopen (getenv ("DEBUG"), "a")) == NULL)
	{
	    if ((debugfp = fopen (getenv ("DEBUG"), "w")) == NULL)
	    {
		fprintf (stderr, "NO DEBUG\n");
		debug_on = 0;
	    }
	    else
		debug_on = 2;
	}
	else
	    debug_on = 2;
    }
    if (debug_on)
    {
	long timer;

	setbuf (debugfp, NULL);
	time (&timer);
	debugf ("\n\nSTARTUP: %s", asctime (localtime(&timer)));
	debugf ("USER: %s  File: %s\n", G_whoami(), file);
    }
}

close_debug()
{
    switch (debug_on) {
	case 1:
#ifdef FOO
	    /* set 24 rows */
	    fprintf (stderr, "%ce(", /*)*/  27);
	    /* set to full screen */
	    fprintf (stderr, "%cx0", 27);
	    flush_keyboard ();
#endif
	    break;
	case 2:
	    fclose (debugfp);
	    break;
	default:
	    break;
    }
    debug_on = 0;
}
debugf (format, a, b, c, d, e, f, g, h, i, j, k, l)
    char *format;
    int a, b, c, d, e, f, g, h, i, j, k, l;
{
      
    char buf[1024], *p;

    if (!debug_on)
	return 0;

    switch (debug_on) {
	case 1:
	    /* { */
	    fprintf (stderr, "%c}", 27);

	    sprintf (buf, format, a, b, c, d, e, f, g, h, i, j, k, l);
	    for (p = buf ; *p ; p++)
	    {
		fputc (*p, stderr);
		if (*p == '\n')
		    fputc ('\r', stderr);
	    }

	    fprintf (stderr, "%c]", 27);
	    break;
	case 2:
	    fprintf (debugfp, format, a, b, c, d, e, f, g, h, i, j, k, l);
	    break;
	default:
	    break;
    }
}
#endif
