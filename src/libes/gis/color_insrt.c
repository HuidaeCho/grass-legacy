/* This routine is public only because source is in different files.
 * It should NEVER be called directly.
 * It is used by G_add_color_rule() and G__read_old_colors().
 * These routines know when it is approriate to call this routine.
 */
#include "gis.h"
#define umalloc(n) (unsigned char *) G_malloc((int)n)
#define urealloc(s,n) (unsigned char *) G_realloc(s,(int)n)

#define LIMIT(x) if (x < 0) x = 0; else if (x > 255) x = 255;

G__insert_color_into_lookup (cat, red, grn, blu, cp)
    CELL cat;
    int red, grn, blu;
    struct _Color_Info_ *cp;
{
    long nalloc;
    long i;
    long newlen, curlen, gap;

    LIMIT(red);
    LIMIT(grn);
    LIMIT(blu);

/* color number 0 is a special case */
    if (cat == 0)
    {
	cp->lookup.r0 = red;
	cp->lookup.g0 = grn;
	cp->lookup.b0 = blu;
	cp->lookup.s0 = 1;
	return 1;
    }

/* first color? */
    if (!cp->lookup.active)
    {
	cp->lookup.active = 1;
	cp->lookup.nalloc = 256;
	cp->lookup.red = umalloc(cp->lookup.nalloc);
	cp->lookup.grn = umalloc(cp->lookup.nalloc);
	cp->lookup.blu = umalloc(cp->lookup.nalloc);
	cp->lookup.set = umalloc(cp->lookup.nalloc);
	cp->max = cp->min = cat;
    }

/* extend the color table? */
    else if (cat > cp->max)
    {
	curlen = cp->max - cp->min + 1;
	newlen = cat - cp->min + 1;
	nalloc = newlen;
	if (nalloc != (int) nalloc)        /* check for int overflow */
	    return -1;

	if (nalloc > cp->lookup.nalloc)
	{
	    while (cp->lookup.nalloc < nalloc)
		cp->lookup.nalloc += 256;
	    nalloc = cp->lookup.nalloc;

	    cp->lookup.red = urealloc (cp->lookup.red, nalloc);
	    cp->lookup.grn = urealloc (cp->lookup.grn, nalloc);
	    cp->lookup.blu = urealloc (cp->lookup.blu, nalloc);
	    cp->lookup.set = urealloc (cp->lookup.set, nalloc);
	}

	/* fill in gap with white */
	for (i = curlen; i < newlen; i++)
	{
	    cp->lookup.red[i] = 255;
	    cp->lookup.grn[i] = 255;
	    cp->lookup.blu[i] = 255;
	    cp->lookup.set[i] = 0;
	}
	cp->max = cat;
    }
    else if (cat < cp->min)
    {
	curlen = cp->max - cp->min + 1;
	newlen = cp->max - cat + 1;
	gap    = newlen - curlen;
	nalloc = newlen;
	if (nalloc != (int) nalloc)        /* check for int overflow */
	    return -1;

	if (nalloc > cp->lookup.nalloc)
	{
	    while (cp->lookup.nalloc < nalloc)
		cp->lookup.nalloc += 256;
	    nalloc = cp->lookup.nalloc;

	    cp->lookup.red = urealloc (cp->lookup.red, nalloc);
	    cp->lookup.grn = urealloc (cp->lookup.grn, nalloc);
	    cp->lookup.blu = urealloc (cp->lookup.blu, nalloc);
	    cp->lookup.set = urealloc (cp->lookup.set, nalloc);
	}

    /* shift the table to make room in front */
	for (i = 1; i <= curlen; i++)
	{
	    cp->lookup.red[newlen-i] = cp->lookup.red[curlen-i];
	    cp->lookup.grn[newlen-i] = cp->lookup.grn[curlen-i];
	    cp->lookup.blu[newlen-i] = cp->lookup.blu[curlen-i];
	    cp->lookup.set[newlen-i] = cp->lookup.set[curlen-i];
	}

    /* fill in gap with white */
	for (i=1; i < gap; i++)
	{
	    cp->lookup.red[i] = 255 ;
	    cp->lookup.grn[i] = 255 ;
	    cp->lookup.blu[i] = 255 ;
	    cp->lookup.set[i] = 0 ;
	}
	cp->min = cat;
    }

/* set the color! */
    i = cat - cp->min;
    cp->lookup.red[i] = red;
    cp->lookup.grn[i] = grn;
    cp->lookup.blu[i] = blu;
    cp->lookup.set[i] = 1;

    return 1;
}
