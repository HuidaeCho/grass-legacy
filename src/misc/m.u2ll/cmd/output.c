#include <stdio.h>
#include <unistd.h>

int 
output (char *buf)
{
    fprintf (stdout,"%s\n", buf);
    if (isatty(0) && !isatty(1))
	fprintf (stderr, "%s\n", buf);

  return 0;
}
