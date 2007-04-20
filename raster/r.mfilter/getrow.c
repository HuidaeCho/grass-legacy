#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <grass/gis.h>
#include "glob.h"
#include "local_proto.h"

int getmaprow (int fd, void *buf, int row, int len)
{
    if (G_get_map_row (fd, (CELL *) buf, row) < 0)
	exit(1);
    return 1;
}

int getrow (int fd, void *buf, int row, int len)
{
    if (direction > 0)
	lseek (fd, (off_t) row*len, 0);
    else
	lseek (fd, (off_t) (nrows-row-1)*len, 0);
    if (read (fd, (CELL *) buf, len) != len)
	G_fatal_error ("error reading temporary file");
    return 1;
}
