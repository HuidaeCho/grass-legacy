/*
 * January, 1991	Agricultural Engineering, Purdue University
 * Raghavan Srinivasan (srin@ecn.purdue.edu)
 * 
 * int cell_open_new(name)
 * 
 * To open a new map with name and in the current mapset and returns the file
 * id.
 */

#include "gis.h"

int cell_open_new (name)
  char *name;
{
  int fd, G_open_cell_new ();

  fd = G_open_cell_new (name);
  if (fd < 0)
    printf ("oops - can`t open cell file %s\n", name);

  return (fd);
}
