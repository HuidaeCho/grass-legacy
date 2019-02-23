
/*
#define		WRITE_ATT_FORMAT	"%c  %12.2lf  %12.2lf  %8d"
#define		WRITE_ATT_FORMAT	"%c %14.2lf %14.2lf %7d"
*/
#define		WRITE_ATT_FORMAT	"%c %14s %14s %7d"
#define FlSIZ 14
#define		READ_ATT_FORMAT		"%c %lf %lf %d"

/*  only types allowed in atts file 
*	A - area, L - line, P - point
*/
#define		ATT_TYPES		"LAP"

#define LINE 0
#define AREA 1
#define DOT 2
#define DEAD_LINE 4
#define DEAD_AREA 5
#define DEAD_DOT 6

struct attribute
{
	char type ;
	double x ;
	double y ;
	int cat ;
	long offset ;
} ;


struct atts_index
{
	long *area_off ;
	long *line_off ;
	long *point_off ;
	int area_alloc ;
	int line_alloc ;
	int point_alloc ;
	int max_areas ;
	int max_lines ;
	int max_points ;
	int max_atts ;
} ;

