#include "raster.h"
#include <stdio.h>
#include "gis.h"
#include "monitors.h"
#include "open.h"

int 
main (int argc, char *argv[])
{
    struct MON_CAP *cap;
    struct MON_CAP *R_parse_monitorcap();
    char *status;
    char *fmt = "%-15s %-30s %s\n";

    fprintf (stdout,fmt, "name","description","status");
    fprintf (stdout,fmt, "----","-----------","------");
    while ((cap = R_parse_monitorcap(MON_NEXT,"")) != NULL)
    {
        G__setenv("MONITOR",cap->name);
        R__open_quiet();
        switch(R_open_driver())
        {
        case OK:
            status = "running";
            R_close_driver();
            R_release_driver();
            break;
        case NO_RUN:
            status = "not running";
            break;
        case LOCKED:
            status = "in use";
            break;
        default:
            status = "??";
            break;
        }                   /* switch */
        fprintf (stdout,fmt,cap->name,cap->comment,status);
    }

    return 0;
}
