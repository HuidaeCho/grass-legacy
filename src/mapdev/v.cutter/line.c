/**** line.c ****/
/*
**  Written by David Gerdes  Fall 1992
**  US Army Construction Engineering Research Lab
**  Copyright  David Gerdes  USA-CERL  1992
*/



#include <stdlib.h>
#include <stdio.h>
#include "cutter.h"

int main (int argc, char *argv[])
{
  struct line_t L1;
  struct point_t P;
  double x, y;
  int ret;

      L1.p1.x = atof (argv[1]);
      L1.p1.y = atof (argv[2]); 
      L1.p2.x = atof (argv[3]);
      L1.p2.y = atof (argv[4]);
      P.x = atof (argv[5]); 
      P.y = atof (argv[6]);

      ret = point_to_right_of_segment (&L1, &P);

      fprintf (stdout," return =  %2d: %s\n", ret, ret ? "RIGHT" : "LEFT");

      return !ret;
}
