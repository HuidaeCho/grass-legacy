
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <grass/gis.h>
#include "pngdriver.h"

static unsigned char *put_2(unsigned char *p, unsigned int n)
{
    *p++ = n & 0xFF;
    n >>= 8;
    *p++ = n & 0xFF;
    return p;
}

static unsigned char *put_4(unsigned char *p, unsigned int n)
{
    *p++ = n & 0xFF;
    n >>= 8;
    *p++ = n & 0xFF;
    n >>= 8;
    *p++ = n & 0xFF;
    n >>= 8;
    *p++ = n & 0xFF;
    return p;
}

static void make_bmp_header(unsigned char *p)
{
    *p++ = 'B';
    *p++ = 'M';

    p = put_4(p, HEADER_SIZE + width * height * 4);
    p = put_4(p, 0);
    p = put_4(p, HEADER_SIZE);

    p = put_4(p, 40);
    p = put_4(p, width);
    p = put_4(p, -height);
    p = put_2(p, 1);
    p = put_2(p, 32);
    p = put_4(p, 0);
    p = put_4(p, width * height * 4);
    p = put_4(p, 0);
    p = put_4(p, 0);
    p = put_4(p, 0);
    p = put_4(p, 0);
}

void write_bmp(void)
{
    char header[HEADER_SIZE];
    FILE *output;
    int x, y;
    unsigned int *p;

    output = fopen(file_name, "wb");
    if (!output)
	G_fatal_error("PNG: couldn't open output file %s", file_name);

    memset(header, 0, sizeof(header));
    make_bmp_header(header);
    fwrite(header, sizeof(header), 1, output);

    for (y = 0, p = grid; y < height; y++) {
	for (x = 0; x < width; x++, p++) {
	    unsigned int c = *p;
	    int r, g, b, a;

	    get_pixel(c, &r, &g, &b, &a);

	    fputc((unsigned char)b, output);
	    fputc((unsigned char)g, output);
	    fputc((unsigned char)r, output);
	    fputc((unsigned char)a, output);
	}
    }

    fclose(output);
}
