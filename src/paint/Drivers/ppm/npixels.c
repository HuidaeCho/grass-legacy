#include <stdio.h>
Pnpixels (rows, cols)
    int *rows, *cols;
{
    char *getenv(), *p;

    if (NULL == (p = getenv ("HEIGHT")) || sscanf(p,"%d",cols) != 1 || *cols <= 0)
	    *rows = 1000;
    if (NULL == (p = getenv ("WIDTH")) || sscanf(p,"%d",cols) != 1 || *cols <= 0)
	    *cols = 750;
}
