/* %W% %G% */

#include "gis.h"

static char *tempfile;
FILE *opentemp();

main (argc, argv) char *argv[];
{
    char entry[1024];
    char question[100];
    char word1[100], word2[100];
    int section;
    int any;
    int interactive;
    FILE *fd, *popen();

    G_gisinit (argv[0]);
    tempfile = G_tempfile();
    G_clear_screen();
    printf ("GRASS online manual\n");
    interactive = 1;

    while (1)
    {
	do
	{
	    printf ("\n");
	    printf ("Which manual entry would you like to see?\n");
	    printf ("Enter \"L\" for a list of manual entries\n");
	    printf ("Hit RETURN to quit\n");
	    printf ("> ");
	}
	while (!G_gets(entry));
	any = 0;
	switch (sscanf (entry, "%s %s", word1, word2))
	{
	case 1:
	    if (strcmp (word1, "l") == 0 || strcmp (word1, "L") == 0)
	    {
		list (interactive, 1, 2);
		continue;
	    }
	    for (section = 1; section <= 9; section++)
		if (show (section, word1, any, interactive))
		    any = 1;
	    if (!any)
		printf ("%s - no such entry in the manual\n", word1);
	    break;

	case 2:
	    if (strcmp (word1, "l") == 0 || strcmp (word1, "L") == 0)
	    {
		if (sscanf (word2, "%d", &section) == 1)
		    list (interactive, section, section);
		else if (strcmp (word2,"all") == 0)
		{
		    for (section = 1; is_section(section); section++)
			    ;
		    list (interactive, 1, section-1);
		}
		continue;
	    }
	    if (sscanf (word2, "%d", &section) != 1)
		break;
	    if (!is_section (section))
		printf ("Manual does not have a section %d\n", section);
	    else if (!show (section, word1, any, interactive))
		printf ("%s - no such entry in section %d of the manual\n", word1, section);
	    break;

	default:
	    exit(0);
	}
    }
}

show (section, entry, any, interactive)
    char *entry;
{
    char buf[1024];
    char temp[3];
    FILE *in, *out;
    int blanks;

    sprintf (buf, "%s/man/%d/%s", G_gisbase(), section, entry);
    if ((in = fopen (buf, "r")) == NULL)
	return 0;
    if (interactive && any)
    {	
	sprintf (buf,
	"Another entry for %s exists in section %d\nWould you like to see it? ",
	    entry, section);
	if (!G_yes(buf,0))
	{
	    fclose (in);
	    return 1;
	}
    }

/* copy entry to temp file, squeezing multiple blank lines into 1 */
    out = opentemp ("w");
    blanks = 0;
    while (fgets (buf, sizeof buf, in))
    {
	if (sscanf (buf, "%1s", temp) != 1)
	    blanks = 1;
	else
	{
	    if (blanks)
		fprintf (out, "\n");
	    fprintf (out, "%s", buf);
	    blanks = 0;
	}
    }
    fclose (in);
    fclose (out);

    sprintf (buf, "%s %s", interactive?"more -d":"cat", tempfile);
    system (buf);
    unlink (tempfile);
    if (!interactive || !G_yes("\nWould you like to send this entry to the printer? ", 0))
	return 1;
    printf ("Sending to the printer ...\n");
    sprintf (buf, "lpr %s/man/%d/%s", G_gisbase(), section, entry);
    system (buf);
    return 1;
}

list (interactive, section1, section2)
{
    char buf[1024];
    int any;
    int section;
    unlink (tempfile);
    any = 0;
    for (section = section1; section <= section2; section++)
    {
	if (is_section(section))
	{
	    prepare(section);
	    any = 1;
	}
	else
	    printf ("Manual does not have a section %d\n\n", section);
    }
    if (!any) return;
    sprintf (buf, "%s %s", interactive ? "more -d" : "cat", tempfile);
    system (buf);
    if (!interactive || !G_yes("Would you like to send this list to the printer? ", 0))
	return 1;
    printf ("Sending to the printer ...\n");
    sprintf (buf, "lpr %s", tempfile);
    system (buf);
}

prepare (section)
{
    char buf[1024];
    FILE *fd;

    fd = opentemp ("a");
    fprintf (fd, "Section %d\n", section);
    fprintf (fd,
"------------------------------------------------------------------------\n");
    fclose (fd);
    sprintf (buf, "ls -C %s/man/%d >> %s", G_gisbase(), section, tempfile);
    system (buf);
    fd = opentemp ("a");
    fprintf (fd, "\n");
    fclose (fd);
    return 1;
}

FILE *
opentemp (mode)
    char *mode;
{
    FILE *fd;
    if(fd = fopen (tempfile, mode))
	return fd;
    G_fatal_error ("Can't open any temp files");
}

is_section (section)
{
    char buf[1024];
    sprintf (buf, "%s/man/%d", G_gisbase(), section);
    return (access(buf,0) == 0);
}
