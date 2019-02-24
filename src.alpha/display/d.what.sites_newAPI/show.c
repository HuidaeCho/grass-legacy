#include "gis.h"

static int nlines = 100;

show_buttons (once)
{
    if (once)
    {
	fprintf (stderr, "\nClick mouse button on desired location\n\n");
	nlines = 3;
    }
    else if (nlines >= 18)	/* display prompt every screen full */
    {
	fprintf (stderr, "\n");
	fprintf (stderr, "Buttons\n");
	fprintf (stderr, " Left:  what's here\n");
	fprintf (stderr, " Right: quit\n");
	nlines = 4;
    }
}
