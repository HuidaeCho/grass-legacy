#include <stdio.h>
#include "vask.h"
#include "config.h"

int setup (char *file, int new)
{
    static char title[80];

    V_clear();
    sprintf(title, "PAINT LABELS: %-25s %s labels", file, new?"new":"existing");
    V_line (1, title);

    V_line (TEXT,      "TEXT:                                                       SKIP:");
    V_line (EAST,      "LOCATION:  EAST:                      OFFSET:");
    V_line (NORTH,     "          NORTH:                      OFFSET:");
    V_line (REF,       "      PLACEMENT:");
    V_line (FONT,      "FONT:");
    V_line (SIZE,      "TEXT SIZE:");
    V_line (COLOR,     "TEXT COLOR:                             WIDTH:");
    V_line (HCOLOR,    "HIGHLIGHT COLOR:                        WIDTH:");
    V_line (BACKGROUND,"BACKGROUND COLOR:");
    V_line (BORDER,    "BORDER COLOR:");
    V_line (OPAQUE,    "OPAQUE TO VECTORS:");

    return 0;
}
