/*  %W%  %G%  */

#include "graph.h"

R_polyline_abs(xarray, yarray, number)
	int *xarray, *yarray ;
	int number ;
{
	int n ;
	_send_ident(POLYLINE_ABS) ;
	n = number ;
	_send_int(&n) ;
	_send_int_array(number,xarray) ;
	_send_int_array(number,yarray) ;
}
