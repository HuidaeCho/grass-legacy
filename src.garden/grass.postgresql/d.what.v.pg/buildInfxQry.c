#include "gis.h"
#include <stdio.h> 

int buildInfxQry(ktab,keycat,curcat)
	struct Option *ktab, *keycat; 
	int curcat;
{

	
	static char SQL_stmt[1024] ;

	
	snprintf(SQL_stmt, 1024,
          "SELECT * from %s where %s=%d",ktab->answer,keycat->answer, curcat);
	
	return ((int) SQL_stmt) ;

}


