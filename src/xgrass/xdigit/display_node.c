/*
**  Written by Dave Gerdes  5/1988
**  US Army Construction Engineering Research Lab
*/

#include "digit.h"

display_node (node, map)
    int node;
    struct Map_info *map;
{
    P_NODE *Node;

    Node = &(map->Node[node]);
    if (NODE_ALIVE (Node))
    {
	standard_color( dcolors[dig_node_color (Node->n_lines)]);
	_Blot (&(Node->x), &(Node->y));
    }
}

highlight_node (node, map)
    int node;
    struct Map_info *map;
{
    P_NODE *Node;

    Node = &(map->Node[node]);
    if (NODE_ALIVE (Node))
    {
	standard_color( dcolors[CLR_HIGHLIGHT]);
	_Blot (&(Node->x), &(Node->y));
    }
}

color_node (node, map, color)
    int node;
    struct Map_info *map;
    int color;
{
    P_NODE *Node;

    Node = &(map->Node[node]);
    if (NODE_ALIVE (Node))
    {
	standard_color( dcolors[color]);
	_Blot (&(Node->x), &(Node->y));
    }
}

erase_node (node, map)
    int node;
    struct Map_info *map;
{
    P_NODE *Node;

    Node = &(map->Node[node]);
    if (NODE_ALIVE (Node))
    {
	standard_color( dcolors[CLR_ERASE]);
	_Blot (&(Node->x), &(Node->y));
    }
}
