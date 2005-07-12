/****************************************************************************
*
* MODULE:       Symbol library 
*   	    	
* AUTHOR(S):    Radim Blazek
*
* PURPOSE:      Stroke symbol
*
* COPYRIGHT:    (C) 2001 by the GRASS Development Team
*
*               This program is free software under the GNU General Public
*   	    	License (>=v2). Read the file COPYING that comes with GRASS
*   	    	for details.
*
*****************************************************************************/
#include <stdlib.h>
#include <math.h>
#include "gis.h"
#include "symbol.h"

#define PI M_PI

void 
add_coor ( SYMBCHAIN *chain, int x, int y)
{
    G_debug ( 5, "    add_coor %d, %d", x, y );
    if ( chain->scount == chain->salloc ) {
	chain->salloc += 10;
	chain->sx = (int *) G_realloc ( chain->sx, chain->salloc * sizeof(int) );
	chain->sy = (int *) G_realloc ( chain->sy, chain->salloc * sizeof(int) );
    }
    chain->sx[chain->scount] = x;
    chain->sy[chain->scount] = y;
    chain->scount++;
}

/* draw chain
*   s - scale
*   ch - chain number 
*/
int 
stroke_chain ( SYMBPART *part, int ch, double s ) 
{ 
    int k, l, first;
    SYMBEL *elem;
    SYMBCHAIN *chain;
    double r;
    double a1, a2, da;
    int x, y, x0, y0;

    G_debug ( 5, "  stroke_chain(): ch = %d", ch );
    chain = part->chain[ch];
    
    G_debug ( 5, "    element count = %d", chain->count );
    first = 1;
    for ( k = 0; k < chain->count; k++ ) {
	elem = chain->elem[k];
	switch ( elem->type ) {
	    case S_LINE:
                G_debug ( 5, "    LINE count = %d", elem->coor.line.count );
		for (l = 0; l < elem->coor.line.count; l++) {
		    add_coor ( chain, s * elem->coor.line.x[l], s * elem->coor.line.y[l] );
		    if ( first ) {
			x0 = s * elem->coor.line.x[l];
			y0 = s * elem->coor.line.y[l];
			first = 0;
		    }
		}
		break;
	    case S_ARC:
		da = 10 * PI / 180 ; /* later calc from size and tolerance */
		r = elem->coor.arc.r;
                G_debug ( 5, "    ARC da = %f r = %f", da, r );
		
	        /* convert to positive angles */
	        a1 = PI * elem->coor.arc.a1 / 180; 
	        if ( a1 < 0 ) a1 += 2 * PI; 
	        a2 = PI * elem->coor.arc.a2 / 180; 
	        if ( a2 < 0 ) a2 += 2 * PI; 

	        if ( elem->coor.arc.clock ) {	/* clockwise */
		    while (1) {
		        x = s * elem->coor.arc.x + s * r * cos(a1);
		        y = s * elem->coor.arc.y + s * r * sin(a1);
		        add_coor ( chain, x, y);
			if ( first ) {
			    x0 = x;
			    y0 = y;
			    first = 0;
			}
			if ( a1 == a2 ) break;
			a1 -= da;
			if ( a1 < a2 ) a1 = a2;
		    }

		} else { 
		    while (1) {
		        x = s * elem->coor.arc.x + s * r * cos(a1);
		        y = s * elem->coor.arc.y + s * r * sin(a1);
		        add_coor ( chain, x, y);
			if ( first ) {
			    x0 = x;
			    y0 = y;
			    first = 0;
			}
			if ( a1 == a2 ) break;
			a1 += da;
			if ( a1 > a2 ) a1 = a2;
		    }
	        }
		break;
	}
    }
    if ( part->type == S_POLYGON ) {
	add_coor ( chain, x0, y0); /* Close ring */
    }

    return 0;
}

/* 
*  Stroke symbol to form used for Xdriver.
*
*  rotation and tolerance currently not supported
*/
void
S_stroke ( SYMBOL *Symb, int size, double rotation, int tolerance )
{
    int i, j;
    double s;
    SYMBPART *part; 

    G_debug ( 3, "S_stroke(): size = %d rotation = %f tolerance = %d", size, rotation, tolerance );

    /* TODO: support for rotation and tolerance */

    s = size * Symb->scale;
    
    for ( i = 0; i < Symb->count; i++ ) { 
        G_debug ( 4, "  part %d", i );
	part = Symb->part[i];
	switch ( part->type ) {
	    case S_POLYGON: 
		for ( j = 0; j < part->count; j++ ) { /* RINGS */
		    stroke_chain ( part, j, s);
		}
		break;
	    case S_STRING: /* string has 1 chain */
		stroke_chain ( part, 0, s);
		break;
	}
    }
}

