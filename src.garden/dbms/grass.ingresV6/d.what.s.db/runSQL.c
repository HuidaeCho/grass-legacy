/* runSQL - read user supplied SQL query, execute and print output
 
   INGRES modifications - New name for header file:
				ingres.h <=> infx.h

			  '\g' added to query

			  Syntax for execution of SQL command modified

   Katarina Johnsson, 930421
*/

#include "gis.h"
#include "ingres.h"


runSQL(sqlin, pts )
	char *sqlin;
        struct Sql *pts;
{
	FILE *fpin, *fpout;
	char buf[1024];
	char ch ;
	char sqlFile[100] ;
	int i;

	i = 1;

	sprintf(sqlFile, "/tmp/%d.sql",getpid() ) ;

        /* Open file to read SQL commands */
        if((fpin = fopen(sqlin,"r")) == NULL) {
            fprintf(stderr, "File read error on temporary file (sql).\n");
            exit(-1);
           }

        /* Open file to write SQL commands  with cahracter substitution */
        if((fpout = fopen(sqlFile,"w")) == NULL) {
            fprintf(stderr, "File write error on temporary file (sql output).\n");
            exit(-1);
           }


        /* Read SQL input and do character substitution */
        while (!feof(fpin) ) {
                ch = getc(fpin);
                if(ch == '?') {
			fprintf(fpout,"%f",(double)getVal(i,pts));
			i++;
		}
                else {
			fprintf(fpout,"%c",ch);
		}
        }
	/* added for INGRES, KJ 930421 */
	fprintf(fpout,"\\g\n");

        fclose(fpout);
        fclose (fpin);

	printf("Querying database ...\n");

	/* Syntax modified for INGRES, KJ 930421 */	
	sprintf(buf,"sql -s %s < %s 2>/dev/null", G_getenv("DATABASE"), sqlFile);

	/* Use the following to see DB output number of rows returned 
	sprintf(buf,"isql %s  %s 2>&1", G_getenv("DATABASE"), sqlFile);
	*/

	if(isatty(1)) strcat (buf," | more");
	system(buf);


	unlink(sqlFile) ;

	return 0 ;
}


getVal(curval, pts)
	int curval;
        struct Sql *pts;
{
        switch (curval) {
        	case 1:
			return (pts->centX);
			break;
                case 2:
                        return (pts->centX);
			break;
                case 3:
                        return (pts->centY);
			break;
                case 4:
                        return (pts->centY);
			break;
                case 5:
                        return (pts->rad2);
			break;
	}

}
