/* Functions: Panel_save, Panel_restore, Panel_delete	P.W. Carlson 12/88 */

#include <stdio.h>
#include "vdc600.h"

Panel_save(name, top, bottom, left, right)
int top, bottom, left, right;
char *name;
{
    register int y, n;
    int fd, numpixels;
    int i, paintfile;
    unsigned char buff[640];
    unsigned char R, G, B;
    float red, green, blue;

    numpixels = right - left + 1;

    /* is this file to be painted? */
    paintfile = (top | bottom | left | right) ? 0 : 1;

    /* open the file */
    fd = creat(name, 0644);

    /* if not a paint file, write size info */
    if (!paintfile)
    {	write(fd, &top, sizeof(top));
    	write(fd, &bottom, sizeof(bottom));
    	write(fd, &left, sizeof(left));
    	write(fd, &numpixels, sizeof(numpixels));
    }

    /* if a paint file, write color and size info */
    else
    {	i = NCOLORS;
	write(fd, &i, sizeof(i));
	for (n = 0; n < i; n++)
    	{   read_dac(n, R, G, B);
	    red   = (float)R / 63.0;
	    green = (float)G / 63.0;
	    blue  = (float)B / 63.0;
    	    write(fd, &red,   sizeof(red));
    	    write(fd, &green, sizeof(green));
    	    write(fd, &blue,  sizeof(blue));
	}
	i = 1;
	write(fd, &i, sizeof(i));
	i = SCREEN_BOTTOM + 1;
	write(fd, &i, sizeof(i));
	i = SCREEN_RIGHT + 1;
	write(fd, &i, sizeof(i));
	bottom = SCREEN_BOTTOM;
	right = SCREEN_RIGHT;
	numpixels = right + 1;
    }

    /* get the raster and write it to the file */
    for (y = top; y <= bottom; y++)
    {	getraster(y, buff);
	write(fd, buff + left, numpixels);
    }
    close(fd);
}


Panel_restore(name)
char *name;
{
    int fd, top, bottom, left, right, numpixels;
    unsigned char buff[640];
    register int y;

    /* open the file and read the size info */
    fd = open(name, 0);
    read(fd, &top, sizeof(top));
    read(fd, &bottom, sizeof(bottom));
    read(fd, &left, sizeof(left));
    read(fd, &numpixels, sizeof(numpixels));

    /* read the raster from the file and display it */
    right = left + numpixels - 1;
    for (y = top; y <= bottom; y++)
    {	read(fd, buff, numpixels);
	putraster(left, y, right, y, buff);
    }
    close(fd);
}


Panel_delete(name)
char *name;
{
    unlink(name);
}
