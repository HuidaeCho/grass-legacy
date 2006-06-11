/* 
 * $Id$
 * added little/big endian test Markus Neteler 9/2000 
 * changed 10/99 Jaro*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

#include <grass/gis.h>
#include <grass/image.h>
#include <grass/gstypes.h>


static void ierrfunc(char *ebuf)
{
    G_message("%s", ebuf);

    return;
}

int GS_write_rgb(char *name)
{
    int y, x;
    unsigned int xsize, ysize;
    IMAGE *image;
    unsigned char *pixbuf;
    unsigned short rbuf[8192];
    unsigned short gbuf[8192];
    unsigned short bbuf[8192];


    gsd_getimage(&pixbuf, &xsize, &ysize);

    if (pixbuf) {
	i_seterror(ierrfunc);

	if (NULL ==
	    (image = iopen(name, "w", VERBATIM(1), 3, xsize, ysize, 3))) {
	    G_warning("Unable to open %s for writing.", name);

	    return (-1);
	}

	for (y = 0; y < ysize; y++) {
	    for (x = 0; x < xsize; x++) {
	        rbuf[x] = pixbuf[(y * xsize + x) * 4 + 0];
	        gbuf[x] = pixbuf[(y * xsize + x) * 4 + 1];
	        bbuf[x] = pixbuf[(y * xsize + x) * 4 + 2];

		putrow(image, rbuf, y, 0);	/* red row */
		putrow(image, gbuf, y, 1);	/* green row */
		putrow(image, bbuf, y, 2);	/* blue row */
	    }
	}

	free(pixbuf);
	iclose(image);

	return (0);
    }

    return (-1);
}
