/*
 * Copyright (C) 1994. James Darrell McCauley.  (darrell@mccauley-usa.com)
 * 	                                        http://mccauley-usa.com/
 *
 * This program is free software under the GPL (>=v2)
 * Read the file GPL.TXT coming with GRASS for details.
 */

extern const char *plot_program;

#define GREATER_THAN 1
#define LESS_THAN -1

#define NOTRIM -999

char **cdpath ();
int dblcompare ();
void save_plot ();
void zread ();
double inverse_normal ();
double stdnorm ();
