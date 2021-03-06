
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <grass/gis.h>
#include "pngdriver.h"

static unsigned int get_2(const unsigned char **q)
{
    const unsigned char *p = *q;
    unsigned int n = (p[0] << 0) | (p[1] << 8);

    *q += 2;
    return n;
}

static unsigned int get_4(const unsigned char **q)
{
    const unsigned char *p = *q;
    unsigned int n = (p[0] << 0) | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);

    *q += 4;
    return n;
}

static int read_bmp_header(const unsigned char *p)
{
    if (*p++ != 'B')
	return 0;
    if (*p++ != 'M')
	return 0;

    if (get_4(&p) != HEADER_SIZE + width * height * 4)
	return 0;

    get_4(&p);

    if (get_4(&p) != HEADER_SIZE)
	return 0;

    if (get_4(&p) != 40)
	return 0;

    if (get_4(&p) != width)
	return 0;
    if (get_4(&p) != -height)
	return 0;

    get_2(&p);
    if (get_2(&p) != 32)
	return 0;

    if (get_4(&p) != 0)
	return 0;
    if (get_4(&p) != width * height * 4)
	return 0;

    get_4(&p);
    get_4(&p);
    get_4(&p);
    get_4(&p);

    return 1;
}

void read_bmp(void)
{
    char header[HEADER_SIZE];
    FILE *input;
    int x, y;
    unsigned int *p;

    if (!true_color)
	G_fatal_error("PNG: cannot use BMP with indexed color");

    input = fopen(file_name, "rb");
    if (!input)
	G_fatal_error("PNG: couldn't open input file %s", file_name);

    if (fread(header, sizeof(header), 1, input) != 1)
	G_fatal_error("PNG: invalid input file %s", file_name);

    if (!read_bmp_header(header))
	G_fatal_error("PNG: invalid BMP header for %s", file_name);

    for (y = 0, p = grid; y < height; y++) {
	for (x = 0; x < width; x++, p++) {
	    int b = fgetc(input);
	    int g = fgetc(input);
	    int r = fgetc(input);
	    int a = fgetc(input);
	    unsigned int c = get_color(r, g, b, a);

	    *p = c;
	}
    }

    fclose(input);
}
