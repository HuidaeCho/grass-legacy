#include "P.h"
Ptext (s) char *s;
{
    while (*s >= ' ' && *s < 0177)
	Poutc (*s++);
    Poutc('\r');
    Poutc('\n');
}
