#include "global.h"
static int page = 0;
static char *date = NULL;
header(unit1, unit2)
{
    int i,k;
    char buf[1024];
    char tbuf1[256], tbuf2[256];
    char north[50], south[50];
    char east[50], west[50];
    char ns_res[50], ew_res[50];
    int len1, len2;
    char *label;
    char *mask;
    char *maskinfo();
    char *print_label();

    nlines = page_length;
    if (date == NULL)
	date = G_date();

    divider("+");
    page++;

    if (page == 1 || with_headers)
    {
	lcr ("", "MAP LAYER CATEGORY REPORT", "", buf,page_width-2);
	pbuf(buf);
	sprintf (tbuf1, "LOCATION: %s", G_location());
	if (with_headers && (page_length > 0))
	    sprintf (tbuf2, "Page %d", page);
	else
	    *tbuf2 = 0;
	lcr (tbuf1, tbuf2, date, buf,page_width-2);
	pbuf(buf);

	divider("|");

	G_format_northing (window.north, north, window.proj);
	G_format_easting  (window.east,  east, window.proj);
	G_format_northing (window.south, south, window.proj);
	G_format_easting  (window.west,  west, window.proj);
	G_format_resolution (window.ns_res, ns_res, window.proj);
	G_format_resolution (window.ew_res, ew_res, window.proj);
	len1 = max(strlen (north), strlen(south));
	len1 = max(len1, strlen(ns_res));
	len2 = max(strlen (east), strlen(west));
	len2 = max(len2, strlen(ew_res));

	sprintf (buf, "%-9s north: %*s    east: %*s",
		"", len1, north, len2, east);
	pbuf(buf);

	sprintf (buf, "%-9s south: %*s    west: %*s",
		"WINDOW", len1, south, len2, west);
	pbuf(buf);

	sprintf (buf, "%-9s res:   %*s    res:  %*s",
		"", len1, ns_res, len2, ew_res);
	pbuf(buf);

	divider("|");

	label = nlayers > 1 ? "LAYERS:" : "LAYER:" ;
	len1 = strlen (label)+1;

	mask = maskinfo();
	label = "MASK:";
	while (mask)
	{
	    printf ("|%-*s", len1, label);
	    label = "";
	    mask = print_label (mask, page_width-len1-2, 1, 0, ' ');
	    printf ("|");
	    newline();
	}
	divider("|");


	label = nlayers > 1 ? "LAYERS:" : "LAYER:" ;
	for (i = 0; i < nlayers; i++)
	{
	    sprintf (buf, "%-*s%*s%s in %s", len1, label,
		    i*2, "", layers[i].name, layers[i].mapset);
	    pbuf(buf);
	    label = "";
	}
	divider("|");
    }

    len1 = layers[0].clen+layers[0].nlen;
    for (k = 0; k < 2; k++)
    {
	if (k == 0)
	    lcr ("", "Category Information", "", buf, len1);
	else
	{
	    sprintf (tbuf1, "%*s|description", layers[0].nlen, "#");
	    lcr (tbuf1, "", "", buf, len1);
	}
	printf ("|%s ", buf);
	for (i = unit1; i <= unit2; i++)
	    printf ("|%*s", unit[i].len,unit[i].label[k]);
	printf ("|");
	newline();
    }
    divider("|");
}

static
pbuf(buf)
    char *buf;
{
    printf ("|%-*.*s|", page_width-2, page_width-2, buf);
    newline();
}

static
divider(edge)
    char *edge;
{
    int n;
    printf ("%s", edge);
    n = page_width - 2;
    while(--n >= 0)
	printf ("-");
    printf ("%s", edge);
    newline();
}

static
max(a,b)
{
    return a>b?a:b;
}

trailer()
{
    divider("+");
    while (nlines > 0)
	newline();
    if (use_formfeed)
	printf ("\f");
}

newline()
{
    printf ("\n");
    nlines--;
}

lcr(left,center,right,buf,n)
    char *left;
    char *center;
    char *right;
    char *buf;
{
    int ll, lc, lr;

    ll = strlen (left);
    lc = strlen (center);
    lr = strlen (right);

    sprintf (buf, "%s%*s%s%*s%s",
	left,
	(n-lc)/2-ll, "", center,
	n-lr-lc-(n-lc)/2,"",right);
}
