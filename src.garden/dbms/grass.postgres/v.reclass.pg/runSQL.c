#include "gis.h"

extern char dbkey[1024];
extern char dbcol[1024];

runSQL(sqlin, input, output, key, col, ALLOPTS, vtype, disolve )
  char *sqlin,*input, *output, *key, *col, *vtype;
  int ALLOPTS, disolve ;
  {
    FILE *fp, *fpin;
    char sysbuf[1024];		/* buffer to issue system cmd 	*/
    int outFlag, readFlag ;
    char *tmpfile_out;

    tmpfile_out = G_tempfile();
    outFlag = readFlag =0;

           

        /* Open file for SQL commands */
        if((fp = fopen(sqlin,"r")) == NULL) {
            fprintf(stderr, "File read error on select file (%s)\n",sqlin);
            exit(-1);
           }



/************************ BEGIN SQL Processing ************************/

	/* Execute SQL command				*/
/*        
	sprintf(sysbuf,"isql %s %s > %s",
		G__getenv("DATABASE"),sqlin,tmpfile_out);
*/	
	sprintf(sysbuf,"psql -tq -c \" ` cat %s ` \" %s > %s",
		sqlin, G__getenv("PG_DBASE"), tmpfile_out);
 
	
	system(sysbuf);

	/* Check SQL output type row or column and -s flag options	*/
	if ((outFlag = row_or_col(tmpfile_out, ALLOPTS)) < 1)
		{ fprintf(stderr, "Row/Column error on SQL output.\n"); exit(-1); }

	if (ALLOPTS == 0) { key=dbkey; col=dbcol; } 
	if (readFlag = (readSQLout(tmpfile_out, outFlag, key, col, 
			input, output, vtype, disolve)) != 0)
		{ fprintf(stderr, "Read error on SQL output.\n"); exit(-1); }

	return(readFlag) ;
}	