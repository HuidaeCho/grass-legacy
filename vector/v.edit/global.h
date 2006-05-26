#ifndef _V_EDIT_
# define _V_EDIT_

# ifdef MAIN
#  define global
# else
#  define global extern
# endif

# include <stdio.h>
# include <stdlib.h>
# include <grass/gis.h>
# include <grass/Vect.h>
# include <grass/glocale.h>

enum mode {
    MODE_ADD,
    MODE_DEL,
    MODE_MOVE,
    MODE_MERGE
};

void help(const char *msg);
int parser(int argc, char*argv[]);

void cat_max_set ( int field, int cat);
int cat_max_get ( int field );
void cat_init(struct Map_info *Map);
int add_line(struct Map_info *Map, int type, struct line_pnts *Points, int cat);

global struct Option *map_opt, *act_opt, *typ_opt, *cat_opt, *pnt_opt;
global struct Flag *n_flg;
global struct GModule *module;
global enum mode action_mode;
global char *mapset;

#endif
