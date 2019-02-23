#include "digit.h"

shuffle_dots (map)
    struct Map_info *map;
{
    P_LINE *Line;
    register int hi, lo;

    Line = map->Line;
    lo = 0;
    hi = map->n_lines + 1;

    while (1)
    {
	lo = next_dot (map, lo);
	hi = next_undot (map, hi);

	if (lo > map->n_lines || hi < 1)
	    break;
	if (lo >= hi)
	    break;
	line_swap (map, lo, hi);
    }
}

static
next_dot (map, lo)
    struct Map_info *map;
{
    while (++lo)
    {
	if (lo > map->n_lines || map->Line[lo].type == DOT)
	    break;
    }
    return (lo);
}

static
next_undot (map, hi)
    struct Map_info *map;
{
    while (--hi)
    {
	if (map->Line[hi].type != DOT)
	    break;
    }
    return (hi);
}
