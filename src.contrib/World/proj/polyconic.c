#include "map.h"

Xpolyconic (place, x, y) struct place *place ; float *x, *y ;
{
double r, alpha ;
float lat2, lon2 ;

if (abs (place->nlat.l) > .01)
	{
	r = place->nlat.c / place->nlat.s ;
	alpha = place->wlon.l * place->nlat.s ;
	*y = place->nlat.l + r * (1 - cos (alpha)) ;
	*x = -r * sin (alpha) ;
	}
   else {
	lon2 = place->wlon.l * place->wlon.l ;
	lat2 = place->nlat.l * place->nlat.l ;
	*y = place->nlat.l *  (1 + (lon2 / 2) * (1 - (8 + lon2) * lat2 / 12)) ;
	*x = -place->wlon.l * (1 - lat2 * (3 + lon2) / 6) ;
	}

return (1) ;
}

int (*polyconic ()) () { return (Xpolyconic) ; }
