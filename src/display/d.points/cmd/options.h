#ifndef GLOBAL
#define GLOBAL extern
#endif

GLOBAL int color ;
GLOBAL int size ;
GLOBAL int type ;
GLOBAL FILE *infile ;
GLOBAL char *temp_name;
GLOBAL FILE *temp_file;

#define TYPE_X  1
#define TYPE_PLUS   2
#define TYPE_BOX    3
#define TYPE_DIAMOND    4
