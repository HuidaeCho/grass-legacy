#include "windows.h"
#include "gis.h"
#include "variables.h"

show_legend()
{
	struct Range range ;
	char buff[256] ;

/* Draw legend */
	R_open_driver();
	Dchoose(LEG.name) ;
	Derase("black") ;
	R_close_driver();

	if (-1 != G_read_range(mapname, mapset, &range))
	{
		int cats_num ;
		cats_num = range.pmax - range.nmin + 1 ;
		if (cats_num < 25)
		{
			sprintf(buff,"'%s@%s' lines=30", mapname, mapset) ;
			gorun("d.legend", buff) ;
		}
		else
		{
			if (cats_num < 12 * 25)
				sprintf(buff,"'%s@%s' lines=25 cols=12", mapname, mapset) ;
			else
				sprintf(buff,"'%s@%s'", mapname, mapset) ;
			gorun("d.colortable", buff) ;
		}
	}
}
