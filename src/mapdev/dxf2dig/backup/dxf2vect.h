#include <stdio.h>
#include <string.h>
/* #include <stdlib.h> */
/* #include <float.h> */


#define DBL_MAX		9999999999999999999999.9
#define DBL_MIN		-99999999999999999999.9


#define DXF_ASCII 0
#define DXF_LABEL 1
#define DXF_LABEL_LINE 2

#define MAX_FILES 15 

#define POLYFLAG1 1

#define RSTEP  5.0

#define POINT  struct _point_
POINT
{
	double x, y;
};

#define DXF_DIG		struct dxf_dig
DXF_DIG
{
	char	*name;
	int 	type;
	FILE	*fd;
	int	status;
};

#define ARRAY_INCR	256

#ifdef MAIN
	char  	*dxf_file = NULL;
	long	file_size;
	int     percent;
	char	dig_path[240];
	char	basename[100];   /* dpg */
	POINT	*pt_array;
	int	arr_size;
	int	arr_max;
	double	n, s, e, w;
	long	n_off, s_off, e_off, w_off;
	char	zzero[8];
	char	eeight[8];
	char	tten[8];
	char	ttwenty[8];
	char	eelev[8];
	char	ttwentyone[8];
	char	entitie[12];
	char	header[12];
	char	extmin[12];
	char 	extmax[12];
	char	polyline[12];
	char	circle[12];
	char	text[12]; 	/* dpg */
	char	line[8];
	char	point[8];
	char	arc[8];
	char	vertex[8];
	char	seqend[8];
	char	dxf_line[80];
	int	num_open_layers = 0;
	int	num_closed_layers = 0;
	DXF_DIG	layers[MAX_FILES];
	DXF_DIG *closed_layers;
	int     from_table;
	int	XMAX,XMIN,YMAX,YMIN;
	int	BOUNDARIES;
#else
	extern char	*dxf_file;
	extern long	file_size;
	extern int    	percent;
	extern char	dig_path[240];
	extern char	basename[100];
	extern POINT	*pt_array;
	extern int	arr_size;
	extern int	arr_max;
	extern double	n, s, e, w;
	extern long	n_off, s_off, e_off, w_off;
	extern char	zzero[8];
	extern char	eeight[8];
	extern char	tten[8];
	extern char	ttwenty[8];
	extern char	eelev[8];
	extern char	ttwentyone[8];
	extern char	entitie[12];
	extern char	header[12];
	extern char	extmin[12];
	extern char 	extmax[12];
	extern char	polyline[12];
	extern char	circle[12];
	extern char	text[12]; 	/* dpg */
	extern char	line[8];
	extern char	point[8];
	extern char	arc[8];
	extern char	vertex[8];
	extern char	seqend[8];
	extern char	dxf_line[80];
	extern int	num_open_layers;
	extern int	num_closed_layers;
	extern DXF_DIG	layers[MAX_FILES];
	extern DXF_DIG  *closed_layers;
	extern int      from_table;
	extern int	XMAX,XMIN,YMAX,YMIN;
	extern int	BOUNDARIES;
#endif


char *remap ();
char *dxf_fgets ();

