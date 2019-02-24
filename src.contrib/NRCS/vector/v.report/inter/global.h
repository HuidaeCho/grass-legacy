#include "gis.h"

struct units
{
    char *name;
    char *code;
    char marked[2];
};
#ifdef GLOBAL
struct units units[] = {
	{"meters","me"},
	{"kilometers","k"},
	{"feet","f"},
	{"miles","mi"},
	{"acres","a"},
	{"hectacres","h"},
	{"counts","c"},
        {NULL,NULL}};
#else
extern struct units units[];
#endif

#ifndef GLOBAL
#define GLOBAL extern
#else
#define GLOBAL
#endif


GLOBAL char *stats_file;
GLOBAL char *report_file;
GLOBAL int nlayers;
GLOBAL struct layer { char *name; char *mapset; char *type;} *layer;
