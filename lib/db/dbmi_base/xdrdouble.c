#include <stdlib.h>
#include "xdr.h"


int
db__send_double(d)
    double d;
{
    XDR xdrs;
    int stat;

    stat = DB_OK;

    xdr_begin_send (&xdrs);
    if(!xdr_double (&xdrs, &d))
	stat = DB_PROTOCOL_ERR;
    xdr_end_send (&xdrs);

    if (stat == DB_PROTOCOL_ERR)
	db_protocol_error();

    return stat;
}

int
db__recv_double (double *d)
{
    XDR xdrs;
    int stat;

    stat = DB_OK;
    xdr_begin_recv (&xdrs);
    if(!xdr_double (&xdrs, d))
	stat = DB_PROTOCOL_ERR;
    xdr_end_recv (&xdrs);

    if (stat == DB_PROTOCOL_ERR)
	db_protocol_error();

    return stat;
}


int
db__send_double_array (double *x, int n)
{
    XDR xdrs;
    int i;
    int stat;

    stat = DB_OK;

    xdr_begin_send (&xdrs);

    if(!xdr_int (&xdrs, &n))
	stat = DB_PROTOCOL_ERR;

    for (i = 0; stat == DB_OK && i < n; i++)
    {
	if(!xdr_double (&xdrs, x))
	    stat = DB_PROTOCOL_ERR;
	x++;
    }

    xdr_end_send (&xdrs);

    if (stat == DB_PROTOCOL_ERR)
	db_protocol_error();

    return stat;
}

/* returns an allocated array of doubles */
/* caller is responsible for free() */
int
db__recv_double_array (double **x, int *n)
{
    XDR xdrs;
    int i, count, stat;
    double y, *a;

    *x = NULL;
    *n = 0;

    stat = DB_OK;
    xdr_begin_recv (&xdrs);
    if (xdr_int (&xdrs, &count))
    {
	if (count <= 0)
	    stat = DB_PROTOCOL_ERR;
	a = (double *)db_calloc (count, sizeof (double));
	if (a == NULL && stat == DB_OK)
	    stat = DB_MEMORY_ERR;

	for (i = 0; i < count; i++)
	{
	    if (!xdr_double (&xdrs, &y))
	    {
		stat = DB_PROTOCOL_ERR;
		break;
	    }
	    if (a) a[i] = y;
	}
	if (stat != DB_OK)
	{
	    if (a != NULL) free(a);
	    a = NULL;
	}
    }
    else
	stat = DB_PROTOCOL_ERR;

    if (stat == DB_OK)
    {
	*x = a;
	*n = count;
    }
    else if (stat == DB_PROTOCOL_ERR)
	db_protocol_error();

    xdr_end_recv (&xdrs);

    return stat;
}
