#include "local_proto.h"
int 
diamond (int x, int y)
{
    static char form[7][7] =
    {
	{0,0,0,1,0,0,0},
	{0,0,1,1,1,0,0},
	{0,1,1,1,1,1,0},
	{1,1,1,1,1,1,1},
	{0,1,1,1,1,1,0},
	{0,0,1,1,1,0,0},
	{0,0,0,1,0,0,0}
    } ;

    int i,j;

    y -= 4;
    x -= 4;

    for (i=0; i < 7; i++, x-= 7, y++)
	for (j = 0; j < 7; j++, x++)
	    if (form[i][j])
		dot(x,y);

  return 0;
}
