/*
 *	isetname and isetcolormap -
 *
 *				Paul Haeberli - 1984
 *
 */
#include	<stdio.h>
#include	<string.h>
#include	"image.h"


void isetname(IMAGE *image, char *name)
{
    strncpy(image->name, name, 80 * sizeof(char));
}

void isetcolormap(IMAGE *image, int colormap)
{
    image->colormap = colormap;
}
