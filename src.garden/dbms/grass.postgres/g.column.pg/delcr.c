#include <stdio.h>

delcr(str)   /* char *str   */
   char str[80];
{

        str[strlen(str)-1] = NULL;
	printf("%s",str);
}
	
