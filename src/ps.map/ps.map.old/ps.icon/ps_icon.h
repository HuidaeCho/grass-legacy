/* File: ps_icon.h
**
** This is the header file for the ps.icon program.
**
** Author: Paul W. Carlson	May 1992
*/

#include <stdio.h>

#define MAX_POINTS	50

struct ps_icon {
    int xp[MAX_POINTS], yp[MAX_POINTS];
    int points;
    int cx, cy;
    int x_max, y_max, x_min, y_min;
    int file_exists;
    char *title;
    FILE *fp;
};

#ifdef MAIN
struct ps_icon icon;
#else
extern struct ps_icon icon;
#endif
/* draw_grid.c */
int draw_grid(int, int, int, int);
/* icon_file.c */
int open_icon_file(char *);
/* make_icon.c */
int make_icon(void);
/* save_icon.c */
int save_icon(void);
/* show_icon.c */
int show_icon(void);
